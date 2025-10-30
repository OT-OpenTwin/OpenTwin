// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsBoxLayoutItemCfg.h"

#define OT_JSON_MEMBER_Item "Item"
#define OT_JSON_MEMBER_Items "Items"
#define OT_JSON_MEMBER_Stretch "Stretch"
#define OT_JSON_MEMBER_Orientation "Orientation"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsBoxLayoutItemCfg> registrar(ot::GraphicsBoxLayoutItemCfg::className());

ot::GraphicsBoxLayoutItemCfg::GraphicsBoxLayoutItemCfg(ot::Orientation _orientation)
	: m_orientation(_orientation)
{}

ot::GraphicsBoxLayoutItemCfg::GraphicsBoxLayoutItemCfg(const GraphicsBoxLayoutItemCfg & _other) : GraphicsLayoutItemCfg(_other) {
	m_orientation = _other.m_orientation;

	for (const itemStrechPair_t& itm : _other.m_items) {
		itemStrechPair_t newEntry;
		newEntry.second = itm.second;
		if (itm.first) {
			newEntry.first = itm.first->createCopy();
		}
		else {
			newEntry.first = nullptr;
		}
		m_items.push_back(newEntry);
	}
}

ot::GraphicsBoxLayoutItemCfg::~GraphicsBoxLayoutItemCfg() {
	this->clearItems();
}

void ot::GraphicsBoxLayoutItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GraphicsLayoutItemCfg::addToJsonObject(_object, _allocator);

	JsonArray itemArr;
	for (auto i : m_items) {
		JsonObject pairObj;
		pairObj.AddMember(OT_JSON_MEMBER_Stretch, i.second, _allocator);
		if (i.first == nullptr) {
			JsonNullValue itemObj;
			pairObj.AddMember(OT_JSON_MEMBER_Item, itemObj, _allocator);
		}
		else {
			JsonObject itemObj;
			i.first->addToJsonObject(itemObj, _allocator);
			pairObj.AddMember(OT_JSON_MEMBER_Item, itemObj, _allocator);
		}
		itemArr.PushBack(pairObj, _allocator);
	}

	_object.AddMember(OT_JSON_MEMBER_Items, itemArr, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Orientation, JsonString(ot::toString(m_orientation), _allocator), _allocator);
}

void ot::GraphicsBoxLayoutItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	this->clearItems();

	ot::GraphicsLayoutItemCfg::setFromJsonObject(_object);

	m_orientation = stringToOrientation(json::getString(_object, OT_JSON_MEMBER_Orientation));
	ConstJsonArray itemArr = json::getArray(_object, OT_JSON_MEMBER_Items);

	for (rapidjson::SizeType i = 0; i < itemArr.Size(); i++) {
		ConstJsonObject pairObj = json::getObject(itemArr, i);
		if (!pairObj.HasMember(OT_JSON_MEMBER_Item)) { OT_LOG_EA("Invalid item entry"); return; }

		if (pairObj[OT_JSON_MEMBER_Item].IsNull()) {
			m_items.push_back(itemStrechPair_t(nullptr, pairObj[OT_JSON_MEMBER_Stretch].GetInt()));
		}
		else if (pairObj[OT_JSON_MEMBER_Item].IsObject()) {
			GraphicsItemCfg* itm = nullptr;
			ConstJsonObject itemObj = json::getObject(pairObj, OT_JSON_MEMBER_Item);
			itm = GraphicsItemCfgFactory::create(itemObj);
			if (!itm) {
				continue;
			}
			m_items.push_back(itemStrechPair_t(itm, json::getInt(pairObj, OT_JSON_MEMBER_Stretch)));
		}
		else {
			OT_LOG_EAS("Invalid JSON Object type");
		}
	}
}

void ot::GraphicsBoxLayoutItemCfg::addChildItem(ot::GraphicsItemCfg* _item) {
	this->addChildItem(_item, 0);
}

void ot::GraphicsBoxLayoutItemCfg::addChildItem(ot::GraphicsItemCfg* _item, int _stretch) {
	OTAssert(_stretch >= 0, "Stretch should be greater or equal to 0");
	m_items.push_back(itemStrechPair_t(_item, _stretch));
}

void ot::GraphicsBoxLayoutItemCfg::addStrech(int _stretch) {
	OTAssert(_stretch > 0, "Stretch should be greater than 0");
	m_items.push_back(itemStrechPair_t(nullptr, _stretch));
}

void ot::GraphicsBoxLayoutItemCfg::clearItems(void) {
	for (auto i : m_items) {
		if (i.first) delete i.first;
	}
	m_items.clear();
}
