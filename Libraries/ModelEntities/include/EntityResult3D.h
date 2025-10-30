// @otlicense
// File: EntityResult3D.h
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
#include "EntityVis2D3D.h"
#include "PropertyBundlePlane.h"
#include "PropertyBundleScaling.h"
#include "PropertyBundleVis2D3D.h"

class __declspec(dllexport) EntityResult3D : public EntityVis2D3D
{
public:
	EntityResult3D() : EntityResult3D(0, nullptr, nullptr, nullptr, "") {};
	EntityResult3D(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);

	static std::string className() { return "EntityResult3D"; }
	virtual std::string getClassName(void) const override { return EntityResult3D::className(); }
	
	virtual void createProperties(void) override;
	virtual bool updatePropertyVisibilities(void) override;

protected:
	virtual int getSchemaVersion(void) override { return 1; };

	PropertyBundlePlane propertyBundlePlane;
	PropertyBundleScaling propertyBundleScaling;
	PropertyBundleVis2D3D propertyBundleVis2D3D;
};
