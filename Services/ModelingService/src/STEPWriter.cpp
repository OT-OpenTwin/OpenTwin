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
#include "ShapeHealing.h"
#include "Application.h"

#include "OTCommunication/Msg.h"
#include "OTModelEntities/DataBase.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/uiComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "XSControl_WorkSession.hxx"
#include "XSControl_TransferReader.hxx"
#include "StepRepr_RepresentationItem.hxx"
#include "XCAFApp_Application.hxx"
#include "XCAFDoc_DocumentTool.hxx"
#include "XCAFDoc_ShapeTool.hxx"
#include "XCAFDoc_ColorTool.hxx"
#include "TDF_Label.hxx"
#include "TDocStd_Document.hxx"
#include "TDataStd_Name.hxx"
#include "Quantity_Color.hxx"
#include "STEPCAFControl_Reader.hxx"
#include "TopoDS_Iterator.hxx"
#include "BRepBuilderAPI_Transform.hxx"
#include "ShapeFix_Shape.hxx"
#include "ShapeFix_Wireframe.hxx"
#include "ShapeFix_FixSmallFace.hxx"
#include "TopExp_Explorer.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_Solid.hxx"
#include "TopoDS.hxx"
#include "ShapeBuild_ReShape.hxx"
#include "BRep_Tool.hxx"
#include "BRepOffsetAPI_Sewing.hxx"
#include "BRepBuilderAPI_MakeSolid.hxx"
#include "BRepCheck_Analyzer.hxx"
#include "BRepLib.hxx"
#include "GProp_GProps.hxx"
#include "BRepGProp.hxx"
#include "BRepTools.hxx"
#include "Bnd_Box.hxx"
#include "BRepBndLib.hxx"
#include "BRepAdaptor_Surface.hxx"
#include "ShapeUpgrade_ShapeDivideArea.hxx"
#include "ShapeUpgrade_FaceDivideArea.hxx"
#include "ShapeCustom_BSplineRestriction.hxx"
#include "ShapeCustom_RestrictionParameters.hxx"
#include "ShapeCustom.hxx"

#include <iostream>
#include <filesystem>

#include "base64.h"
#include "zlib.h"

void STEPWriter::getExportFileContent(std::string& data)
{
	data = "Hello World!";
}
