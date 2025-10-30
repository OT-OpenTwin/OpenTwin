// @otlicense
// File: EntitySignalType.h
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

#include "EntityContainer.h"

class __declspec(dllexport) EntitySignalType : public EntityContainer
{
public:
	EntitySignalType() : EntitySignalType(0, nullptr, nullptr, nullptr, "") {};
	EntitySignalType(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySignalType();
	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) const override { return "EntitySignalType"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;

	void createProperties();

	static const std::string GetPropertyNameExcitationType();
	static const std::string GetValueSinGuasSignal();

	static const std::string GetPropertyNameMinFrequency();
	static const std::string GetPropertyNameMaxFrequency();


protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:

	// Inherited via EntityBase
	virtual bool getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax) override;
};