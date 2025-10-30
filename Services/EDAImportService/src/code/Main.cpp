// @otlicense
// File: Main.cpp
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

#include "Viewer.h"
#include "IC.h"
#include "ICFactory.h"
#include "IC3DCreator.h"
#include "STEPWriter.h"

#include <STEPCAFControl_Writer.hxx>

//// polygons
//BRepBuilderAPI_MakePolygon make_polygon(std::list<gp_Pnt>& pnts);
//std::list<BRepBuilderAPI_MakePolygon> make_polygons(std::list<std::list<gp_Pnt>>& pnts);
//
//// faces
//BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon& polygon);
//std::list<BRepBuilderAPI_MakeFace> make_faces(std::list<BRepBuilderAPI_MakePolygon>& polygons);
//
//// fuse faces
//BRepAlgoAPI_Fuse fuse_faces(std::list<BRepBuilderAPI_MakeFace>& faces);
//
//// offset
//BRepBuilderAPI_MakeFace ConvertCurvesToLines(const TopoDS_Shape& shape);
//BRepBuilderAPI_MakeFace create_offset(std::vector<std::pair<int, int>> points, int width, int path_type);
//
//void create_IC(IC ic, Handle(XCAFDoc_ShapeTool)& shape_tool, Handle(XCAFDoc_ColorTool)& color_tool, Viewer& vout);
//
//
//
//const std::vector<Quantity_Color> COLORS = {
//		Quantity_Color(Quantity_NOC_RED),
//		Quantity_Color(Quantity_NOC_GREEN),
//		Quantity_Color(Quantity_NOC_BLUE),
//		Quantity_Color(Quantity_NOC_YELLOW),
//		Quantity_Color(Quantity_NOC_CYAN),
//		Quantity_Color(Quantity_NOC_MAGENTA)
//};
//
//
//
//bool WriteSTEP(const Handle(TDocStd_Document)& doc, const char* filename)
//{
//	STEPCAFControl_Writer Writer;
//	if (!Writer.Transfer(doc))
//		return false;
//	if (Writer.Write(filename) != IFSelect_RetDone)
//		return false;
//
//	return true;
//}

int main(int argc, char** argv)
{
	Viewer viewer(50, 50, 500, 500);

	STEPWriter stepWriter;

	std::string path = R"(C:\Users\Valentin\OneDrive\Info\Master\SoSe 24\Projekt Digitalization\Example Files\gdsii\nand2.gds)";
	std::string stackUp = R"(C:\Users\Valentin\OneDrive\Info\Master\SoSe 24\Projekt Digitalization\Example Files\gdsii\SDFFRS_X2.csv)";
	//std::string stackUp = R"(C:\Users\Alici\Documents\Projekt Digitalisierung - Digital Twins\repository\Projekt-Digital-Twin-Model-Convertion\example_files\SDFFRS_X2.CSV)";

	IC newIC = ICFactory::generateIC("Test", path, stackUp);

	IC3DCreator ic3DCreator;
	ic3DCreator.createIC(newIC, stepWriter, viewer);

	stepWriter.writeSTEP("test.stp");

	viewer.StartMessageLoop();
	return 0;

}
// mail: peter_thoma@outlook.com
