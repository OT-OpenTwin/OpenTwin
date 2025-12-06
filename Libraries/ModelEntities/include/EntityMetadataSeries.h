// @otlicense
// File: EntityMetadataSeries.h
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
#include "EntityWithDynamicFields.h"


class __declspec(dllexport) EntityMetadataSeries : public EntityWithDynamicFields
{
public:
	EntityMetadataSeries() : EntityMetadataSeries(0, nullptr, nullptr, nullptr) {};
	EntityMetadataSeries(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms);
	std::string getClassName() const override { return "EntityMetadataSeries"; };
	virtual entityType getEntityType(void) const override { return TOPOLOGY; };
	virtual bool getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax) override;
	
	std::vector<std::string> getAllParameterDocumentNames();
	std::vector<std::string> getAllQuantityDocumentNames();
	const std::string getParameterDocumentName() const { return _parameterDocument; }
	const std::string getQuantityDocumentName() const { return _quantityDocument; }

	void InsertToParameterField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName = "");
	void InsertToQuantityField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName = "");
private:
	const std::string _parameterDocument = "Parameter";
	const std::string _quantityDocument = "Quantity";
};
