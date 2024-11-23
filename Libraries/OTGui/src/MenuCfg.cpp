//! @file MenuCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/MenuCfg.h"
#include "OTGui/MenuSeparatorCfg.h"
#include "OTGui/MenuEntryCfgFactory.h"

static ot::MenuEntryCfgFactoryRegistrar<ot::MenuCfg> menuRegistarar(ot::MenuEntryCfg::toString(ot::MenuEntryCfg::Menu));

ot::MenuCfg::MenuCfg() {

}

ot::MenuCfg::MenuCfg(const std::string& _name, const std::string& _text, const std::string& _iconPath)
	: MenuClickableEntryCfg(_name, _text, _iconPath)
{

}

ot::MenuCfg::MenuCfg(const MenuCfg& _other)
	: MenuClickableEntryCfg(_other)
{
	for (const MenuEntryCfg* entry : _other.getEntries()) {
		OTAssertNullptr(entry);
		this->add(entry->createCopy());
	}
}

ot::MenuCfg::MenuCfg(const ot::ConstJsonObject& _object) {
	this->setFromJsonObject(_object);
}

ot::MenuCfg::~MenuCfg() {
	this->clear();
}

ot::MenuEntryCfg* ot::MenuCfg::createCopy(void) const {
	return new MenuCfg(*this);
}

void ot::MenuCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	MenuClickableEntryCfg::addToJsonObject(_object, _allocator);

	JsonArray entryArr;
	for (const MenuEntryCfg* entry : m_childs) {
		OTAssertNullptr(entry);
		JsonObject entryObj;
		entry->addToJsonObject(entryObj, _allocator);
		entryArr.PushBack(entryObj, _allocator);
	}
	_object.AddMember("Childs", entryArr, _allocator);
}

void ot::MenuCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	MenuClickableEntryCfg::setFromJsonObject(_object);

	this->clear();
	
	for (const ConstJsonObject& entryObj : json::getObjectList(_object, "Childs")) {
		MenuEntryCfg* newEntry = MenuEntryCfgFactory::create(entryObj);
		if (newEntry) {
			this->add(newEntry);
		}
	}
}

void ot::MenuCfg::add(MenuEntryCfg* _entry) {
	OTAssertNullptr(_entry);
	m_childs.push_back(_entry);
}

ot::MenuCfg* ot::MenuCfg::addMenu(const std::string& _name, const std::string& _text, const std::string& _iconPath) {
	MenuCfg* newMenu = new MenuCfg(_name, _text, _iconPath);
	this->add(newMenu);
	return newMenu;
}

ot::MenuButtonCfg* ot::MenuCfg::addButton(const std::string& _name, const std::string& _text, const std::string& _iconPath, MenuButtonCfg::ButtonAction _action) {
	MenuButtonCfg* newItem = new MenuButtonCfg(_name, _text, _iconPath, _action);
	this->add(newItem);
	return newItem;
}

void ot::MenuCfg::addSeparator(void) {
	MenuSeparatorCfg* newSeparator = new MenuSeparatorCfg;
	this->add(newSeparator);
}

void ot::MenuCfg::clear(void) {
	for (MenuEntryCfg* child : m_childs) {
		delete child;
	}
	m_childs.clear();
}
