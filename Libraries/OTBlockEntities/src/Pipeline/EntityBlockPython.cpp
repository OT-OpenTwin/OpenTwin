// @otlicense
// File: EntityBlockPython.cpp
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
#include "OTCore/FolderNames.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/PropertyHelper.h"
#include "OTBlockEntities/BlockImageNames.h"
#include "OTBlockEntities/Pipeline/EntityBlockPython.h"
#include "OTBlockEntities/Pipeline/PythonHeaderInterpreter.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/Lms/EntityPythonScript.h"

static EntityFactoryRegistrar<EntityBlockPython> registrar(EntityBlockPython::className());

EntityBlockPython::EntityBlockPython(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms)
	:EntityBlock(ID, parent, obs, ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon(ot::BlockImageNames::getCornerImagePath() + getIconName());
	treeItem.setHiddenIcon(ot::BlockImageNames::getCornerImagePath() + getIconName());
	this->setDefaultTreeItem(treeItem);
	
	setBlockTitle("Python");

	resetModified();
}

void EntityBlockPython::createProperties()
{
	EntityPropertiesExtendedEntityList::createProperty("Python properties", m_propertyNameScripts, ot::FolderNames::PythonScriptFolder, ot::invalidUID, "", -1, { "< Load from Library >" }, { "" }, "default", getProperties());
	/*EntityPropertiesEntityList::createProperty("Python properties", m_propertyNameScripts, ot::FolderNames::PythonScriptFolder, ot::invalidUID, "", -1, "default", getProperties());
	EntityPropertiesEntityList::createProperty("Python properties", m_propertyNameEnvironments, ot::FolderNames::PythonManifestFolder, ot::invalidUID, "", -1, "default", getProperties());*/
	EntityPropertiesExtendedEntityList::createProperty("Python properties", m_propertyNameEnvironments, ot::FolderNames::PythonManifestFolder, ot::invalidUID, "", -1 , { "< Load from Library >" },{ "" }, "default", getProperties());
}

std::string EntityBlockPython::getSelectedScript()
{
	auto propBase = getProperties().getProperty(m_propertyNameScripts);
	auto scriptSelection = dynamic_cast<EntityPropertiesEntityList*>(propBase);
	assert(scriptSelection != nullptr);

	return scriptSelection->getValueName();
}

ot::UID EntityBlockPython::getSelectedEnvironment()
{
	auto propBase = getProperties().getProperty(m_propertyNameEnvironments);
	auto selectedManifest = dynamic_cast<EntityPropertiesExtendedEntityList*>(propBase);
	assert(selectedManifest != nullptr);

	return selectedManifest->getValueID();

	//ot::UID selectedManifest = PropertyHelper::getEntityListPropertyValueID(this, m_propertyNameEnvironments);
	//return selectedManifest;
}

ot::GraphicsItemCfg* EntityBlockPython::createBlockCfg()
{
	ot::GraphicsFlowItemBuilder block;
	block.setName(this->getClassName());
	block.setTitle(this->createBlockHeadline());

	const ot::Color colourTitle(ot::Cyan);
	const ot::Color colourBackground(ot::White);
	block.setTitleBackgroundGradientColor(colourTitle);
	block.setLeftTitleCornerImagePath(ot::BlockImageNames::getCornerImagePath() + ot::BlockImageNames::getCornerImageNamePython());
	block.setBackgroundImagePath(ot::BlockImageNames::getCornerImagePath() + getIconName());
	//block.setBackgroundImageInsertMode(ot::GraphicsFlowItemBuilder::OnStack);

	const std::string blockName = getClassName();
	const std::string blockTitel = createBlockHeadline();
	addConnectors(block);
	
	auto graphicsItemConfig = block.createGraphicsItem();
	return graphicsItemConfig;
}

bool EntityBlockPython::updateFromProperties()
{
	// Check if LoadFromLibrary was selected
	auto basePropertyModel = getProperties().getProperty(m_propertyNameScripts);
	auto modelProperty = dynamic_cast<EntityPropertiesExtendedEntityList*>(basePropertyModel);
	if (modelProperty == nullptr) {
		OT_LOG_E("Model selection property cast failed");
		return false;
	}

	if (modelProperty->getValueName() == "< Load from Library >") {

		ot::LibraryElementSelectionCfg config;
		config.setRequestingEntityID(this->getEntityID());
		config.setCollectionName("PythonScripts");
		config.setCallBackAction(OT_ACTION_CMD_LMS_CreateConfig);
		config.setEntityType(EntityPythonScript::className());
		config.setNewEntityFolder(ot::FolderNames::PythonScriptFolder);
		config.setPropertyName(m_propertyNameScripts);

		// Build the document
		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_CreateConfig, doc.GetAllocator()), doc.GetAllocator());
		// Add the config information to the document
		ot::JsonObject configObj;
		config.addToJsonObject(configObj, doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_Config, configObj, doc.GetAllocator());

		// if it was selected use observer to send message to LMS
		getObserver()->requestLibraryElement(doc);
	}

	auto scriptSelectionProperty =	getProperties().getProperty(m_propertyNameScripts);
	if (scriptSelectionProperty->needsUpdate())
	{
		updateBlockAccordingToScriptHeader();
		createBlockItem();
	}
	getProperties().forceResetUpdateForAllProperties();
	return true;
}

void EntityBlockPython::setScriptFolder(ot::UID _scriptFolderID) {
	
	auto propBase = getProperties().getProperty(m_propertyNameScripts);
	auto scriptSelection = dynamic_cast<EntityPropertiesEntityList*>(propBase);
	scriptSelection->setEntityContainerID(_scriptFolderID);

	//auto scriptProperty = PropertyHelper::getEntityListProperty(this, m_propertyNameScripts);
	//scriptProperty->setEntityContainerID(_scriptFolderID);
}

void EntityBlockPython::setManifestFolder(ot::UID _manifestFolderID)
{
	auto propBase = getProperties().getProperty(m_propertyNameEnvironments);
	auto selectedManifest = dynamic_cast<EntityPropertiesEntityList*>(propBase);
	assert(selectedManifest != nullptr);
	selectedManifest->setEntityContainerID(_manifestFolderID);

	/*auto manifestProperty = PropertyHelper::getEntityListProperty(this, m_propertyNameEnvironments);
	manifestProperty->setEntityContainerID(_manifestFolderID);*/
}

void EntityBlockPython::updateBlockAccordingToScriptHeader()
{
	resetBlockRelatedAttributes();

	auto propertyBase =	getProperties().getProperty(m_propertyNameScripts);
	auto propertyEntityList = dynamic_cast<EntityPropertiesExtendedEntityList*>(propertyBase);
	if(propertyEntityList->getValueName() == "< Load from Library >" || propertyEntityList->getValueName() == "")
	{
		return; // No script selected, so we do not need to update the block.
	}

	//auto propertyBase = getProperties().getProperty(m_propertyNameScripts);
	//auto propertyEntityList = dynamic_cast<EntityPropertiesEntityList*>(propertyBase);
	ot::UID scriptID = propertyEntityList->getValueID();
	std::map<ot::UID,EntityBase*> entityMap;
	EntityBase* baseEntity = readEntityFromEntityID(nullptr, scriptID, entityMap);
	assert(baseEntity != nullptr);
	std::shared_ptr<EntityFile> scriptEntity(dynamic_cast<EntityFile*>(baseEntity));
	assert(scriptEntity != nullptr);
	scriptEntity->setObserver(nullptr);

	PythonHeaderInterpreter headerInterpreter;
	const bool interpretationSuccessfull = headerInterpreter.interprete(scriptEntity);

	if (interpretationSuccessfull)
	{
		auto allConnectors = headerInterpreter.getAllConnectors();
		auto allProperties = headerInterpreter.getAllProperties();

		for (auto& connector : allConnectors) 
		{
			addConnector(std::move(connector));
		}

		for (auto& property : allProperties)
		{
			getProperties().createProperty(property, property->getGroup());
		}
	}
	else
	{
		std::string report = headerInterpreter.getErrorReport();
		report = "\nError while analysing pythonscript:\n" + report;
		ot::JsonDocument cmdDoc;
		cmdDoc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_DisplayMessage,cmdDoc.GetAllocator());
		auto jReport =	ot::JsonString(report, cmdDoc.GetAllocator());
		cmdDoc.AddMember(OT_ACTION_PARAM_MESSAGE, jReport, cmdDoc.GetAllocator());
		getObserver()->sendMessageToViewer(cmdDoc);
	}
}

void EntityBlockPython::resetBlockRelatedAttributes()
{
	clearConnectors();
	
	auto allProperties = getProperties().getListOfAllProperties();
	for (auto& property : allProperties)
	{
		const std::string propertyName = property->getName();
		const std::string propertyGroupName = property->getGroup();

		if (propertyName != m_propertyNameScripts && propertyName != m_propertyNameEnvironments)
		{
			getProperties().deleteProperty(propertyName, propertyGroupName);
		}
	}
	
}

