//! @file GraphicsItemFileCfg.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Painter2DFactory.h"
#include "OTGui/GraphicsItemFileCfg.h"
#include "OTGui/GraphicsItemCfgFactory.h"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsItemFileCfg> fileItemCfg(OT_FactoryKey_GraphicsFileItem);

ot::GraphicsItemFileCfg::GraphicsItemFileCfg()
{}

ot::GraphicsItemFileCfg::GraphicsItemFileCfg(const std::string& _subPath)
	: m_file(_subPath)
{}


ot::GraphicsItemFileCfg::GraphicsItemFileCfg(const GraphicsItemFileCfg& _other) 
	: GraphicsItemCfg(_other), m_file(_other.m_file)
{

}

ot::GraphicsItemFileCfg::~GraphicsItemFileCfg() {
	
}

ot::GraphicsItemCfg* ot::GraphicsItemFileCfg::createCopy(void) const {
	return new GraphicsItemFileCfg;
}

void ot::GraphicsItemFileCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	GraphicsItemCfg::addToJsonObject(_object, _allocator);
	_object.AddMember("File", JsonString(m_file, _allocator), _allocator);
}

void ot::GraphicsItemFileCfg::setFromJsonObject(const ConstJsonObject& _object) {
	GraphicsItemCfg::setFromJsonObject(_object);
	m_file = json::getString(_object, "File");
}

// ###########################################################################################################################################################################################################################################################################################################################

// Setter / Getter
