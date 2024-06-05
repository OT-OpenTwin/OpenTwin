//! @file GraphicsItemDesignerExportConfig.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerExportConfig.h"

GraphicsItemDesignerExportConfig::GraphicsItemDesignerExportConfig() 
	: m_name("New Item")
{

}

GraphicsItemDesignerExportConfig::GraphicsItemDesignerExportConfig(const GraphicsItemDesignerExportConfig& _other) {
	*this = _other;
}

GraphicsItemDesignerExportConfig::~GraphicsItemDesignerExportConfig() {

}

GraphicsItemDesignerExportConfig& GraphicsItemDesignerExportConfig::operator = (const GraphicsItemDesignerExportConfig& _other) {
	if (this == &_other) return *this;
	m_name = _other.m_name;

	return *this;
}
