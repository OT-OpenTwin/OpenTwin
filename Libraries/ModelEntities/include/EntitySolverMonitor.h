// @otlicense
// File: EntitySolverMonitor.h
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

#include "EntityContainer.h"

class __declspec(dllexport) EntitySolverMonitor : public EntityContainer
{
public:
	EntitySolverMonitor() : EntitySolverMonitor(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverMonitor(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverMonitor();

	virtual void addVisualizationNodes(void) override;

	virtual std::string getClassName(void) const override { return "EntitySolverMonitor"; };

	virtual entityType getEntityType(void) const override { return TOPOLOGY; };

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual bool updateFromProperties(void) override;
	
	bool updatePropertyVisibilities(void);

	void createProperties();

	static const std::string GetPropertyNameDomain(void);
	static const std::string GetPropertyNameDoF(void);
	static const std::string GetPropertyNameFieldComp(void);
	static const std::string GetPropertyNameQuantity(void);
	static const std::string GetPropertyNameMonitorFrequency(void);
	static const std::string GetPropertyNameVolume(void);
	static const std::string GetPropertyValueVolumeFull(void);
	static const std::string GetPropertyValueVolumePoint(void);
	static const std::string GetPropertyNameXCoord(void);
	static const std::string GetPropertyNameYCoord(void);
	static const std::string GetPropertyNameZCoord(void);
	static const std::string GetPropertyNameObservedTimeStep(void);

	static const std::string GetPropertyValueFieldCompElectr(void);
	static const std::string GetPropertyValueFieldCompMagnetic(void);
	static const std::string GetPropertyValueDomainFrequency(void);
	static const std::string GetPropertyValueDomainTime(void);
	static const std::string GetPropertyValueDoFNode(void);
	static const std::string GetPropertyValueDoFEdge(void);
	static const std::string GetPropertyValueQuantityVector(void);
	static const std::string GetPropertyValueQuantityComponentX(void);
	static const std::string GetPropertyValueQuantityComponentY(void);
	static const std::string GetPropertyValueQuantityComponentZ(void);
	static const std::string GetPropertyNameObservFreq(void);

protected:
	virtual int getSchemaVersion(void) override { return 1; };

private:

};