//! @file GraphicsCopyInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/CopyInformationFactory.h"
#include "OTGui/GraphicsCopyInformation.h"

static ot::CopyInformationFactoryRegistrar<ot::GraphicsCopyInformation> graphicsCopyInformationRegistrar(ot::GraphicsCopyInformation::getGraphicsCopyInformationType());

void ot::GraphicsCopyInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	CopyInformation::addToJsonObject(_object, _allocator);


}

void ot::GraphicsCopyInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	CopyInformation::setFromJsonObject(_object);


}
/*
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
}*/

