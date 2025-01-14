//! @file GraphicsCopyInformation.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsCopyInformation.h"

void ot::GraphicsCopyInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("ViewName", JsonString(m_viewName, _allocator), _allocator);
	JsonArray itemsArr;
	for (const ItemInformation& info : m_items) {
		JsonObject itemObj;
		itemObj.AddMember("UID", info.uid, _allocator);

		JsonObject posObj;
		info.pos.addToJsonObject(posObj, _allocator);
		itemObj.AddMember("Pos", posObj, _allocator);

		itemsArr.PushBack(itemObj, _allocator);
	}
	_object.AddMember("Items", itemsArr, _allocator);
}

void ot::GraphicsCopyInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_items.clear();

	m_viewName = json::getString(_object, "ViewName");

	for (const ConstJsonObject& itemObj : json::getObjectList(_object, "Items")) {
		ItemInformation info;
		info.uid = json::getUInt64(itemObj, "UID");
		info.pos.setFromJsonObject(json::getObject(itemObj, "Pos"));
		m_items.push_back(info);
	}
}

void ot::GraphicsCopyInformation::addItemInformation(UID _uid, const Point2DD& _pos) {
	ItemInformation newInfo;
	newInfo.uid = _uid;
	newInfo.pos = _pos;

	this->addItemInformation(newInfo);
}

void ot::GraphicsCopyInformation::addItemInformation(const ItemInformation& _info) {
	m_items.push_back(_info);
}

void ot::GraphicsCopyInformation::moveItemsToPoint(const Point2DD& _pos) {
	Point2DD minPos(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

	// Get min values
	for (ItemInformation& info : m_items) {
		if (info.pos.x() < minPos.x()) {
			minPos.setX(info.pos.x());
		}
		if (info.pos.y() < minPos.y()) {
			minPos.setY(info.pos.y());
		}
	}

	// Move
	for (ItemInformation& info : m_items) {
		info.pos = (info.pos - minPos) + _pos;
	}
}

void ot::GraphicsCopyInformation::moveItemsBy(const Point2DD& _dist) {
	for (ItemInformation& info : m_items) {
		info.pos = info.pos + _dist;
	}
}
