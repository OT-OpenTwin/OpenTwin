// Entity.cpp : Defines the Entity class which is exported for the DLL application.
//

#include "OTCommunication/ActionTypes.h"
#include "EntityResultText.h"
#include "EntityResultTextData.h"
#include "DataBase.h"
#include "OldTreeIcon.h"
#include "OTGui/VisualisationTypes.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityResultText::EntityResultText(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityContainer(ID, parent, obs, ms, factory, owner),
	textData(nullptr),
	textDataStorageId(-1),
	textDataStorageVersion(-1)
{
}

EntityResultText::~EntityResultText()
{
}

bool EntityResultText::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResultText::StoreToDataBase(void)
{
	if (textData != nullptr)
	{
		storeTextData();
	}

	EntityBase::StoreToDataBase();
}

void EntityResultText::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityContainer::AddStorageData(storage);

	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("TextDataID", textDataStorageId),
		bsoncxx::builder::basic::kvp("TextDataVersion", textDataStorageVersion)
	);
}

void EntityResultText::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	textDataStorageId = doc_view["TextDataID"].get_int64();
	textDataStorageVersion = doc_view["TextDataVersion"].get_int64();

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

void EntityResultText::removeChild(EntityBase *child)
{
	if (child == textData)
	{
		textData = nullptr;
	}

	EntityContainer::removeChild(child);
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
	EnsureTextDataLoaded();
	assert(textData != nullptr);

	return textData;
}

std::string EntityResultText::getText()
{
	EnsureTextDataLoaded();
	assert(textData != nullptr);

	return textData->getText();
}

bool EntityResultText::visualiseText()
{
	return true;
}

ot::TextEditorCfg EntityResultText::createConfig()
{
	ot::TextEditorCfg result;
	result.setEntityName(this->getName());
	result.setTitle(this->getName());
	result.setPlainText(this->getText());

	result.setDocumentSyntax(ot::DocumentSyntax::PlainText);
	
	return result;
}

ot::ContentChangedHandling EntityResultText::getTextContentChangedHandling()
{
	return m_contentChangedHandling;
}

void EntityResultText::deleteTextData(void)
{
	textData = nullptr;
	textDataStorageId = -1;
	textDataStorageVersion = -1;

	setModified();
}

void EntityResultText::storeTextData(void)
{
	if (textData == nullptr) return;
	assert(textData != nullptr);

	textData->StoreToDataBase();

	if (textDataStorageId != textData->getEntityID() || textDataStorageVersion != textData->getEntityStorageVersion())
	{
		setModified();
	}

	textDataStorageId = textData->getEntityID();
	textDataStorageVersion = textData->getEntityStorageVersion();
}

void EntityResultText::releaseTextData(void)
{
	if (textData == nullptr) return;

	storeTextData();

	delete textData;
	textData = nullptr;
}

void EntityResultText::EnsureTextDataLoaded(void)
{
	if (textData == nullptr)
	{
		if (textDataStorageId == -1)
		{
			textData = new EntityResultTextData(getUidGenerator()->getUID(), this, getObserver(), getModelState(), getClassFactory(), getOwningService());
		}
		else
		{
			std::map<ot::UID, EntityBase *> entityMap;
			textData = dynamic_cast<EntityResultTextData *>(readEntityFromEntityID(this, textDataStorageId, entityMap));
		}

		assert(textData != nullptr);
		addChild(textData);
	}
}

void EntityResultText::setText(const std::string &text)
{
	EnsureTextDataLoaded();
	assert(textData != nullptr);

	textData->setText(text);

	storeTextData();
}