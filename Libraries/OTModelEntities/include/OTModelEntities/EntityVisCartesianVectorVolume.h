// @otlicense
// File: EntityVisCartesianVectorVolume.h
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
#include "OTModelEntities/EntityResultBase.h"
#include "OTModelEntities/PropertyBundlePlane.h"
#include "OTModelEntities/PropertyBundleScaling.h"
#include "OTModelEntities/PropertyBundleVisCartesianVectorVolume.h"

#include <list>

class __declspec(dllexport) EntityVisCartesianVectorVolume : public EntityVis2D3D
{
public:
	EntityVisCartesianVectorVolume() : EntityVisCartesianVectorVolume(0, nullptr, nullptr, nullptr) {};
	EntityVisCartesianVectorVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntityVisCartesianVectorVolume();

	virtual std::string getClassName(void) const override { return "EntityVisCartesianVectorVolume"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

	virtual void setGlobalRange(double minValue, double maxValue) override { propertyBundleScaling.setGlobalRange(this, minValue, maxValue); }

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisCartesianVectorVolume propertyBundleVisCartesianVector;
	// Temporary
	//EntityResultBase *source;
};



