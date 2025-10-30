// @otlicense

// OToolkit header
#include "GraphicsItemDesignerExportConfig.h"

GraphicsItemDesignerExportConfig::GraphicsItemDesignerExportConfig() 
	: m_exportConfigFlags(AutoAlign)
{

}

GraphicsItemDesignerExportConfig::GraphicsItemDesignerExportConfig(const GraphicsItemDesignerExportConfig& _other) {
	*this = _other;
}

GraphicsItemDesignerExportConfig::~GraphicsItemDesignerExportConfig() {

}

GraphicsItemDesignerExportConfig& GraphicsItemDesignerExportConfig::operator = (const GraphicsItemDesignerExportConfig& _other) {
	if (this == &_other) return *this;
	
	m_fileName = _other.m_fileName;
	m_exportConfigFlags = _other.m_exportConfigFlags;

	return *this;
}
