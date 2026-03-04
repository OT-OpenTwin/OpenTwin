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
            TopoDS_Shape exportShape = normalizeToSingleSolidOrOriginal(brepEntity->getBrep(), /*sewingTol*/ 1e-4);

			TDF_Label label = shapeTool->AddShape(exportShape);

			std::string name = brepNameMap[brepItem.getEntityID()];
			TDataStd_Name::Set(label, TCollection_ExtendedString(name.c_str()));

            std::vector<double> color = brepColorMap[brepItem.getEntityID()];
            assert(color.size() == 3);
			Quantity_Color col(color[0], color[1], color[2], Quantity_TOC_RGB);
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

TopoDS_Shape STEPWriter::normalizeToSingleSolidOrOriginal(const TopoDS_Shape& inShape, double sewingTol, bool doUnifySameDomain, bool unifyEdges, bool unifyFaces, bool concatBSplines, bool requireValid)
{
    // Normalize an OCCT shape to a *single* valid Solid if possible.

    // Requested behavior:
    //  - Runs UnifySameDomain (optional) to merge same-domain faces/edges (improves topology).
    //  - If input contains multiple solids -> returns the ORIGINAL input shape.
    //  - If conversion fails -> returns the ORIGINAL input shape.

    // Notes:
    //  - Return type is TopoDS_Shape so we can return either a Solid or the original shape.
    //  - UnifySameDomain flags are passed via ctor/Initialize (OCCT API), not via setters.
    
    auto isGoodSolid = [&](const TopoDS_Solid& s) -> bool
        {
            if (s.IsNull())
                return false;

            if (requireValid)
            {
                // Basic topology + geometry validity check
                BRepCheck_Analyzer ana(s, /*GeomControls*/ true);
                if (!ana.IsValid())
                    return false;
            }
            return true;
        };

    auto extractSingleSolid = [&](const TopoDS_Shape& s, TopoDS_Solid& outSolid) -> int
        {
            int count = 0;
            for (TopExp_Explorer ex(s, TopAbs_SOLID); ex.More(); ex.Next())
            {
                ++count;
                if (count == 1)
                    outSolid = TopoDS::Solid(ex.Current());
            }
            return count;
        };

    // 0) Optional: unify same-domain faces/edges first
    TopoDS_Shape shape = inShape;
    if (doUnifySameDomain && !shape.IsNull())
    {
        // OCCT API: flags are passed in ctor (or Initialize), then Build(), then Shape().
        ShapeUpgrade_UnifySameDomain unify(shape, unifyEdges, unifyFaces, concatBSplines);
        // Optional tolerances (keep defaults unless you know you need them):
        // unify.SetLinearTolerance(1e-6);
        // unify.SetAngularTolerance(Precision::Angular());

        unify.Build();
        const TopoDS_Shape& unified = unify.Shape();
        if (!unified.IsNull())
            shape = unified;
        // If it returns null (rare), silently continue with original.
    }

    // 1) If the shape is already a solid, accept it if it's valid
    if (shape.ShapeType() == TopAbs_SOLID)
    {
        TopoDS_Solid s = TopoDS::Solid(shape);
        if (isGoodSolid(s))
            return s;
        // If invalid, try to rebuild below; if that fails we return original.
    }

    // 2) If it contains solids: accept only if EXACTLY ONE solid is present
    {
        TopoDS_Solid s;
        int n = extractSingleSolid(shape, s);
        if (n == 1 && isGoodSolid(s))
            return s;

        if (n > 1)
            return inShape; // requested: multiple solids -> return original
    }

    // 3) Shell -> Solid (try each shell; accept first good solid)
    for (TopExp_Explorer ex(shape, TopAbs_SHELL); ex.More(); ex.Next())
    {
        TopoDS_Shell shell = TopoDS::Shell(ex.Current());
        BRepBuilderAPI_MakeSolid mkSolid(shell);
        if (!mkSolid.IsDone())
            continue;

        TopoDS_Solid s = mkSolid.Solid();
        if (isGoodSolid(s))
            return s;
    }

    // 4) Sewing -> shells -> solid
    {
        BRepBuilderAPI_Sewing sew(sewingTol);
        sew.Add(shape);
        sew.Perform();

        TopoDS_Shape sewed = sew.SewedShape();
        if (sewed.IsNull())
            return inShape;

        // Unify again after sewing (often helps)
        if (doUnifySameDomain)
        {
            ShapeUpgrade_UnifySameDomain unify(sewed, unifyEdges, unifyFaces, concatBSplines);
            unify.Build();
            const TopoDS_Shape& unified = unify.Shape();
            if (!unified.IsNull())
                sewed = unified;
        }

        // If sewing produced solids, enforce "exactly one"
        {
            TopoDS_Solid s;
            int n = extractSingleSolid(sewed, s);
            if (n == 1 && isGoodSolid(s))
                return s;

            if (n > 1)
                return inShape; // requested: multiple solids -> original
        }

        // Otherwise try shells from the sewed result
        for (TopExp_Explorer ex(sewed, TopAbs_SHELL); ex.More(); ex.Next())
        {
            TopoDS_Shell shell = TopoDS::Shell(ex.Current());
            BRepBuilderAPI_MakeSolid mkSolid(shell);
            if (!mkSolid.IsDone())
                continue;

            TopoDS_Solid s = mkSolid.Solid();
            if (isGoodSolid(s))
                return s;
        }
    }

    // 5) Failed to normalize -> return original
    return inShape;
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