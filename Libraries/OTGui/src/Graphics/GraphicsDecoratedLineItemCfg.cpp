// @otlicense

// OpenTwin header
#include "OTGui/Graphics/GraphicsItemCfgFactory.h"
#include "OTGui/Graphics/GraphicsDecoratedLineItemCfg.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsDecoratedLineItemCfg> registrar(ot::GraphicsDecoratedLineItemCfg::className());

ot::GraphicsDecoratedLineItemCfg::GraphicsDecoratedLineItemCfg() {

}

ot::GraphicsDecoratedLineItemCfg::GraphicsDecoratedLineItemCfg(const GraphicsDecoratedLineItemCfg& _other)
	: GraphicsLineItemCfg(_other), m_fromDecoration(_other.m_fromDecoration), m_toDecoration(_other.m_toDecoration)
{}

ot::GraphicsDecoratedLineItemCfg::~GraphicsDecoratedLineItemCfg() {}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

void ot::GraphicsDecoratedLineItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsLineItemCfg::addToJsonObject(_object, _allocator);

	_object.AddMember("FromDecoration", JsonObject(m_fromDecoration, _allocator), _allocator);
	_object.AddMember("ToDecoration", JsonObject(m_toDecoration, _allocator), _allocator);
}

void ot::GraphicsDecoratedLineItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsLineItemCfg::setFromJsonObject(_object);

	m_fromDecoration.setFromJsonObject(json::getObject(_object, "FromDecoration"));
	m_toDecoration.setFromJsonObject(json::getObject(_object, "ToDecoration"));
}
