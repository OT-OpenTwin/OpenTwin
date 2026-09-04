// @otlicense
// File: EntityMicrostripPort.h
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

#include "OTCADEntities/EntityGeometry.h"

#include "TopoDS_Shape.hxx"
#include "Precision.hxx"

class OT_CADENTITIES_API_EXPORT EntityMicrostripPort : public EntityGeometry
{
public:
	EntityMicrostripPort() : EntityMicrostripPort(0, nullptr, nullptr, nullptr) {};
	EntityMicrostripPort(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntityMicrostripPort();

	static std::string className() { return "EntityMicrostripPort"; };
	virtual std::string getClassName(void) const override { return EntityMicrostripPort::className(); };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual void setName(const std::string& _name) override;

	void createProperties();

	TopoDS_Shape createShape(double lineRadius = 0.0, double tolerance = Precision::Confusion());

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual bool getBackfaceCulling() override { return false; }
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;
	virtual void addSpecificMembersForVisualization(ot::JsonDocument& doc) override;
	void clearText(void);
	TopoDS_Shape createMicrostripPortFace(const std::string& propagationDirection, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, double xpos, double ypos, double zpos);
	void determinePortLabel(const std::string& propagationDirection, const std::string& currentDirection, double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, double xpos, double ypos, double zpos);
	std::vector<double> getVectorFromText(const std::string& direction);


	std::vector<double> textPosition = { 0.0, 0.0, 0.0 };
	std::vector<double> textNormal = { 0.0, 0.0, 0.0 };
	std::vector<double> textDirU = { 0.0, 0.0, 0.0 };
	std::string textString;
};


