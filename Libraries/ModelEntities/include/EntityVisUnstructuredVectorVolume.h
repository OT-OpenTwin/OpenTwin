// @otlicense
// File: EntityVisUnstructuredVectorVolume.h
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

#include "EntityVis2D3D.h"
#include "EntityResultBase.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVisUnstructuredVectorVolume.h"

#include <list>

class __declspec(dllexport) EntityVisUnstructuredVectorVolume : public EntityVis2D3D
{
public:
	EntityVisUnstructuredVectorVolume() : EntityVisUnstructuredVectorVolume(0, nullptr, nullptr, nullptr) {};
	EntityVisUnstructuredVectorVolume(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntityVisUnstructuredVectorVolume();

	virtual std::string getClassName(void) const override { return "EntityVisUnstructuredVectorVolume"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisUnstructuredVectorVolume propertyBundleVisUnstructuredVector;
	// Temporary
	//EntityResultBase *source;
};



