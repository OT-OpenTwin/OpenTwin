// @otlicense

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

ot::MenuCfg& ot::MenuCfg::operator=(const MenuCfg& _other) {
	if (this != &_other) {
		this->clear();

		MenuClickableEntryCfg::operator=(_other);

		for (const MenuEntryCfg* entry : _other.getEntries()) {
			OTAssertNullptr(entry);
			this->add(entry->createCopy());
		}
	}

	return *this;
}

ot::MenuEntryCfg* ot::MenuCfg::createCopy(void) const {
	return new MenuCfg(*this);
}

void ot::MenuCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	MenuClickableEntryCfg::addToJsonObject(_object, _allocator);

	JsonArray childArr;
	for (const MenuEntryCfg* child : m_childs) {
		OTAssertNullptr(child);
		JsonObject childObj;
		child->addToJsonObject(childObj, _allocator);
		childArr.PushBack(childObj, _allocator);
	}
	_object.AddMember("Childs", childArr, _allocator);
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

ot::MenuButtonCfg* ot::MenuCfg::findMenuButton(const std::string& _name) const {
	for (MenuEntryCfg* child : m_childs) {
		OTAssertNullptr(child);
		const MenuCfg* childMenu = dynamic_cast<const MenuCfg*>(child);
		if (childMenu) {
			ot::MenuButtonCfg* result = childMenu->findMenuButton(_name);
			if (result) {
				return result;
			}
		}
		else {
			MenuButtonCfg* button = dynamic_cast<MenuButtonCfg*>(child);
			if (button) {
				if (button->getName() == _name) {
					return button;
				}
			}
		}
	}

	return nullptr;
}

bool ot::MenuCfg::isEmpty(void) const {
	for (const MenuEntryCfg* child : m_childs) {
		const MenuButtonCfg* button = dynamic_cast<const MenuButtonCfg*>(child);
		if (button) {
			return false;
		}
		else {
			const MenuCfg* childMenu = dynamic_cast<const MenuCfg*>(child);
			if (childMenu) {
				if (!childMenu->isEmpty()) {
					return false;
				}
			}
		}
	}

	return true;
}

void ot::MenuCfg::clear(void) {
	for (MenuEntryCfg* child : m_childs) {
		delete child;
	}
	m_childs.clear();
}
