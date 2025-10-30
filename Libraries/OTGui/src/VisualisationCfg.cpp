// @otlicense

// OpenTwin header
#include "OTGui/VisualisationCfg.h"

void ot::VisualisationCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("SetAsActiveView", m_setAsActiveView, _allocator);
	_object.AddMember("OverWriteContent", m_overrideViewerContent, _allocator);
	_object.AddMember("VisualisingEntities", JsonArray(m_visualisingEntities, _allocator), _allocator);
	_object.AddMember("SupressViewHandling", m_supressViewHandling, _allocator);
	_object.AddMember("VisualisationType", JsonString(m_visualisationType, _allocator), _allocator);

	if (m_customViewFlags.has_value()) {
		_object.AddMember("CustomViewFlags", JsonArray(WidgetViewBase::toStringList(m_customViewFlags.value()), _allocator), _allocator);
	}
	else {
		_object.AddMember("CustomViewFlags", JsonNullValue(), _allocator);
	}
}

void ot::VisualisationCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_setAsActiveView = json::getBool(_object,"SetAsActiveView");
	m_overrideViewerContent = json::getBool(_object, "OverWriteContent");
	m_visualisingEntities = json::getUInt64List(_object, "VisualisingEntities");
	m_supressViewHandling = json::getBool(_object, "SupressViewHandling");
	m_visualisationType = json::getString(_object, "VisualisationType");

	if (_object["CustomViewFlags"].IsNull()) {
		m_customViewFlags = std::nullopt;
	}
	else {
		m_customViewFlags = WidgetViewBase::stringListToViewFlags(json::getStringList(_object, "CustomViewFlags"));
	}
}
