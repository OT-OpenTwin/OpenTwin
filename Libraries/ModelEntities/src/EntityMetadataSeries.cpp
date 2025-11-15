// @otlicense
// File: EntityMetadataSeries.cpp
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

#include "EntityMetadataSeries.h"

#include "OTCommunication/ActionTypes.h"

static EntityFactoryRegistrar<EntityMetadataSeries> registrar("EntityMetadataSeries");

EntityMetadataSeries::EntityMetadataSeries(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms)
	: EntityWithDynamicFields(ID, parent, mdl, ms)
{
	ot::EntityTreeItem treeItem;
	treeItem.setVisibleIcon("MSMD");
	treeItem.setHiddenIcon("MSMD");
	this->setTreeItem(treeItem, true);

	CreatePlainDocument(_parameterDocument);
	CreatePlainDocument(_quantityDocument);
}

bool EntityMetadataSeries::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

std::vector<std::string> EntityMetadataSeries::getAllParameterDocumentNames()
{
	return getDocumentsNames(_parameterDocument);
}

std::vector<std::string> EntityMetadataSeries::getAllQuantityDocumentNames()
{
	return getDocumentsNames(_quantityDocument);
}

void EntityMetadataSeries::InsertToParameterField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName)
{
	std::string fullDocumentPath = _parameterDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, std::move(values), fullDocumentPath);
}

void EntityMetadataSeries::InsertToQuantityField(std::string fieldName, std::list<ot::Variable>&& values, std::string documentName)
{
	std::string fullDocumentPath = _quantityDocument;
	if (documentName != "")
	{
		fullDocumentPath += "/" + documentName;
	}
	InsertInField(fieldName, std::move(values), fullDocumentPath);
}

