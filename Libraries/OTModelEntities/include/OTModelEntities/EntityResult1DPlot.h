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

// OpenTwin header
#include "OTCore/Symbol.h"
#include "OTModelEntities/EntityContainer.h"
#include "OTModelEntities/Visualization/IVisualisationPlot1D.h"
#include "OTModelEntities/PropertyBundleQuerySettings.h"
#include "OTCore/QueryDescription/ValueComparisonDescription.h"

class __declspec(dllexport) EntityResult1DPlot: public EntityContainer, public ot::IVisualisationPlot1D
{
public:
	EntityResult1DPlot() : EntityResult1DPlot(0, nullptr, nullptr, nullptr) {};
	EntityResult1DPlot(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms);

	bool getEntityBox(double& _xmin, double& _xmax, double& _ymin, double& _ymax, double& _zmin, double& _zmax) override { return false; };
	entityType getEntityType() const override { return entityType::TOPOLOGY; };
	virtual bool considerForPropertyFilter() const override { return true; };
	virtual bool considerChildrenForPropertyFilter() const override { return false; };
	virtual void storeToDataBase() override;
	virtual void addVisualizationNodes() override;
	virtual std::string getClassName() const override { return "EntityResult1DPlot"; };
	virtual int getSchemaVersion() override { return 2; };
	virtual bool updateFromProperties() override;
	bool updatePropertyVisibilities();

	void hideAxisQuantityProperties();

	virtual void addChild(EntityBase* _child) override;
	virtual void removeChild(EntityBase* _child) override;

	virtual void propertiesAboutToBeShown() override;

	void createProperties();
	
	// Inherited via IVisualisationPlot1D
	const ot::Plot1DCfg getPlot() override;
	void setPlot(const ot::Plot1DCfg& _config) override;

	bool visualisePlot() override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Property Setter / Getter

	ot::Plot1DCfg::PlotType getPlotType() const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private helper

private:
	static std::string getXAxisPropertyGroupName() { return "X axis"; };
	static std::string getYAxisPropertyGroupName() { return "Y axis"; };
	static std::string getRadiusAxisPropertyGroupName() { return "r axis"; };
	static std::string getAzimuthAxisPropertyGroupName() { return std::string(ot::Symbol::phi()) + " axis"; };

	void createAxisProperties(const std::string& _axisName);
	void setAxisFromProperties(const std::string& _axisName, ot::Plot1DAxisCfg& _axis);
	bool setAxisPropertiesVisibility(const std::string& _axisName, bool _visible);
	bool updateAxisPropertiesVisibility(const std::string& _axisName);

	virtual void addStorageData(bsoncxx::builder::basic::document& _storage) override;
	virtual void readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap) override;

};
