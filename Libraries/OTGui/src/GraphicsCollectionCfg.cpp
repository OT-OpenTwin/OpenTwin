// OpenTwin header
#include "OTGui/GraphicsCollectionCfg.h"
#include "OTGui/GraphicsItemCfg.h"
#include "OTCore/SimpleFactory.h"
#include "OTCore/Logger.h"

#define OT_JSON_Member_Name "Name"
#define OT_JSON_Member_Title "Title"
#define OT_JSON_Member_Items "Items"
#define OT_JSON_Member_Collections "Collections"

ot::GraphicsCollectionCfg::GraphicsCollectionCfg() {}

ot::GraphicsCollectionCfg::GraphicsCollectionCfg(const std::string& _collectionName, const std::string& _collectionTitle) : m_name(_collectionName), m_title(_collectionTitle) {

}

ot::GraphicsCollectionCfg::~GraphicsCollectionCfg() {
	this->memFree();
}

void ot::GraphicsCollectionCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_Member_Name, JsonString(m_name, _allocator), _allocator);
	_object.AddMember(OT_JSON_Member_Title, JsonString(m_title, _allocator), _allocator);

	JsonArray collectionArr;
	for (auto c : m_collections) {
		JsonObject collectionObj;
		c->addToJsonObject(collectionObj, _allocator);
		collectionArr.PushBack(collectionObj, _allocator);
	}
	_object.AddMember(OT_JSON_Member_Collections, collectionArr, _allocator);

	JsonArray itemArr;
	for (auto i : m_items) {
		JsonObject itemObj;
		i->addToJsonObject(itemObj, _allocator);
		itemArr.PushBack(itemObj, _allocator);
	}
	_object.AddMember(OT_JSON_Member_Items, itemArr, _allocator);
}

void ot::GraphicsCollectionCfg::setFromJsonObject(const ConstJsonObject& _object) {
	this->memFree();

	m_name = json::getString(_object, OT_JSON_Member_Name);
	m_title = json::getString(_object, OT_JSON_Member_Title);

	ConstJsonArray collectionArr = json::getArray(_object, OT_JSON_Member_Collections);
	for (rapidjson::SizeType i = 0; i < collectionArr.Size(); i++) {
		ConstJsonObject collectionObj = json::getObject(collectionArr, i);
		GraphicsCollectionCfg* newChild = new GraphicsCollectionCfg;
		try {
			newChild->setFromJsonObject(collectionObj);
			m_collections.push_back(newChild);
		}
		catch (...) {
			OT_LOG_E("Failed to create child collection. Abort");
			delete newChild;
			this->memFree();
			throw std::exception("Failed to create graphics collection");
		}
	}

	ConstJsonArray itemArr = json::getArray(_object, OT_JSON_Member_Items);
	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		ConstJsonObject itemObj = json::getObject(itemArr, i);

		GraphicsItemCfg* itm = nullptr;
		try {
			
			itm = ot::SimpleFactory::instance().createType<GraphicsItemCfg>(itemObj);
			itm->setFromJsonObject(itemObj);
			m_items.push_back(itm);
		}
		catch (const std::exception& _e) {
			OT_LOG_E("Failed to create item by factory. " + std::string(_e.what()) + ". Abort");
			if (itm) delete itm;
			this->memFree();
			throw std::exception("Failed to create graphics item");
		}
		catch (...) {
			OT_LOG_E("Failed to create item by factory. Unknown error. Abort");
			if (itm) delete itm;
			this->memFree();
			throw std::exception("Failed to create graphics item");
		}
	}
}

void ot::GraphicsCollectionCfg::addChildCollection(GraphicsCollectionCfg* _child) {
	OTAssertNullptr(_child);
	m_collections.push_back(_child);
}

void ot::GraphicsCollectionCfg::addItem(GraphicsItemCfg* _item) {
	OTAssertNullptr(_item);
	m_items.push_back(_item);
}

void ot::GraphicsCollectionCfg::memFree(void) {
	for (auto c : m_collections) delete c;
	m_collections.clear();

	for (auto i : m_items) delete i;
	m_items.clear();

}
