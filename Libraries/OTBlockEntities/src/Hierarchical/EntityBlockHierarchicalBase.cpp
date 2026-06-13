// @otlicense
// File: EntityBlockHierarchicalBase.cpp
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
#include "OTGui/Graphics/Builder/GraphicsHierarchicalItemBuilder.h"
#include "OTModelEntities/EntityBinaryData.h"
#include "OTBlockEntities/Hierarchical/EntityBlockHierarchicalBase.h"

ot::EntityBlockHierarchicalBase::EntityBlockHierarchicalBase(UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBlock(_ID, _parent, _obs, _ms), m_centerImageUID(ot::invalidUID), m_centerImageVersion(ot::invalidUID), m_centerImageData(nullptr),
	m_centerImageFormat(ot::ImageFileFormat::PNG)
{}

bool ot::EntityBlockHierarchicalBase::updateFromProperties() {
	bool needsUpdate = EntityBlock::updateFromProperties();
	return needsUpdate;
}

void ot::EntityBlockHierarchicalBase::createProperties()
{
	EntityBlock::createProperties();
}

void ot::EntityBlockHierarchicalBase::fillContextMenu(const MenuRequestData* _requestData, MenuCfg& _menu)
{
	_menu.addButton("Open", "Open", "ContextMenu/Open.png", MenuButtonCfg::ButtonAction::TriggerButton)
		->setTriggerButton("Project/Selection/Open");

	EntityBlock::fillContextMenu(_requestData, _menu);
}

ot::GraphicsItemCfg* ot::EntityBlockHierarchicalBase::createBlockCfg()
{
	ensureCenterImageLoaded();

	ot::GraphicsHierarchicalItemBuilder builder;

	builder.setEntityName(this->getName());
	builder.setTopText(this->createBlockHeadline());
	builder.setBottomText(this->getBlockBottomText());

	if (hasCenterImage())
	{
		builder.setCenterImageData(m_centerImageData->getData(), m_centerImageFormat);
	}

	// Create the item
	return builder.createGraphicsItem();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data accessors

void ot::EntityBlockHierarchicalBase::setCenterImage(ot::UID _entityID, ot::UID _entityVersion, ot::ImageFileFormat _format)
{
	m_centerImageUID = _entityID;
	m_centerImageVersion = _entityVersion;
	m_centerImageFormat = _format;
	m_centerImageData.reset();
	m_centerImageData = nullptr;

	setModified();
}

void ot::EntityBlockHierarchicalBase::removeCenterImage()
{
	m_centerImageUID = ot::invalidUID;
	m_centerImageVersion = ot::invalidUID;
	m_centerImageData.reset();
	m_centerImageData = nullptr;

	setModified();
}

std::shared_ptr<EntityBinaryData> ot::EntityBlockHierarchicalBase::getCenterImageData()
{
	ensureCenterImageLoaded();
	return m_centerImageData;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::EntityBlockHierarchicalBase::createNavigationTreeEntry()
{
	EntityBlock::createNavigationTreeEntry();
}

void ot::EntityBlockHierarchicalBase::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	EntityBlock::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("CenterImageID", static_cast<int64_t>(m_centerImageUID)),
		bsoncxx::builder::basic::kvp("CenterImageVersion", static_cast<int64_t>(m_centerImageVersion)),
		bsoncxx::builder::basic::kvp("CenterImageType", ot::toString(m_centerImageFormat))
	);
}

void ot::EntityBlockHierarchicalBase::readSpecificDataFromDataBase(const bsoncxx::document::view& _docView, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBlock::readSpecificDataFromDataBase(_docView, _entityMap);

	auto docIt = _docView.find("CenterImageID");
	if (docIt != _docView.end())
	{
		m_centerImageUID = static_cast<ot::UID>(docIt->get_int64());
		m_centerImageVersion = static_cast<ot::UID>(_docView["CenterImageVersion"].get_int64());
		m_centerImageFormat = ot::stringToImageFileFormat(_docView["CenterImageType"].get_string().value.data());
	}
}

std::string ot::EntityBlockHierarchicalBase::getBlockBottomText() const
{
	return std::string();
}

void ot::EntityBlockHierarchicalBase::ensureCenterImageLoaded()
{
	if (m_centerImageUID == ot::invalidUID)
	{
		return;
	}
	if (m_centerImageData != nullptr)
	{
		return;
	}

	std::map<ot::UID, EntityBase*> entityMap;
	m_centerImageData.reset(dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, m_centerImageUID, m_centerImageVersion, entityMap)));
}
