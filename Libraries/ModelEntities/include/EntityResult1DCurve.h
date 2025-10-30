// @otlicense
// File: EntityResult1DCurve.h
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
#include "EntityBase.h"
#include "IVisualisationCurve.h"
#include "OTGui/QueryInformation.h"

class __declspec(dllexport) EntityResult1DCurve : public EntityBase, public IVisualisationCurve
{
public:
	enum DefaultCurveStyle {
		Default,
		ScatterPlot
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// Static methods

	static ot::Plot1DCurveCfg createDefaultConfig(const std::string& _plotName, const std::string& _curveName, DefaultCurveStyle _style = DefaultCurveStyle::Default);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Constructor / Destructor

	EntityResult1DCurve() : EntityResult1DCurve(0, nullptr, nullptr, nullptr, "") {};
	EntityResult1DCurve(ot::UID _ID, EntityBase* _parent, EntityObserver* _mdl, ModelState* _ms, const std::string& _owner);
	EntityResult1DCurve(const EntityResult1DCurve& _other) = default;
	EntityResult1DCurve(EntityResult1DCurve&& _other) noexcept = default;
	~EntityResult1DCurve() = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Operators

	EntityResult1DCurve& operator=(const EntityResult1DCurve& _other) = default;
	EntityResult1DCurve& operator=(EntityResult1DCurve&& _other) = default;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Virtual methods

	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override { return false; };

	virtual bool considerForPropertyFilter() override { return true; };
	virtual bool considerChildrenForPropertyFilter() override { return false; };
	virtual entityType getEntityType() const override { return TOPOLOGY; };

	virtual void addVisualizationNodes() override;
	virtual bool updateFromProperties() override;

	virtual std::string getClassName() const override { return "EntityResult1DCurve_New"; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Configuration

	void createProperties(DefaultCurveStyle _style = DefaultCurveStyle::Default);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	// Inherited via IVisualisationCurve
	bool visualiseCurve() override { return true; };
	ot::Plot1DCurveCfg getCurve() override;
	void setCurve(const ot::Plot1DCurveCfg& _curve) override;

	const ot::QueryInformation& getQueryInformation() const { return m_queryInformation; }

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected: Virtual methods

protected:
	//! @brief Updates the property visibilities of the curve properties.
	//! @return True if the visibility has changed.
	virtual bool updatePropertyVisibilities();
	
	virtual void addStorageData(bsoncxx::builder::basic::document& storage) override;
	
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Helper

private:
	ot::QueryInformation m_queryInformation;

	bsoncxx::builder::basic::document serialise(ot::QuantityContainerEntryDescription& _quantityContainerEntryDescription);
	ot::QuantityContainerEntryDescription deserialise(bsoncxx::v_noabi::document::view _subDocument);
};
