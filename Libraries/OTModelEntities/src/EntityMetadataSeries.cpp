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

#include "OTModelEntities/EntityMetadataSeries.h"

#include "OTCommunication/ActionTypes.h"
#include "bsoncxx/json.hpp"
#include "OTCore/EntityName.h"
static EntityFactoryRegistrar<EntityMetadataSeries> registrar("EntityMetadataSeries");

EntityMetadataSeries::EntityMetadataSeries(ot::UID ID, EntityBase* parent, EntityObserver* mdl, ModelState* ms)
	: EntityBase(ID, parent, mdl, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/MSMD");
	treeItem.setHiddenIcon("Default/MSMD");
	this->setDefaultTreeItem(treeItem);
}


void EntityMetadataSeries::addVisualizationNodes()
{
	
	if (!getName().empty())
	{
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

		doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

		getObserver()->sendMessageToViewer(doc);
	}

	EntityBase::addVisualizationNodes();
}

bool EntityMetadataSeries::getEntityBox(double& xmin, double& xmax, double& ymin, double& ymax, double& zmin, double& zmax)
{
	return false;
}

void EntityMetadataSeries::setName(const std::string& _name)
{
	EntityBase::setName(_name);
	m_series.setLabel(_name);
}
using bsoncxx::builder::basic::kvp;
#include "OTModelEntities/VariableToBSONConverter.h"
void EntityMetadataSeries::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBase::addStorageData(_storage);

	_storage.append(bsoncxx::builder::basic::kvp("SeriesName",m_series.getName()));
	_storage.append(bsoncxx::builder::basic::kvp("SeriesIndex", static_cast<int64_t>(m_series.getSeriesIndex())));
	
	bsoncxx::builder::basic::array parameter;
	for (const auto& param : m_series.getParameter())
	{
		bsoncxx::builder::basic::array values;
		VariableToBSONConverter converter;
		for (const auto& entry : param.values)
		{
			converter(values, entry);
		}
		const std::string metadata = ot::json::toJson(param.metaData);
		bsoncxx::document::value metadataDoc = bsoncxx::from_json(metadata);

		bsoncxx::document::value subdoc = bsoncxx::builder::basic::make_document(
			kvp("label", param.parameterLabel),
			kvp("type", param.typeName),
			kvp("name", param.parameterName),
			kvp("uid", static_cast<int64_t>(param.parameterUID)),
			kvp("unit", param.unit),
			kvp("values", values.view()),
			kvp("metadata", metadataDoc.view())
		);
		parameter.append(subdoc.view());
	}

	_storage.append(kvp("Parameters", parameter.view()));

	bsoncxx::builder::basic::array quantities;
	for (const auto& quantity: m_series.getQuantities())
	{
		const std::string metadata = ot::json::toJson(quantity.metaData);
		bsoncxx::document::value metadataDoc = bsoncxx::from_json(metadata);

		auto& tupleDescription = quantity.m_tupleDescription;

		bsoncxx::builder::basic::array dataTypes, units, dimensions, dependingParameter;
		for (const auto& s : tupleDescription.getTupleElementDataTypes()) 
		{
			dataTypes.append(s);
		}
		for (const auto& s : tupleDescription.getTupleUnits()) {
			units.append(s);
		}
		for (const auto& uid : quantity.dependingParameterIds) {
			dependingParameter.append(static_cast<int64_t>(uid));
		}
		for (const auto& dimension : quantity.dataDimensions) {
			dimensions.append(static_cast<int32_t>(dimension));
		}
		
		bsoncxx::document::value subdoc = bsoncxx::builder::basic::make_document(
			kvp("label", quantity.quantityLabel),
			kvp("name", quantity.quantityName),
			kvp("uid", static_cast<int64_t>(quantity.quantityIndex)),
			kvp("dimensions", dimensions.view()),
			kvp("dependParam", dependingParameter.view()),
			kvp("units", units.view()),
			kvp("dataType", dataTypes.view()),
			kvp("tupleForm", tupleDescription.getTupleFormatName()),
			kvp("tuple", tupleDescription.getTupleTypeName()),
			kvp("meta", metadataDoc.view())
		);		

		quantities.append(subdoc.view());
	}
	_storage.append(kvp("Quantities", quantities.view()));
	
	try
	{
		const std::string metadata = ot::json::toJson(m_series.getMetadata());
		bsoncxx::document::value doc = bsoncxx::from_json(metadata);
		_storage.append(bsoncxx::builder::basic::kvp("Metadata", doc));
	}
	catch (std::exception& _e)
	{
		const std::string message = "Failed to store metadata of series: " + this->getName() + ". Exception: " + std::string(_e.what());
		OT_LOG_E(message);
	}
}

#include "OTModelEntities/BSONToVariableConverter.h"
void EntityMetadataSeries::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	const std::string name = std::string(doc_view["SeriesName"].get_string().value);
	MetadataSeries series(name);

	auto label = ot::EntityName::getSubName(getName());
	assert(label.has_value());
	series.setLabel(label.value());
	series.setIndex(static_cast<uint64_t>(doc_view["SeriesIndex"].get_int64().value));
	m_series = series;

	// ── Parameters ──────────────────────────────────────────────────
	for (const auto& paramElem : doc_view["Parameters"].get_array().value)
	{
		bsoncxx::document::view paramDoc = paramElem.get_document().value;

		MetadataParameter param;
		param.parameterLabel = std::string(paramDoc["label"].get_string().value);
		param.parameterName = std::string(paramDoc["name"].get_string().value);
		param.parameterUID = static_cast<uint64_t>(paramDoc["uid"].get_int64().value);
		param.unit = std::string(paramDoc["unit"].get_string().value);
		param.typeName = std::string(paramDoc["type"].get_string().value);
		
		BSONToVariableConverter converter;
		for (const auto& valElem : paramDoc["values"].get_array().value)
		{
			param.values.push_back(converter(valElem)); 
		}

		const std::string metaJson = bsoncxx::to_json(paramDoc["metadata"].get_document().value);
		param.metaData.fromJson(metaJson);
	
		m_series.addParameter(std::move(param));
	}

	// ── Quantities ───────────────────────────────────────────────────
	for (const auto& quantElem : doc_view["Quantities"].get_array().value)
	{
		bsoncxx::document::view quantDoc = quantElem.get_document().value;

		MetadataQuantity quantity;
		quantity.quantityLabel = std::string(quantDoc["label"].get_string().value);
		quantity.quantityName = std::string(quantDoc["name"].get_string().value);
		quantity.quantityIndex = static_cast<uint64_t>(quantDoc["uid"].get_int64().value);

		// dataTypes
		auto& tupleDesc = quantity.m_tupleDescription;
		std::vector<std::string> dataTypes;
		for (const auto& e : quantDoc["dataType"].get_array().value)
		{
			dataTypes.push_back(std::string(e.get_string().value));
		}
		tupleDesc.setTupleElementDataTypes(dataTypes);

		// units
		std::vector<std::string> units;
		for (const auto& e : quantDoc["units"].get_array().value)
		{
			units.push_back(std::string(e.get_string().value));
		}
		tupleDesc.setTupleUnits(units);

		// tuple format / type name
		tupleDesc.setTupleFormatName(std::string(quantDoc["tupleForm"].get_string().value));
		tupleDesc.setTupleTypeName(std::string(quantDoc["tuple"].get_string().value));

		// dimensions
		for (const auto& e : quantDoc["dimensions"].get_array().value)
		{
			quantity.dataDimensions.push_back(static_cast<uint32_t>(e.get_int32().value)); 
		}

		// dependingParameterIds
		for (const auto& e : quantDoc["dependParam"].get_array().value)
		{
			quantity.dependingParameterIds.push_back(static_cast<uint64_t>(e.get_int64().value));
		}

		// metadata
		const std::string metaJson = bsoncxx::to_json(quantDoc["meta"].get_document().value);
		quantity.metaData.fromJson(metaJson);

		m_series.addQuantity(std::move(quantity));
	}
		
	if (doc_view.find("Metadata") != doc_view.end())
	{
		try
		{
			auto metadataView = doc_view["Metadata"];
			const std::string metadataString = bsoncxx::to_json(metadataView.get_document());
			ot::JsonDocument metadata;
			metadata.fromJson(metadataString);
			m_series.setMetadata(metadata);
		}
		catch (std::exception& _e)
		{
			const std::string message = "Failed to load metadata of series: " + this->getName() + ". Exception: " + std::string(_e.what());
			OT_LOG_E(message);
		}
	}
}
