#include "OTGui/VisualisationCfg.h"

void ot::VisualisationCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const
{
	_object.AddMember("SetAsActiveView", m_setAsActiveView, _allocator);
	_object.AddMember("OverWriteContent", m_overrideViewerContent, _allocator);
	_object.AddMember("VisualisingEntities", ot::JsonArray(m_visualisingEntities, _allocator), _allocator);
	_object.AddMember("SupressViewHandling", m_supressViewHandling, _allocator);
	_object.AddMember("VisualisationType", ot::JsonString(m_visualisationType, _allocator), _allocator);
}

void ot::VisualisationCfg::setFromJsonObject(const ot::ConstJsonObject& _object)
{
	m_setAsActiveView = ot::json::getBool(_object,"SetAsActiveView");
	m_overrideViewerContent = ot::json::getBool(_object, "OverWriteContent");
	m_visualisingEntities = ot::json::getUInt64List(_object, "VisualisingEntities");
	m_supressViewHandling = ot::json::getBool(_object, "SupressViewHandling");
	m_visualisationType = ot::json::getString(_object, "VisualisationType");
}
