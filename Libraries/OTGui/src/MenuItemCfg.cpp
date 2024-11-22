//! @file MenuItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/MenuItemCfg.h"
#include "OTGui/MenuEntryCfgFactory.h"

ot::MenuEntryCfgFactoryRegistrar<ot::MenuItemCfg> itemRegistarar(ot::MenuEntryCfg::toString(ot::MenuEntryCfg::Item));

ot::MenuItemCfg::MenuItemCfg() {

}

ot::MenuItemCfg::MenuItemCfg(const std::string& _text, const std::string& _iconPath)
	: m_text(_text), m_iconPath(_iconPath)
{

}

ot::MenuItemCfg::MenuItemCfg(const MenuItemCfg& _other) 
	: MenuEntryCfg(_other), m_text(_other.m_text), m_iconPath(_other.m_iconPath)
{

}

ot::MenuItemCfg::MenuItemCfg(const ot::ConstJsonObject& _object) {
	this->setFromJsonObject(_object);
}

ot::MenuItemCfg::~MenuItemCfg() {

}

ot::MenuEntryCfg* ot::MenuItemCfg::createCopy(void) const {
	return new MenuItemCfg(*this);
}

void ot::MenuItemCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	MenuEntryCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Icon", JsonString(m_iconPath, _allocator), _allocator);
}

void ot::MenuItemCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	MenuEntryCfg::setFromJsonObject(_object);

	m_text = json::getString(_object, "Text");
	m_iconPath = json::getString(_object, "Icon");
}
