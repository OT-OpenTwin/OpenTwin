// @otlicense
// File: EntityAnnotationData.cpp
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

// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "EntityAnnotationData.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityAnnotationData> registrar(EntityAnnotationData::className());

EntityAnnotationData::EntityAnnotationData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityBase(ID, parent, obs, ms),
	edgeColorRGB { 1.0, 1.0, 1.0 }
{
	
}

EntityAnnotationData::~EntityAnnotationData()
{

}

bool EntityAnnotationData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityAnnotationData::addPoint(double x, double y, double z, double r, double g, double b)
{
	std::array<double, 3> p{ x, y, z };
	std::array<double, 3> rgb{ r, g, b };

	points.push_back(p);
	points_rgb.push_back(rgb);

	setModified();
}

void EntityAnnotationData::addTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double r, double g, double b)
{
	std::array<double, 3> p1{ x1, y1, z1 };
	std::array<double, 3> p2{ x2, y2, z2 };
	std::array<double, 3> p3{ x3, y3, z3 };
	std::array<double, 3> rgb{ r, g, b };

	triangle_p1.push_back(p1);
	triangle_p2.push_back(p2);
	triangle_p3.push_back(p3);
	triangle_rgb.push_back(rgb);

	setModified();
}

void EntityAnnotationData::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(storage);

	// Now we store the particular information about the current object (points and triangles)

	// First the color
	auto color = bsoncxx::builder::basic::array();

	color.append(edgeColorRGB[0]);
	color.append(edgeColorRGB[1]);
	color.append(edgeColorRGB[2]);

	// Now the points
	auto pointX = bsoncxx::builder::basic::array();
	auto pointY = bsoncxx::builder::basic::array();
	auto pointZ = bsoncxx::builder::basic::array();
	auto pointR = bsoncxx::builder::basic::array();
	auto pointG = bsoncxx::builder::basic::array();
	auto pointB = bsoncxx::builder::basic::array();

	for (size_t i = 0; i < points.size(); i++)
	{
		pointX.append(points[i][0]);
		pointY.append(points[i][1]);
		pointZ.append(points[i][2]);
	}

	for (size_t i = 0; i < points_rgb.size(); i++)
	{
		pointR.append(points_rgb[i][0]);
		pointG.append(points_rgb[i][1]);
		pointB.append(points_rgb[i][2]);
	}

	// And finally the triangles
	auto triangle1X = bsoncxx::builder::basic::array();
	auto triangle1Y = bsoncxx::builder::basic::array();
	auto triangle1Z = bsoncxx::builder::basic::array();
	auto triangle2X = bsoncxx::builder::basic::array();
	auto triangle2Y = bsoncxx::builder::basic::array();
	auto triangle2Z = bsoncxx::builder::basic::array();
	auto triangle3X = bsoncxx::builder::basic::array();
	auto triangle3Y = bsoncxx::builder::basic::array();
	auto triangle3Z = bsoncxx::builder::basic::array();
	auto triangleR  = bsoncxx::builder::basic::array();
	auto triangleG  = bsoncxx::builder::basic::array();
	auto triangleB  = bsoncxx::builder::basic::array();

	for (size_t i = 0; i < triangle_p1.size(); i++)
	{
		triangle1X.append(triangle_p1[i][0]);
		triangle1Y.append(triangle_p1[i][1]);
		triangle1Z.append(triangle_p1[i][2]);
	}

	for (size_t i = 0; i < triangle_p2.size(); i++)
	{
		triangle2X.append(triangle_p2[i][0]);
		triangle2Y.append(triangle_p2[i][1]);
		triangle2Z.append(triangle_p2[i][2]);
	}

	for (size_t i = 0; i < triangle_p3.size(); i++)
	{
		triangle3X.append(triangle_p3[i][0]);
		triangle3Y.append(triangle_p3[i][1]);
		triangle3Z.append(triangle_p3[i][2]);
	}

	for (size_t i = 0; i < triangle_rgb.size(); i++)
	{
		triangleR.append(triangle_rgb[i][0]);
		triangleG.append(triangle_rgb[i][1]);
		triangleB.append(triangle_rgb[i][2]);
	}

	// Build the document
	storage.append(
		bsoncxx::builder::basic::kvp("EdgesColor", color),
		bsoncxx::builder::basic::kvp("PointX", pointX),
		bsoncxx::builder::basic::kvp("PointY", pointY),
		bsoncxx::builder::basic::kvp("PointZ", pointZ),
		bsoncxx::builder::basic::kvp("PointR", pointR),
		bsoncxx::builder::basic::kvp("PointG", pointG),
		bsoncxx::builder::basic::kvp("PointB", pointB),
		bsoncxx::builder::basic::kvp("Triangle1X", triangle1X),
		bsoncxx::builder::basic::kvp("Triangle1Y", triangle1Y),
		bsoncxx::builder::basic::kvp("Triangle1Z", triangle1Z),
		bsoncxx::builder::basic::kvp("Triangle2X", triangle2X),
		bsoncxx::builder::basic::kvp("Triangle2Y", triangle2Y),
		bsoncxx::builder::basic::kvp("Triangle2Z", triangle2Z),
		bsoncxx::builder::basic::kvp("Triangle3X", triangle3X),
		bsoncxx::builder::basic::kvp("Triangle3Y", triangle3Y),
		bsoncxx::builder::basic::kvp("Triangle3Z", triangle3Z),
		bsoncxx::builder::basic::kvp("TriangleR",  triangleR),
		bsoncxx::builder::basic::kvp("TriangleG",  triangleG),
		bsoncxx::builder::basic::kvp("TriangleB",  triangleB)
	);
}

void EntityAnnotationData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the specific information about this object

	auto color = doc_view["EdgesColor"].get_array().value;

	auto col = color.begin();
	edgeColorRGB[0] = col->get_double(); col++;
	edgeColorRGB[1] = col->get_double(); col++;
	edgeColorRGB[2] = col->get_double(); col++;

	auto pointX = doc_view["PointX"].get_array().value;
	auto pointY = doc_view["PointY"].get_array().value;
	auto pointZ = doc_view["PointZ"].get_array().value;

	auto px = pointX.begin();
	auto py = pointY.begin();
	auto pz = pointZ.begin();

	size_t numberPoints = std::distance(pointX.begin(), pointX.end());
	assert(numberPoints == std::distance(pointY.begin(), pointY.end()));
	assert(numberPoints == std::distance(pointZ.begin(), pointZ.end()));

	points.resize(numberPoints);
	
	for (unsigned long index = 0; index < numberPoints; index++)
	{
		points[index][0] = px->get_double();
		points[index][1] = py->get_double();
		points[index][2] = pz->get_double();

		px++;
		py++;
		pz++;
	}

	auto pointR = doc_view["PointR"].get_array().value;
	auto pointG = doc_view["PointG"].get_array().value;
	auto pointB = doc_view["PointB"].get_array().value;

	auto pr = pointR.begin();
	auto pg = pointG.begin();
	auto pb = pointB.begin();

	assert(numberPoints == std::distance(pointR.begin(), pointR.end()));
	assert(numberPoints == std::distance(pointG.begin(), pointG.end()));
	assert(numberPoints == std::distance(pointB.begin(), pointB.end()));

	points_rgb.resize(numberPoints);

	for (unsigned long index = 0; index < numberPoints; index++)
	{
		points_rgb[index][0] = pr->get_double();
		points_rgb[index][1] = pg->get_double();
		points_rgb[index][2] = pb->get_double();

		pr++;
		pg++;
		pb++;
	}

	auto triangle1X = doc_view["Triangle1X"].get_array().value;
	auto triangle1Y = doc_view["Triangle1Y"].get_array().value;
	auto triangle1Z = doc_view["Triangle1Z"].get_array().value;

	auto t1x = triangle1X.begin();
	auto t1y = triangle1Y.begin();
	auto t1z = triangle1Z.begin();

	size_t numberTriangles = std::distance(triangle1X.begin(), triangle1X.end());
	assert(numberTriangles == std::distance(triangle1Y.begin(), triangle1Y.end()));
	assert(numberTriangles == std::distance(triangle1Z.begin(), triangle1Z.end()));

	triangle_p1.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_p1[index][0] = t1x->get_double();
		triangle_p1[index][1] = t1y->get_double();
		triangle_p1[index][2] = t1z->get_double();

		t1x++;
		t1y++;
		t1z++;
	}

	auto triangle2X = doc_view["Triangle2X"].get_array().value;
	auto triangle2Y = doc_view["Triangle2Y"].get_array().value;
	auto triangle2Z = doc_view["Triangle2Z"].get_array().value;

	auto t2x = triangle2X.begin();
	auto t2y = triangle2Y.begin();
	auto t2z = triangle2Z.begin();

	assert(numberTriangles == std::distance(triangle2X.begin(), triangle2X.end()));
	assert(numberTriangles == std::distance(triangle2Y.begin(), triangle2Y.end()));
	assert(numberTriangles == std::distance(triangle2Z.begin(), triangle2Z.end()));

	triangle_p2.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_p2[index][0] = t2x->get_double();
		triangle_p2[index][1] = t2y->get_double();
		triangle_p2[index][2] = t2z->get_double();

		t2x++;
		t2y++;
		t2z++;
	}

	auto triangle3X = doc_view["Triangle3X"].get_array().value;
	auto triangle3Y = doc_view["Triangle3Y"].get_array().value;
	auto triangle3Z = doc_view["Triangle3Z"].get_array().value;

	auto t3x = triangle3X.begin();
	auto t3y = triangle3Y.begin();
	auto t3z = triangle3Z.begin();

	assert(numberTriangles == std::distance(triangle3X.begin(), triangle3X.end()));
	assert(numberTriangles == std::distance(triangle3Y.begin(), triangle3Y.end()));
	assert(numberTriangles == std::distance(triangle3Z.begin(), triangle3Z.end()));

	triangle_p3.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_p3[index][0] = t3x->get_double();
		triangle_p3[index][1] = t3y->get_double();
		triangle_p3[index][2] = t3z->get_double();

		t3x++;
		t3y++;
		t3z++;
	}

	auto triangleR = doc_view["TriangleR"].get_array().value;
	auto triangleG = doc_view["TriangleG"].get_array().value;
	auto triangleB = doc_view["TriangleB"].get_array().value;

	auto tr = triangleR.begin();
	auto tg = triangleG.begin();
	auto tb = triangleB.begin();

	assert(numberTriangles == std::distance(triangleR.begin(), triangleR.end()));
	assert(numberTriangles == std::distance(triangleG.begin(), triangleG.end()));
	assert(numberTriangles == std::distance(triangleB.begin(), triangleB.end()));

	triangle_rgb.resize(numberTriangles);

	for (unsigned long index = 0; index < numberTriangles; index++)
	{
		triangle_rgb[index][0] = tr->get_double();
		triangle_rgb[index][1] = tg->get_double();
		triangle_rgb[index][2] = tb->get_double();

		tr++;
		tg++;
		tb++;
	}

	resetModified();
}
