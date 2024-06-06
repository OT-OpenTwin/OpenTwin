//! @file GraphicsItemDesignerImageExportConfig.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerImageExportConfig.h"

GraphicsItemDesignerImageExportConfig::GraphicsItemDesignerImageExportConfig()
	: m_showGrid(false)
{}

GraphicsItemDesignerImageExportConfig::GraphicsItemDesignerImageExportConfig(const GraphicsItemDesignerImageExportConfig& _other) {
	*this = _other;
}

GraphicsItemDesignerImageExportConfig::~GraphicsItemDesignerImageExportConfig() {

}

GraphicsItemDesignerImageExportConfig& GraphicsItemDesignerImageExportConfig::operator = (const GraphicsItemDesignerImageExportConfig& _other) {
	if (this == &_other) return *this;

	m_fileName = _other.m_fileName;
	m_margins = _other.m_margins;
	m_showGrid = _other.m_showGrid;

	return *this;
}