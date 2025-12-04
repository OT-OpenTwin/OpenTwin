// @otlicense
// File: EntityVisUnstructuredScalarSurface.h
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
#include "OldTreeIcon.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVisUnstructuredScalarSurface.h"

#include <list>

class __declspec(dllexport) EntityVisUnstructuredScalarSurface : public EntityVis2D3D
{
public:
	EntityVisUnstructuredScalarSurface() : EntityVisUnstructuredScalarSurface(0, nullptr, nullptr, nullptr) {};
	EntityVisUnstructuredScalarSurface(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms);
	virtual ~EntityVisUnstructuredScalarSurface();

	virtual std::string getClassName(void) const override { return "EntityVisUnstructuredScalarSurface"; };

	virtual bool updateFromProperties(void) override;

	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

protected:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

private:
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVisUnstructuredScalarSurface propertyBundleVisUnstructuredScalarSurface;
	// Temporary
	//EntityResultBase *source;
};



