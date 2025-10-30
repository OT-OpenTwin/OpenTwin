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
#include <BinXCAFDrivers.hxx>
#include <STEPCAFControl_Writer.hxx>
#include <IFSelect_ReturnStatus.hxx>
#include <TDataStd_Name.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <TDocStd_Application.hxx>

STEPWriter::STEPWriter() {
    Handle(TDocStd_Application) app = new TDocStd_Application;
    BinXCAFDrivers::DefineFormat(app);
    app->NewDocument("BinXCAF", doc);
    shapeTool = XCAFDoc_DocumentTool::ShapeTool(doc->Main());
    colorTool = XCAFDoc_DocumentTool::ColorTool(doc->Main());
}

void STEPWriter::addShape(TopoDS_Shape& shape, int colorIndex, int id, bool type) {
    TDF_Label label = shapeTool->AddShape(shape, false);

    Quantity_Color color = COLORS[colorIndex % COLORS.size()];
    colorTool->SetColor(label, color, XCAFDoc_ColorGen);
    std::string name;
    if (type) 
        name = "layer" + std::to_string(id);
    else
        name = "substrate" + std::to_string(id);
    TDataStd_Name::Set(label, name.c_str());
}

bool STEPWriter::writeSTEP(std::string filename) {
    STEPCAFControl_Writer writer;
    if (!writer.Transfer(doc)) {
        return false;
    }
    return writer.Write(filename.c_str()) == IFSelect_RetDone;
}
