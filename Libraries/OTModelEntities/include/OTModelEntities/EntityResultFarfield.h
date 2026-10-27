// @otlicense
// File: EntityResultFarfield.h
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

#include "OTModelEntities/EntityVis2D3D.h"

class __declspec(dllexport) EntityResultFarfield : public EntityVis2D3D
{
public:
	EntityResultFarfield() : EntityResultFarfield(0, nullptr, nullptr, nullptr) {};
	EntityResultFarfield(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms);
	virtual ~EntityResultFarfield();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool considerForPropertyFilter(void) const override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) const override { return false; };

	void setComplex(bool _complex) { complex = _complex; }
	void setFrequencies(std::list<std::string> _frequencies) { frequencies = _frequencies; }
	void setThetaRange(double _min, double _max, int _steps) { thetaMin = _min; thetaMax = _max; thetaSteps = _steps; }
	void setPhiRange(double _min, double _max, int _steps) { phiMin = _min; phiMax = _max; phiSteps = _steps; }
	void setStructureDimensions(double _cx, double _cy, double _cz, double _d) { structureCx = _cx; structureCy = _cy; structureCz = _cz; structureD = _d; }

	virtual void storeToDataBase(void) override;
	
	virtual std::string getClassName(void) const override { return "EntityResultFarfield"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	
	void createProperties(void);

	virtual bool updateFromProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	bool complex = false;
	std::list<std::string> frequencies;
	double thetaMin = 0.0;
	double thetaMax = 0.0;
	int thetaSteps = 0;
	double phiMin = 0.0;
	double phiMax = 0.0;
	int phiSteps = 0;
	double structureCx = 0.0;
	double structureCy = 0.0;
	double structureCz = 0.0;
	double structureD = 0.0;
};



