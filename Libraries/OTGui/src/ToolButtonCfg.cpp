//! @file ToolButtonCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/ToolButtonCfg.h"
#include "OTGui/WidgetBaseCfgFactory.h"

static ot::WidgetBaseCfgFactoryRegistrar<ot::ToolButtonCfg> g_toolButtonCfgRegistrar(ot::ToolButtonCfg::getToolButtonCfgTypeString());

ot::ToolButtonCfg::ToolButtonCfg() {

}

ot::ToolButtonCfg::ToolButtonCfg(const std::string& _name, const std::string& _text, const std::string& _relativeIconPath, const MenuCfg& _menu) :
	WidgetBaseCfg(_name), m_text(_text), m_iconPath(_relativeIconPath), m_menu(_menu)
{

}

ot::ToolButtonCfg::~ToolButtonCfg() {

}

void ot::ToolButtonCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetBaseCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Icon", JsonString(m_iconPath, _allocator), _allocator);
	_object.AddMember("KeySequence", JsonString(m_keySequence, _allocator), _allocator);
	_object.AddMember("Menu", JsonObject(m_menu, _allocator), _allocator);
}

void ot::ToolButtonCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetBaseCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, "Text");
	m_iconPath = json::getString(_object, "Icon");
	m_keySequence = json::getString(_object, "KeySequence");
	m_menu.setFromJsonObject(json::getObject(_object, "Menu"));
}
