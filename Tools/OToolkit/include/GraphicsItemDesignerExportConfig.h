//! @file GraphicsItemDesignerExportConfig.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qstring.h>

class GraphicsItemDesignerExportConfig {
public:
	GraphicsItemDesignerExportConfig();
	GraphicsItemDesignerExportConfig(const GraphicsItemDesignerExportConfig& _other);
	virtual ~GraphicsItemDesignerExportConfig();

	GraphicsItemDesignerExportConfig& operator = (const GraphicsItemDesignerExportConfig& _other);

private:
	QString m_name;
	
};