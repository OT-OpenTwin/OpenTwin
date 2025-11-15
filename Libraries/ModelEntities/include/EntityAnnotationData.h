// @otlicense
// File: EntityAnnotationData.h
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

#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include <vector>
#include <array>

class __declspec(dllexport) EntityAnnotationData : public EntityBase
{
public:
	EntityAnnotationData() : EntityAnnotationData(0, nullptr, nullptr, nullptr) {};
	EntityAnnotationData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms);
	virtual ~EntityAnnotationData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void addPoint(double x, double y, double z, double r, double g, double b);
	void addTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double r, double g, double b);

	const std::vector<std::array<double, 3>> &getPoints(void) { return points; };
	const std::vector<std::array<double, 3>> &getPointsRGB(void) { return points_rgb; };

	const std::vector<std::array<double, 3>> &getTriangleP1(void) { return triangle_p1; };
	const std::vector<std::array<double, 3>> &getTriangleP2(void) { return triangle_p2; };
	const std::vector<std::array<double, 3>> &getTriangleP3(void) { return triangle_p3; };
	const std::vector<std::array<double, 3>> &getTriangleRGB(void) { return triangle_rgb; };

	const double* getEdgeColorRGB() { return edgeColorRGB; };

	static std::string className() { return "EntityAnnotationData"; };
	virtual std::string getClassName(void) const override { return EntityAnnotationData::className(); };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	double edgeColorRGB[3];

	std::vector<std::array<double, 3>> points;	
	std::vector<std::array<double, 3>> points_rgb;

	std::vector<std::array<double, 3>> triangle_p1;
	std::vector<std::array<double, 3>> triangle_p2;
	std::vector<std::array<double, 3>> triangle_p3;
	std::vector<std::array<double, 3>> triangle_rgb;
};



