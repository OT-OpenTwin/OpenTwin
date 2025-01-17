//! @file LineEditCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/LineEditCfg.h"
#include "OTGui/WidgetBaseCfgFactory.h"

static ot::WidgetBaseCfgFactoryRegistrar<ot::LineEditCfg> g_lineEditCfgRegistrar(ot::LineEditCfg::getLineEditCfgTypeString());

ot::LineEditCfg::LineEditCfg() :
	m_readOnly(false)
{

}

ot::LineEditCfg::LineEditCfg(const std::string& _name, const std::string& _text)
	: WidgetBaseCfg(_name), m_readOnly(false), m_text(_text)
{

}

ot::LineEditCfg::~LineEditCfg() {

}

void ot::LineEditCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetBaseCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("ReadOnly", m_readOnly, _allocator);
	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("Placeholder", JsonString(m_placeholderText, _allocator), _allocator);
}

void ot::LineEditCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetBaseCfg::setFromJsonObject(_object);

	m_readOnly = json::getBool(_object, "ReadOnly");
	m_text = json::getString(_object, "Text");
	m_placeholderText = json::getString(_object, "Placeholder");
}
