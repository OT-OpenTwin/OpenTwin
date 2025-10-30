// @otlicense
// File: EntityResultText.cpp
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

// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "OTCommunication/ActionTypes.h"
#include "EntityResultText.h"
#include "EntityResultTextData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"
#include "OTGui/VisualisationTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

static EntityFactoryRegistrar<EntityResultText> registrar("EntityResultText");

EntityResultText::EntityResultText(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, owner)
{
}

EntityResultText::~EntityResultText()
{
	if (m_textData != nullptr)
	{
		delete m_textData;
		m_textData = nullptr;
	}
}

bool EntityResultText::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResultText::storeToDataBase(void)
{
	if (m_textData != nullptr)
	{
		storeTextData();
	}

	EntityBase::storeToDataBase();
}

void EntityResultText::addStorageData(bsoncxx::builder::basic::document &storage)
{
	EntityBase::addStorageData(storage);
	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("TextDataID", static_cast<int64_t>(m_textDataStorageId)),
		bsoncxx::builder::basic::kvp("TextDataVersion", static_cast<int64_t>(m_textDataStorageVersion))
	);
}

void EntityResultText::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	m_textDataStorageId = doc_view["TextDataID"].get_int64();
	m_textDataStorageVersion = doc_view["TextDataVersion"].get_int64();

	resetModified();
}

void EntityResultText::addVisualizationNodes(void)
{
	addVisualizationItem(getInitiallyHidden());

	EntityBase::addVisualizationNodes();
}

void EntityResultText::addVisualizationItem(bool isHidden)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TextVisible";
	treeIcons.hiddenIcon = "TextHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	ot::VisualisationTypes visTypes;
	visTypes.addTextVisualisation();

	visTypes.addToJsonObject(doc, doc.GetAllocator());
	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
}

void EntityResultText::createProperties(void)
{
	// This item has no properties yet	
	getProperties().forceResetUpdateForAllProperties();
}

bool EntityResultText::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	getProperties().forceResetUpdateForAllProperties();

	return false; // Notify, whether property grid update is necessary
}

EntityResultTextData *EntityResultText::getTextData(void)
{
	ensureTextDataLoaded();
	assert(m_textData != nullptr);

	return m_textData;
}

std::string EntityResultText::getText()
{
	ensureTextDataLoaded();
	assert(m_textData != nullptr);

	return m_textData->getText();
}

bool EntityResultText::visualiseText()
{
	return true;
}

ot::TextEditorCfg EntityResultText::createConfig(bool _includeData)
{
	ot::TextEditorCfg result;
	result.setEntityName(this->getName());
	result.setTitle(this->getName());
	if (_includeData)
	{
		result.setPlainText(this->getText());
	}

	result.setDocumentSyntax(ot::DocumentSyntax::PlainText);
	
	return result;
}

ot::ContentChangedHandling EntityResultText::getTextContentChangedHandling()
{
	return m_contentChangedHandling;
}

void EntityResultText::deleteTextData(void)
{
	m_textData = nullptr;
	m_textDataStorageId = 0;
	m_textDataStorageVersion = 0;

	setModified();
}

void EntityResultText::storeTextData(void)
{
	if (m_textData == nullptr) return;
	assert(m_textData != nullptr);

	m_textData->storeToDataBase();

	if (m_textDataStorageId != m_textData->getEntityID() || m_textDataStorageVersion != m_textData->getEntityStorageVersion())
	{
		setModified();
	}

	m_textDataStorageId = m_textData->getEntityID();
	m_textDataStorageVersion = m_textData->getEntityStorageVersion();
}

void EntityResultText::releaseTextData(void)
{
	if (m_textData == nullptr) return;

	storeTextData();

	delete m_textData;
	m_textData = nullptr;
}

void EntityResultText::ensureTextDataLoaded(void)
{
	if (m_textData == nullptr)
	{
		if (m_textDataStorageId == 0)
		{
			m_textData = new EntityResultTextData(getUidGenerator()->getUID(), this, getObserver(), getModelState(), getOwningService());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			m_textData = dynamic_cast<EntityResultTextData *>(readEntityFromEntityID(this, m_textDataStorageId, entityMap));
		}

		assert(m_textData != nullptr);
	}
}

void EntityResultText::setText(const std::string &text)
{
	ensureTextDataLoaded();
	assert(m_textData != nullptr);

	m_textData->setText(text);

	storeTextData();
}