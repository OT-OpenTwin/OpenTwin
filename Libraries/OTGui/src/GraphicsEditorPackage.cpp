//! @file GraphicsEditorPackage.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################


// OpenTwin header
#include "OTGui/GraphicsEditorPackage.h"
#include "OTGui/GraphicsCollectionCfg.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_Member_Collections "Collections"

ot::GraphicsEditorPackage::GraphicsEditorPackage() {

}

ot::GraphicsEditorPackage::~GraphicsEditorPackage() {
	this->memFree();
}

void ot::GraphicsEditorPackage::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	OT_rJSON_createValueArray(collectionArr);
	for (auto c : m_collections) {
		OT_rJSON_createValueObject(collectionObj);
		c->addToJsonObject(_document, collectionObj);
		collectionArr.PushBack(collectionObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_Member_Collections, collectionArr);
}

void ot::GraphicsEditorPackage::setFromJsonObject(OT_rJSON_val& _object) {
	this->memFree();

	OT_rJSON_checkMember(_object, OT_JSON_Member_Collections, Array);
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
			return;
		}
	}
}

void ot::GraphicsEditorPackage::addCollection(GraphicsCollectionCfg* _collection) {
	m_collections.push_back(_collection);
}

void ot::GraphicsEditorPackage::memFree(void) {
	for (auto c : m_collections) delete c;
}
