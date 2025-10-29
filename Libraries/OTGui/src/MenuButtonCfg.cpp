// @otlicense

//! @file MenuButtonCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/MenuButtonCfg.h"
#include "OTGui/MenuEntryCfgFactory.h"

static ot::MenuEntryCfgFactoryRegistrar<ot::MenuButtonCfg> buttonRegistarar(ot::MenuEntryCfg::toString(ot::MenuEntryCfg::Button));

std::string ot::MenuButtonCfg::toString(ButtonAction _action) {
	switch (_action) {
	case ot::MenuButtonCfg::NotifyOwner: return "NotifyOwner";
	case ot::MenuButtonCfg::Clear: return "Clear";
	default:
		OT_LOG_EAS("Unknown button action (" + std::to_string((int)_action));
		return "NotifyOwner";
	}
}

ot::MenuButtonCfg::ButtonAction ot::MenuButtonCfg::stringToButtonAction(const std::string& _action) {
	if (_action == MenuButtonCfg::toString(ButtonAction::NotifyOwner)) return ButtonAction::NotifyOwner;
	else if (_action == MenuButtonCfg::toString(ButtonAction::Clear)) return ButtonAction::Clear;
	else {
		OT_LOG_EAS("Unknown button action \"" + _action + "\"");
		return ButtonAction::NotifyOwner;
	}
}

ot::MenuButtonCfg::MenuButtonCfg() 
	: m_action(ButtonAction::NotifyOwner)
{

}

ot::MenuButtonCfg::MenuButtonCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath, ButtonAction _action) 
	: MenuClickableEntryCfg(_name, _text, _iconPath), m_action(_action)
{

}

ot::MenuButtonCfg::MenuButtonCfg(const MenuButtonCfg& _other)
	: MenuClickableEntryCfg(_other), m_action(_other.m_action)
{

}

ot::MenuButtonCfg::MenuButtonCfg(const ot::ConstJsonObject& _object) {
	this->setFromJsonObject(_object);
}

ot::MenuButtonCfg::~MenuButtonCfg() {

}

ot::MenuEntryCfg* ot::MenuButtonCfg::createCopy(void) const {
	return new MenuButtonCfg(*this);
}

void ot::MenuButtonCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	MenuClickableEntryCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Action", JsonString(this->toString(m_action), _allocator), _allocator);
}

void ot::MenuButtonCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	MenuClickableEntryCfg::setFromJsonObject(_object);

	m_action = this->stringToButtonAction(json::getString(_object, "Action"));
}
