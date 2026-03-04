// @otlicense
// File: STEPWriter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "STEPWriter.h"
#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "OTModelEntities/EntityAPI.h"
#include "Application.h"

#include "OTCommunication/Msg.h"
#include "OTModelEntities/DataBase.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/uiComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "STEPCAFControl_Writer.hxx"
#include "TDocStd_Document.hxx"
#include "TDocStd_Application.hxx"

#include "XCAFDoc_DocumentTool.hxx"
#include "XCAFDoc_ShapeTool.hxx"
#include "XCAFDoc_ColorTool.hxx"
#include "XCAFApp_Application.hxx"
#include "BRepBuilderAPI_Copy.hxx"
#include "TDataStd_Name.hxx"
#include "TDF_Label.hxx"
#include "Interface_Static.hxx"
#include "Quantity_Color.hxx"
#include "Quantity_TypeOfColor.hxx"
#include "TopoDS_Shape.hxx"
#include "TopAbs_ShapeEnum.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "TopoDS.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS_Shell.hxx"
#include "BRepTools.hxx"
#include "BRepLib.hxx"
#include "ShapeBuild_ReShape.hxx"

#include "ShapeFix_Shape.hxx"
#include "ShapeFix_Solid.hxx"

#include "BRepBuilderAPI_Sewing.hxx"
#include "BRepCheck_Analyzer.hxx"

#include "ShapeUpgrade_UnifySameDomain.hxx"

#include <iostream>
#include <filesystem>
#include <windows.h>

void STEPWriter::getExportFileContent(std::string& data)
{
	data.clear();

	// Get all geometry entities
	ot::UIDList geometryEntities = getAllGeometryEntities();
	std::list<ot::EntityInformation> geometryInfo;
	ot::ModelServiceAPI::getEntityInformation(geometryEntities, geometryInfo);

	DataBase::instance().prefetchDocumentsFromStorage(geometryInfo);

	ot::UIDList brepIDs;
    std::map<ot::UID, std::string> brepNameMap;
    std::map<ot::UID, std::vector<double>> brepColorMap;

	for (auto geometryItem : geometryInfo)
	{
		EntityGeometry* geomEntity = dynamic_cast<EntityGeometry*>(ot::EntityAPI::readEntityFromEntityIDandVersion(geometryItem.getEntityID(), geometryItem.getEntityVersion()));
		assert(geomEntity != nullptr);

		if (geomEntity != nullptr)
		{
			ot::UID brepID = geomEntity->getBrepStorageObjectID();
			brepNameMap[brepID] = geomEntity->getName();

            EntityPropertiesColor* color = dynamic_cast<EntityPropertiesColor*>(geomEntity->getProperties().getProperty("Color"));
            assert(color != nullptr);
            brepColorMap[brepID] = std::vector<double>{ color->getColorR(), color->getColorG(), color->getColorB() };

			brepIDs.push_back(brepID);

			delete geomEntity;
			geomEntity = nullptr;
		}
	}

	std::list<ot::EntityInformation> brepInfo;
	ot::ModelServiceAPI::getEntityInformation(brepIDs, brepInfo);

	DataBase::instance().prefetchDocumentsFromStorage(brepInfo);

	Handle(XCAFApp_Application) app = XCAFApp_Application::GetApplication();
	Handle(TDocStd_Document) doc;
	app->NewDocument("MDTV-XCAF", doc);

	Handle(XCAFDoc_ShapeTool) shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());

	Handle(XCAFDoc_ColorTool) colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
	Interface_Static::SetCVal("write.step.schema", "AP214");
	Interface_Static::SetIVal("write.step.assembly", 0);

	for (auto brepItem : brepInfo)
	{
		EntityBrep* brepEntity = dynamic_cast<EntityBrep*>(ot::EntityAPI::readEntityFromEntityIDandVersion(brepItem.getEntityID(), brepItem.getEntityVersion()));
		assert(brepEntity != nullptr);

		if (brepEntity != nullptr)
        {
            std::string name = brepNameMap[brepItem.getEntityID()];

            std::vector<double> color = brepColorMap[brepItem.getEntityID()];
            assert(color.size() == 3);
            Quantity_Color col(color[0], color[1], color[2], Quantity_TOC_RGB);

            TopoDS_Shape exportShape = cleanSolid(brepEntity->getBrep());

			TDF_Label label = shapeTool->AddShape(exportShape, Standard_False);

			TDataStd_Name::Set(label, TCollection_ExtendedString(name.c_str()));

			colorTool->SetColor(label, col, XCAFDoc_ColorSurf);
			colorTool->SetColor(label, col, XCAFDoc_ColorGen);

			delete brepEntity;
			brepEntity = nullptr;
		}
	}

	const std::string tmpFile = createTempFile();
	
	STEPCAFControl_Writer writer;
	writer.SetNameMode(true);
	writer.SetColorMode(true);

	writer.Transfer(doc, STEPControl_AsIs);
	writer.Write(tmpFile.c_str());
	
	std::ifstream in(tmpFile, std::ios::binary);
	std::ostringstream dataStream;
	dataStream << in.rdbuf();
	in.close();

    std::filesystem::remove(tmpFile);

	data = dataStream.str();
}

std::list<ot::UID> STEPWriter::getAllGeometryEntities(void)
{
	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_MODEL_GetAllGeometryEntitiesForMeshing, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	std::string response;
	application->getModelComponent()->sendMessage(false, reqDoc, response);

	ot::JsonDocument responseDoc;
	responseDoc.fromJson(response);
	return ot::json::getUInt64List(responseDoc, OT_ACTION_PARAM_MODEL_EntityIDList);
}

std::string STEPWriter::createTempFile()
{
    char tempPath[MAX_PATH];
    char tempFile[MAX_PATH];

    // get temp directory
    GetTempPathA(MAX_PATH, tempPath);

    // create unique temp file name
    GetTempFileNameA(tempPath, "occ", 0, tempFile);

    return std::string(tempFile);
}

TopoDS_Shape STEPWriter::cleanSolid(TopoDS_Shape& shape)
{
    TopoDS_Shape exportShape = shape;

    BRepBuilderAPI_MakeSolid ms;
    int count = 0;
    TopExp_Explorer exp;
    for (exp.Init(exportShape, TopAbs_SHELL); exp.More(); exp.Next()) {
        count++;
        ms.Add(TopoDS::Shell(exp.Current()));
    }

    if (count)
    {
        BRepCheck_Analyzer ba(ms);

        if (ba.IsValid())
        {
			exportShape = ms.Shape();

            TopExp_Explorer exp;
            for (exp.Init(exportShape, TopAbs_SOLID); exp.More(); exp.Next())
            {
                TopoDS_Solid solid = TopoDS::Solid(exp.Current());
                TopoDS_Solid newsolid = solid;
                BRepLib::OrientClosedSolid(newsolid);
                Handle_ShapeBuild_ReShape rebuild = new ShapeBuild_ReShape;
                rebuild->Replace(solid, newsolid);
                TopoDS_Shape newshape = rebuild->Apply(exportShape, TopAbs_COMPSOLID);
                exportShape = newshape;
				break; // We expect only one solid per shape
            }
        }
    }

    return exportShape;
}
