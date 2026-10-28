// @otlicense
// File: EntityBlockCircuitSubcircuit.cpp
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

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitSubcircuit.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitTerminal.h"
#include "OTGui/Graphics/Builder/GraphicsCircuitItemBuilder.h"
#include "OTGui/Graphics/GraphicsItemMap.h"
#include "OTModelEntities/Properties/EntityPropertiesItems.h"
#include "OTCore/FolderNames.h"

// Third party header
#include <bsoncxx/builder/basic/array.hpp>
#include <algorithm>
#include <cctype>
static std::string getTerminalTitleFromConnectorName(const std::string& _connectorName) {
	size_t lastUnderscore = _connectorName.rfind('_');
	if (lastUnderscore != std::string::npos && lastUnderscore + 1 < _connectorName.size()) {
		bool allDigits = true;
		for (size_t i = lastUnderscore + 1; i < _connectorName.size(); ++i) {
			if (!std::isdigit(static_cast<unsigned char>(_connectorName[i]))) {
				allDigits = false;
				break;
			}
		}
		if (allDigits) {
			return _connectorName.substr(0, lastUnderscore);
		}
	}
	return _connectorName;
}

static EntityFactoryRegistrar<EntityBlockCircuitSubcircuit> registrar(EntityBlockCircuitSubcircuit::className());

EntityBlockCircuitSubcircuit::EntityBlockCircuitSubcircuit(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	: EntityBlockCircuitElement(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("CircuitElementImages/Subcircuit");
	treeItem.setHiddenIcon("CircuitElementImages/Subcircuit");
	this->setDefaultTreeItem(treeItem);
	setBlockTitle("Subckt");

	registerCallbacks(
		ot::EntityCallbackBase::Callback::DataHandle,
		OT_INFO_SERVICE_TYPE_CircuitSimulatorService
	);

	this->clearConnectors();
	m_orderedTerminalNames.clear();

	// Initially, subcircuit has no terminals
	resetModified();
}

void EntityBlockCircuitSubcircuit::createProperties()
{
	EntityBlockCircuitElement::createProperties();
	EntityPropertiesEntityList::createProperty("Subcircuit Properties", "SubCircuit", ot::FolderNames::CircuitsFolder, ot::invalidUID, "", -1, "default", getProperties());

	std::string currentName = getName();
	std::string subName = getSubcircuitName();
	if (subName.rfind("Circuits/", 0) == 0)
	{
		subName = subName.substr(std::string("Circuits/").length());
	}
	if (!currentName.empty() && !subName.empty() && currentName.rfind(" (") == std::string::npos)
	{
		setName(currentName + " (" + subName + ")");
	}
}


bool EntityBlockCircuitSubcircuit::updateFromProperties(void)
{
	bool refresh = false;
	auto prop = getProperties().getProperty("SubCircuit");
	if (prop && prop->needsUpdate())
	{
		auto listProp = dynamic_cast<EntityPropertiesEntityList*>(prop);
		if (listProp)
		{
			std::string val = listProp->getValueName();
			std::string displayTitle = val;
			if (displayTitle.rfind("Circuits/", 0) == 0)
			{
				displayTitle = displayTitle.substr(std::string("Circuits/").length());
			}
			setBlockTitle(displayTitle.empty() ? "Subckt" : displayTitle);

			std::string currentPath = getName();
			if (!currentPath.empty())
			{
				std::string basePath = currentPath;
				size_t pos = basePath.rfind(" (");
				if (pos != std::string::npos && basePath.back() == ')')
				{
					basePath = basePath.substr(0, pos);
				}
				std::string newPath = displayTitle.empty() ? basePath : (basePath + " (" + displayTitle + ")");
				if (newPath != currentPath)
				{
					setName(newPath);
					if (getObserver() != nullptr)
					{
						ot::JsonDocument doc;
						doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_RenameEntityName, doc.GetAllocator()), doc.GetAllocator());
						doc.AddMember(OT_ACTION_PARAM_PATH_FROM, ot::JsonString(currentPath, doc.GetAllocator()), doc.GetAllocator());
						doc.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(newPath, doc.GetAllocator()), doc.GetAllocator());

						getObserver()->sendMessageToViewer(doc);
					}
				}
			}
		}
		setModified();
		if (getObserver() != nullptr)
		{
			createBlockItem();
			getObserver()->connectionChanged(this);
		}
		refresh = true;
	}

	refresh = EntityBlockCircuitElement::updateFromProperties() || refresh;

	if (refresh)
	{
		getProperties().forceResetUpdateForAllProperties();
	}
	return refresh;
}


std::string EntityBlockCircuitSubcircuit::getSubcircuitName() const
{
	auto prop = getProperties().getProperty("SubCircuit");
	auto listProp = dynamic_cast<const EntityPropertiesEntityList*>(prop);
	if (listProp)
	{
		return listProp->getValueName();
	}
	return "";
}

void EntityBlockCircuitSubcircuit::setSubcircuitName(const std::string& _name)
{
	std::string fullName = _name;
	if (!fullName.empty() && fullName.rfind("Circuits/", 0) != 0)
	{
		fullName = "Circuits/" + fullName;
	}

	auto prop = getProperties().getProperty("SubCircuit");
	auto listProp = dynamic_cast<EntityPropertiesEntityList*>(prop);
	if (listProp)
	{
		listProp->setValueName(fullName);
	}

	std::string displayTitle = fullName;
	if (displayTitle.rfind("Circuits/", 0) == 0)
	{
		displayTitle = displayTitle.substr(std::string("Circuits/").length());
	}
	setBlockTitle(displayTitle.empty() ? "Subckt" : displayTitle);

	std::string currentPath = getName();
	if (!currentPath.empty())
	{
		std::string basePath = currentPath;
		size_t pos = basePath.rfind(" (");
		if (pos != std::string::npos && basePath.back() == ')')
		{
			basePath = basePath.substr(0, pos);
		}
		std::string newPath = displayTitle.empty() ? basePath : (basePath + " (" + displayTitle + ")");
		if (newPath != currentPath)
		{
			setName(newPath);
			if (getObserver() != nullptr)
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_RenameEntityName, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_PATH_FROM, ot::JsonString(currentPath, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_PATH_To, ot::JsonString(newPath, doc.GetAllocator()), doc.GetAllocator());

				getObserver()->sendMessageToViewer(doc);
			}
		}
	}

	setModified();
}

double EntityBlockCircuitSubcircuit::getRotation() const
{
	auto propertyBase = getProperties().getProperty("Rotation");
	auto propertyRotation = dynamic_cast<const EntityPropertiesDouble*>(propertyBase);
	return propertyRotation ? propertyRotation->getValue() : 0.0;
}
bool EntityBlockCircuitSubcircuit::getFlipHorizontal() const
{
	auto propertyBase = getProperties().getProperty("Flip Horizontal");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	return propertyFlip ? propertyFlip->getValue() : false;
}
bool EntityBlockCircuitSubcircuit::getFlipVertical() const
{
	auto propertyBase = getProperties().getProperty("Flip Vertical");
	auto propertyFlip = dynamic_cast<const EntityPropertiesBoolean*>(propertyBase);
	return propertyFlip ? propertyFlip->getValue() : false;
}

ot::GraphicsItemCfg* EntityBlockCircuitSubcircuit::createBlockCfg()
{
	std::vector<std::string> terminals = m_orderedTerminalNames;
	std::map<std::string, std::string> terminalSides;

	if (getObserver() != nullptr)
	{
		std::string modelName = getSubcircuitName();
		if (!modelName.empty())
		{
			std::string sceneName = modelName;
			if (sceneName.rfind("Circuits/", 0) != 0)
			{
				sceneName = "Circuits/" + sceneName;
			}

			const auto* graphicsItemMap = getObserver()->getGraphicsItemMap(sceneName);
			if (graphicsItemMap != nullptr)
			{
				struct TerminalInfo {
					std::string name;
					std::string side;
					ot::UID uid;
				};
				std::vector<TerminalInfo> rawTerminals;
				ot::UIDList graphicsItemsUIDs = graphicsItemMap->getAllIDs();
				for (auto uid : graphicsItemsUIDs)
				{
					//! @warning Do not store or cache the pointer returned by getTopoEntity. The entity may be modified or destroyed at any time.
					EntityBase* entity = getObserver()->getTopoEntity(uid);
					if(!entity)
					{
						continue;
					}
					EntityBlockCircuitTerminal* terminal = dynamic_cast<EntityBlockCircuitTerminal*>(entity);
					if (terminal != nullptr)
					{
						rawTerminals.push_back({ terminal->getTerminalName(), terminal->getTerminalSide(), uid });
					}
				}
				std::sort(rawTerminals.begin(), rawTerminals.end(), [](const TerminalInfo& a, const TerminalInfo& b) {
					if (a.name != b.name) return a.name < b.name;
					return a.uid < b.uid;
				});

				std::map<std::string, int> currentCounts;
				terminals.clear();
				for (const auto& t : rawTerminals) {
					currentCounts[t.name]++;
					std::string uniqueName = (currentCounts[t.name] == 1) ? t.name : (t.name + "_" + std::to_string(currentCounts[t.name]));
					terminals.push_back(uniqueName);
					terminalSides[uniqueName] = t.side;
				}
			}
		}
		else
		{
			terminals.clear();
		}
	}

	if (terminals != m_orderedTerminalNames)
	{
		m_orderedTerminalNames = terminals;
		this->clearConnectors();
		for (const auto& termName : m_orderedTerminalNames)
		{
			std::string title = getTerminalTitleFromConnectorName(termName);
			this->addConnector(ot::Connector(ot::ConnectorType::Any, termName, title));
		}
		setModified();
		if (getObserver() != nullptr)
		{
			getObserver()->connectionChanged(this);
		}
	}
	else if (m_orderedTerminalNames.empty() && getConnectorCount() > 0)
	{
		this->clearConnectors();
		setModified();
	}

	ot::GraphicsCircuitItemBuilder block;
	block.setName(this->getClassName());

	std::string title = getSubcircuitName();
	if (title.rfind("Circuits/", 0) == 0)
	{
		title = title.substr(std::string("Circuits/").length());
	}
	if (title.empty())
	{
		title = createBlockHeadline();
	}
	block.setTitle(title);

	std::vector<std::string> leftTerminals;
	std::vector<std::string> topTerminals;
	std::vector<std::string> rightTerminals;
	std::vector<std::string> bottomTerminals;

	for (const auto& termName : m_orderedTerminalNames)
	{
		std::string rawName = getTerminalTitleFromConnectorName(termName);
		std::string side = "Left";
		if (terminalSides.count(termName))
		{
			side = terminalSides[termName];
		}
		else if (terminalSides.count(rawName))
		{
			side = terminalSides[rawName];
		}

		if (side == "Left")        leftTerminals.push_back(termName);
		else if (side == "Top")    topTerminals.push_back(termName);
		else if (side == "Right")  rightTerminals.push_back(termName);
		else if (side == "Bottom") bottomTerminals.push_back(termName);
		else                       leftTerminals.push_back(termName);
	}

	for (const auto& termName : leftTerminals)
	{
		std::string termTitle = getTerminalTitleFromConnectorName(termName);
		block.addLeft(termName, termTitle);
	}
	for (const auto& termName : topTerminals)
	{
		std::string termTitle = getTerminalTitleFromConnectorName(termName);
		block.addTop(termName, termTitle);
	}
	for (const auto& termName : rightTerminals)
	{
		std::string termTitle = getTerminalTitleFromConnectorName(termName);
		block.addRight(termName, termTitle);
	}
	for (const auto& termName : bottomTerminals)
	{
		std::string termTitle = getTerminalTitleFromConnectorName(termName);
		block.addBottom(termName, termTitle);
	}

	auto graphicsItemConfig = block.createGraphicsItem();
	if (graphicsItemConfig)
	{
		ot::Transform transform;
		transform.setRotation(getRotation());
		transform.setFlipState(ot::Transform::FlipHorizontally, getFlipHorizontal());
		transform.setFlipState(ot::Transform::FlipVertically, getFlipVertical());
		graphicsItemConfig->setTransform(transform);
		graphicsItemConfig->setGraphicsItemFlags(graphicsItemConfig->getGraphicsItemFlags() | ot::GraphicsItemCfg::ItemIsDoubleClickable);
	}

	return graphicsItemConfig;
}


void EntityBlockCircuitSubcircuit::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBlock::addStorageData(_storage);
	bsoncxx::builder::basic::array terminalArray;
	for (const auto& terminal : m_orderedTerminalNames)
	{
		terminalArray.append(terminal);
	}
	_storage.append(bsoncxx::builder::basic::kvp("OrderedTerminals", terminalArray));
}

void EntityBlockCircuitSubcircuit::readSpecificDataFromDataBase(const bsoncxx::document::view& _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(_doc_view, _entityMap);
	m_orderedTerminalNames.clear();
	if (_doc_view.find("OrderedTerminals") != _doc_view.end())
	{
		for (const auto& terminalElem : _doc_view["OrderedTerminals"].get_array().value)
		{
			m_orderedTerminalNames.push_back(std::string(terminalElem.get_string().value));
		}
	}

	if (m_orderedTerminalNames.empty())
	{
		this->clearConnectors();
	}
}