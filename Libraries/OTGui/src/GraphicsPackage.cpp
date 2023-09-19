//! @file GraphicsEditorPackage.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_Member_Name "Name"
#define OT_JSON_Member_Title "Title"
#define OT_JSON_Member_Collections "Collections"

ot::GraphicsItemPickerPackage::GraphicsItemPickerPackage(const std::string& _packageName, const std::string& _editorTitle)
	: m_name(_packageName), m_title(_editorTitle) {}

ot::GraphicsItemPickerPackage::~GraphicsItemPickerPackage() {
	this->memFree();
}

void ot::GraphicsItemPickerPackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueArray(collectionArr);
	for (auto c : m_collections) {
		OT_rJSON_createValueObject(collectionObj);
		c->addToJsonObject(_document, collectionObj);
		collectionArr.PushBack(collectionObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_Member_Name, m_name);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Title, m_title);
	ot::rJSON::add(_document, _object, OT_JSON_Member_Collections, collectionArr);
}

void ot::GraphicsItemPickerPackage::setFromJsonObject(OT_rJSON_val& _object) {
	this->memFree();

	OT_rJSON_checkMember(_object, OT_JSON_Member_Name, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Title, String);
	OT_rJSON_checkMember(_object, OT_JSON_Member_Collections, Array);

	m_name = _object[OT_JSON_Member_Name].GetString();
	m_title = _object[OT_JSON_Member_Title].GetString();

	OT_rJSON_val collectionArr = _object[OT_JSON_Member_Collections].GetArray();
	for (rapidjson::SizeType i = 0; i < collectionArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(collectionArr, i, Object);
		OT_rJSON_val collectionObj = collectionArr[i].GetObject();
		GraphicsCollectionCfg* newCollection = new GraphicsCollectionCfg;
		try {
			newCollection->setFromJsonObject(collectionObj);
			m_collections.push_back(newCollection);
		}
		catch (const std::exception& _e) {
			OT_LOG_E(_e.what());
			this->memFree();
			throw _e;
		}
	}
}

void ot::GraphicsItemPickerPackage::addCollection(GraphicsCollectionCfg* _collection) {
	m_collections.push_back(_collection);
}

void ot::GraphicsItemPickerPackage::memFree(void) {
	for (auto c : m_collections) delete c;
}
