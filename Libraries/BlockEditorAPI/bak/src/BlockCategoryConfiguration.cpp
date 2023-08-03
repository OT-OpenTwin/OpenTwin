// OpenTwin header
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockConfigurationFactory.h"

#define JSON_MEMBER_Title "Title"
#define JSON_MEMBER_IconSubPath "IconSubPath"
#define JSON_MEMBER_Childs "Childs"
#define JSON_MEMBER_Items "Items"

ot::BlockCategoryConfiguration::BlockCategoryConfiguration(const std::string& _name, const std::string& _title) 
	: BlockConfigurationObject(_name), m_parentCategory(nullptr), m_title(_title)
{

}

ot::BlockCategoryConfiguration::~BlockCategoryConfiguration() {
	for (auto c : m_childs) delete c;
	for (auto i : m_items) delete i;
}

void ot::BlockCategoryConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockConfigurationObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Title, this->title());
	ot::rJSON::add(_document, _object, JSON_MEMBER_IconSubPath, m_iconSubPath);

	OT_rJSON_createValueArray(childArr);
	for (auto c : m_childs) {
		OT_rJSON_createValueObject(childObj);
		c->addToJsonObject(_document, childObj);
		childArr.PushBack(childObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, JSON_MEMBER_Childs, childArr);

	OT_rJSON_createValueArray(itemArr);
	for (auto i : m_items) {
		OT_rJSON_createValueObject(itemObj);
		i->addToJsonObject(_document, itemObj);
		itemArr.PushBack(itemObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, JSON_MEMBER_Items, itemArr);
}

void ot::BlockCategoryConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfigurationObject::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_IconSubPath, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Childs, Array);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Items, Array);

	this->setTitle(_object[JSON_MEMBER_Title].GetString());
	m_iconSubPath = _object[JSON_MEMBER_IconSubPath].GetString();

	OT_rJSON_val childArr = _object[JSON_MEMBER_Childs].GetArray();
	for (rapidjson::SizeType i = 0; i < childArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(childArr, i, Object);
		
		OT_rJSON_val childObj = childArr[i].GetObject();
		BlockCategoryConfiguration* newChild = new BlockCategoryConfiguration;
		try {
			newChild->setFromJsonObject(childObj);
			addChild(newChild);
		}
		catch (const std::exception& _e) {
			delete newChild;
			throw _e;
		}
		catch (...) {
			delete newChild;
			throw std::exception("Fatal: Unknown error");
		}
	}

	OT_rJSON_val itemArr = _object[JSON_MEMBER_Items].GetArray();
	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(itemArr, i, Object);

		OT_rJSON_val itemObj = itemArr[i].GetObject();
		BlockConfiguration* newItem = BlockConfigurationFactory::blockConfigurationFromJson(itemObj);
		addItem(newItem);
	}
}

// ########################################################################################################################################################

// Setter/Getter

ot::BlockCategoryConfiguration* ot::BlockCategoryConfiguration::addChild(const std::string& _name) {
	BlockCategoryConfiguration* newChild = new BlockCategoryConfiguration(_name);
	addChild(newChild);
	return newChild;
}

void ot::BlockCategoryConfiguration::addChild(BlockCategoryConfiguration* _category) {
	// In debug mode we check for forbidden characters in the category name
	// In release mode a valid name is expected (refer to Documentation)
	otAssert(_category->name().find('|') == std::string::npos, "Forbidden character in BlockCategoryConfiguration name");

	_category->setParentCategory(this);
	m_childs.push_back(_category);
	
}

void ot::BlockCategoryConfiguration::addItem(BlockConfiguration* _item) {
	// In debug mode we check for forbidden characters in the category name
	// In release mode a valid name is expected (refer to Documentation)
	otAssert(_item->name().find('|') == std::string::npos, "Forbidden character in BlockConfiguration name");

	_item->setParentCategory(this);
	m_items.push_back(_item);
}