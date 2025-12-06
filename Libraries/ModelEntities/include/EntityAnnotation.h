// @otlicense
// File: EntityAnnotation.h
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

class EntityAnnotationData;

class __declspec(dllexport) EntityAnnotation : public EntityBase {
public:
	EntityAnnotation() : EntityAnnotation(0, nullptr, nullptr, nullptr) {};
	EntityAnnotation(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms);
	virtual ~EntityAnnotation();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void addPoint(double x, double y, double z, double r, double g, double b);
	void addTriangle(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3, double r, double g, double b);

	const std::vector<std::array<double, 3>> &getPoints(void);
	const std::vector<std::array<double, 3>> &getPointsRGB(void);

	const std::vector<std::array<double, 3>> &getTriangleP1(void);
	const std::vector<std::array<double, 3>> &getTriangleP2(void);
	const std::vector<std::array<double, 3>> &getTriangleP3(void);
	const std::vector<std::array<double, 3>> &getTriangleRGB(void);

	const double* getEdgeColorRGB();

	virtual void storeToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	static std::string className() { return "EntityAnnotation"; };
	virtual std::string getClassName(void) const override { return EntityAnnotation::className(); };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void)const  override { return TOPOLOGY; };
	virtual void removeChild(EntityBase *child) override;

	EntityAnnotationData *getAnnotationData(void) { ensureAnnotationDataIsLoaded(); return annotationData; }

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void ensureAnnotationDataIsLoaded(void);
	void storeAnnotationData(void);
	void releaseAnnotationData(void);

	EntityAnnotationData *annotationData;
	long long annotationDataStorageId;
};



