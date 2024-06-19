//! @file GraphicsInvisibleItemCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsInvisibleItemCfg.h"

#define OT_JSON_MEMBER_Size "Size"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsInvisibleItemCfg> rectItemCfg(OT_FactoryKey_GraphicsInvisibleItem);

ot::GraphicsInvisibleItemCfg::GraphicsInvisibleItemCfg(const ot::Size2DD& _size)
	: m_size(_size)
{}

ot::GraphicsInvisibleItemCfg::~GraphicsInvisibleItemCfg() {}

ot::GraphicsItemCfg* ot::GraphicsInvisibleItemCfg::createCopy(void) const {
	ot::GraphicsInvisibleItemCfg* copy = new GraphicsInvisibleItemCfg;
	this->copyConfigDataToItem(copy);

	copy->m_size = m_size;

	return copy;
}

void ot::GraphicsInvisibleItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);

	JsonObject sizeObj;
	m_size.addToJsonObject(sizeObj, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Size, sizeObj, _allocator);
}

void ot::GraphicsInvisibleItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	m_size.setFromJsonObject(json::getObject(_object, OT_JSON_MEMBER_Size));
}
