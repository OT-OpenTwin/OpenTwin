// @otlicense
// File: STEPWriter.h
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

#ifndef STEPWRITER_H
#define STEPWRITER_H

#include <TDocStd_Document.hxx>
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>
#include <Quantity_Color.hxx>
#include <TopoDS_Shape.hxx>

class STEPWriter {
public:
    STEPWriter();
    void addShape(TopoDS_Shape& shape, int colorIndex, int id, bool type);
    bool writeSTEP(std::string filename);
private:
    const std::vector<Quantity_Color> COLORS = {
        Quantity_Color(Quantity_NOC_RED),
        Quantity_Color(Quantity_NOC_GREEN),
        Quantity_Color(Quantity_NOC_BLUE),
        Quantity_Color(Quantity_NOC_YELLOW),
        Quantity_Color(Quantity_NOC_CYAN),
        Quantity_Color(Quantity_NOC_MAGENTA)
    };

    Handle(TDocStd_Document) doc;
    Handle(XCAFDoc_ShapeTool) shapeTool;
    Handle(XCAFDoc_ColorTool) colorTool;
};

#endif // STEPWRITER_H
