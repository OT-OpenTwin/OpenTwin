// @otlicense
// File: EntityResult1DPlot.h
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
#include "IVisualisationPlot1D.h"
#include "PropertyBundleQuerySettings.h"
#include "OTCore/ValueComparisionDefinition.h"

class __declspec(dllexport) EntityResult1DPlot: public EntityContainer, public IVisualisationPlot1D
{
public:
	EntityResult1DPlot() : EntityResult1DPlot(0, nullptr, nullptr, nullptr, "") {};
	EntityResult1DPlot(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, const std::string& _owner);

	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType(void) const override { return entityType::TOPOLOGY; };
	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };
	virtual void storeToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	virtual std::string getClassName(void) const override { return "EntityResult1DPlot_New"; };
	virtual bool updateFromProperties(void) override;
	bool updatePropertyVisibilities(void);
	
	virtual void addChild(EntityBase* _child) override;
	virtual void removeChild(EntityBase* _child) override;


	void createProperties(void);
	
	// Inherited via IVisualisationPlot1D
	const ot::Plot1DCfg getPlot() override;
	void setPlot(const ot::Plot1DCfg& _config) override;

	bool visualisePlot() override;
private:
	PropertyBundleQuerySettings m_querySettings;

	void setQuerySelections();

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;

};
