// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/TableHeaderItemCfg.h"

ot::TableHeaderItemCfg::TableHeaderItemCfg() {}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const std::string& _text) 
	: m_text(_text)
{

}

ot::TableHeaderItemCfg::TableHeaderItemCfg(const TableHeaderItemCfg& _other) 
	: m_text(_other.m_text)
{

}

ot::TableHeaderItemCfg::~TableHeaderItemCfg() {

}

ot::TableHeaderItemCfg& ot::TableHeaderItemCfg::operator = (const TableHeaderItemCfg& _other) {
	if (this == &_other) return *this;

	m_text = _other.m_text;
	
	return *this;
}

void ot::TableHeaderItemCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
}

void ot::TableHeaderItemCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_text = json::getString(_object, "Text");
}
