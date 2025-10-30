// @otlicense
// File: PropertyBundleVisUnstructuredVectorSurface.h
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
#include "PropertyBundle.h"

#include "PropertiesVisUnstructuredVector.h"

class __declspec(dllexport) PropertyBundleVisUnstructuredVectorSurface : public PropertyBundle
{
public:
	virtual void setProperties(EntityBase * _thisObject) override;
	virtual bool updatePropertyVisibility(EntityBase * _thisObject) override;

private:
	PropertiesVisUnstructuredVector m_properties;
	const std::string m_defaultCategory = "3D Visualization";
	const std::string m_groupNameArrows = "Arrows";
};
