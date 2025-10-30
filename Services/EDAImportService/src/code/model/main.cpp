// @otlicense
// File: main.cpp
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

//#include "Viewer.h"
//#include "IC.h"
//#include "Layer.h"
//#include "Polygon.h"
//#include <BRepAlgoAPI_Fuse.hxx>
//#include <BRepBuilderAPI_MakeFace.hxx>
//#include <BRepBuilderAPI_MakePolygon.hxx>
//#include <BRepPrimAPI_MakePrism.hxx>
//#include <STEPControl_Writer.hxx>
//#include <list>
//
//#include "ICFactory.h"
//
//// polygons
//BRepBuilderAPI_MakePolygon make_polygon(std::list<gp_Pnt> pnts);
//std::list<BRepBuilderAPI_MakePolygon> make_polygons(std::list<std::list<gp_Pnt>> pnts);
//
//// faces
//BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon polygon);
//std::list<BRepBuilderAPI_MakeFace> make_faces(std::list<BRepBuilderAPI_MakePolygon> polygons);
//
//// fuse faces
//BRepAlgoAPI_Fuse fuse_faces(std::list<BRepBuilderAPI_MakeFace> faces);
//
//std::list<BRepPrimAPI_MakePrism> create_IC(IC ic);
//
////TEST
//BRepPrimAPI_MakePrism test1();
//IC testIC();
//
//
//int main(int argc, char** argv)
//{
//	std::string path = R"(C:\Users\Alici\Documents\Projekt Digitalisierung - Digital Twins\gdsii\xor.gds)";
//	IC new_IC = ICFactory::generateIC("Test", path);
//
//	// create window
//	Viewer vout(50, 50, 500, 500);
//
//	//TEST
//	//BRepPrimAPI_MakePrism prism = test1();
//
//	//IC myic = testIC();
//	std::list<BRepPrimAPI_MakePrism> prisms = create_IC(new_IC);
//
//	// visualize
//	for(auto prism: prisms)
//	{
//		vout << prism;
//	}
//
//	//STEPControl_Writer writer;
//	//writer.Transfer(prisms.front().Shape(), STEPControl_ManifoldSolidBrep);
//	//writer.Transfer(prism.Shape(), STEPControl_ManifoldSolidBrep);
//	//writer.Write("Output.stp");
//
//	vout.StartMessageLoop();
//	return 0;
//}
//
//
//
//BRepBuilderAPI_MakePolygon make_polygon(std::list<gp_Pnt> pnts)
//{
//	BRepBuilderAPI_MakePolygon polygon;
//
//	for (auto itr: pnts) {
//		polygon.Add(itr);
//	}
//
//	return polygon;
//}
//
//std::list<BRepBuilderAPI_MakePolygon> make_polygons(std::list<std::list<gp_Pnt>> pnts)
//{
//	std::list<BRepBuilderAPI_MakePolygon> polygons;
//
//	for (auto itr: pnts)
//	{
//		polygons.push_back(make_polygon(itr));
//	}
//	return polygons;
//}
//
//BRepBuilderAPI_MakeFace make_face(BRepBuilderAPI_MakePolygon polygon)
//{
//	return BRepBuilderAPI_MakeFace(polygon.Wire(), Standard_True);
//}
//
//std::list<BRepBuilderAPI_MakeFace> make_faces(std::list<BRepBuilderAPI_MakePolygon> polygons)
//{
//	std::list<BRepBuilderAPI_MakeFace> faces;
//
//	for (auto itr: polygons) {
//		faces.push_back(make_face(itr));
//	}
//
//	return faces;
//}
//
//BRepAlgoAPI_Fuse fuse_faces(std::list<BRepBuilderAPI_MakeFace> faces)
//{
//	BRepAlgoAPI_Fuse fused_face(faces.front().Shape(), (++faces.begin())->Shape());
//
//	int i = 0;
//	for (auto itr: faces)
//	{
//		if (i >= 2)
//		{
//			fused_face = BRepAlgoAPI_Fuse(fused_face.Shape(), itr.Shape());
//		}
//		i++;
//	}
//	return fused_face;
//}
//
//std::list<BRepPrimAPI_MakePrism> create_IC(IC ic)
//{
//	float height = 0;
//	std::list<BRepPrimAPI_MakePrism> prisms{};
//
//	for (auto layer : ic.getLayers())
//	{
//		std::list<std::list<gp_Pnt>> pnts_of_layer{};
//		height += 150/*layer.getHeight()*/;
//
//		for (auto polygon : layer.getPolygons())
//		{
//			std::list<gp_Pnt> pnts_of_polygon{};
//
//			for (auto point : polygon.getCoordinates())
//			{
//				gp_Pnt pnt(point.first, point.second, height);
//				pnts_of_polygon.push_back(pnt);
//			}
//			pnts_of_layer.push_back(pnts_of_polygon);
//		}
//
//		std::list<BRepBuilderAPI_MakePolygon> polygons = make_polygons(pnts_of_layer);
//		std::list<BRepBuilderAPI_MakeFace> faces = make_faces(polygons);
//		BRepAlgoAPI_Fuse* fused_face = new BRepAlgoAPI_Fuse(fuse_faces(faces));
//		BRepPrimAPI_MakePrism prism(fused_face->Shape(), gp_Vec(0.0, 0.0, /*layer.getThickness()*/100));
//
//		//delete fused_face;
//		//fused_face = nullptr;
//
//		prisms.push_back(prism);
//	}
//
//	return prisms;
//}
//
//
//
//
//BRepPrimAPI_MakePrism test1()
//{
//	// create points
//	std::list<gp_Pnt> pnts{
//		gp_Pnt(60, -20, 0.0),
//		gp_Pnt(60, -80, 0.0),
//		gp_Pnt(80, -80, 0.0),
//		gp_Pnt(80, -20, 0.0),
//		gp_Pnt(60, -20, 0.0) };
//	std::list<gp_Pnt> pnts2{
//		gp_Pnt(40, -20, 0.0),
//		gp_Pnt(40, -100, 0.0),
//		gp_Pnt(80, -100, 0.0),
//		gp_Pnt(80, -80, 0.0),
//		gp_Pnt(60, -80, 0.0),
//		gp_Pnt(60, -20, 0.0),
//		gp_Pnt(40, -20, 0.0) };
//	std::list<gp_Pnt> pnts3{
//		gp_Pnt(56,-20, 0),
//		gp_Pnt(64,-20, 0) ,
//		gp_Pnt(64,-28, 0) ,
//		gp_Pnt(56,-28, 0),
//		gp_Pnt(56,-20, 0) };
//
//	std::list<std::list<gp_Pnt>> points{ pnts, pnts2, pnts3 };
//
//	// create polygons from points
//	std::list<BRepBuilderAPI_MakePolygon> polygons(make_polygons(points));
//
//	// create face from polygons
//	std::list<BRepBuilderAPI_MakeFace> faces = make_faces(polygons);
//
//	// create fused_face from faces
//	//BRepAlgoAPI_Fuse fused_face = fuse_faces(faces);
//
//	// create prism from fused_face with height z
//	double z = 20.0;
//	//TopoDS_Shape s = fused_face.Shape();
//
//	TopoDS_Shape s = fuse_faces(faces).Shape();
//	if (s.IsNull()) {
//		std::cerr << "Fehler: Null Shape" << std::endl;
//		throw std::runtime_error("Null Shape");
//	}
//
//	BRepPrimAPI_MakePrism prism(s, gp_Vec(0.0, 0.0, z), true); // memory management
//
//	return prism;
//}
//
//IC testIC()
//{
//	// vectoren erstellen
//	std::vector<std::pair<int, int>> v1;
//	v1.emplace_back(60, -20);
//	v1.emplace_back(60, -80);
//	v1.emplace_back(80, -80);
//	v1.emplace_back(80, -20);
//	v1.emplace_back(60, -20);
//
//	std::vector<std::pair<int, int>> v2;
//	v2.emplace_back(40, -20);
//	v2.emplace_back(40, -100);
//	v2.emplace_back(80, -20);
//	v2.emplace_back(80, -80);
//	v2.emplace_back(60, -80);
//	v2.emplace_back(60, -20);
//
//	std::vector<std::pair<int, int>> v3;
//	v3.emplace_back(56, -20);
//	v3.emplace_back(64, -20);
//	v3.emplace_back(64, -28);
//	v3.emplace_back(56, -28);
//	v3.emplace_back(56, -20);
//
//	MyPolygon p1(1, v1);
//	MyPolygon p2(1, v2);
//	MyPolygon p3(2, v3);
//
//	std::vector<MyPolygon> lp1;
//	lp1.emplace_back(p1);
//	lp1.emplace_back(p2);
//	std::vector<MyPolygon> lp2;
//	lp2.emplace_back(p3);
//
//	Layer l1(1, lp1, 50, 0);
//	Layer l2(2, lp2, 70, 70);
//
//	std::vector<Layer> vl1;
//	vl1.emplace_back(l1);
//	vl1.emplace_back(l2);
//
//	IC ic("myIC", vl1);
//
//	return ic;
//}
//
