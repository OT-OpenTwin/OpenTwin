// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"

// Qt header
#include <QtCore/qstring.h>

class GraphicsItemDesignerExportConfig {
public:
	enum ExportConfigFlag {
		NoFlags = 0x00,
		AutoAlign = 0x01
	};
	typedef ot::Flags<ExportConfigFlag> ExportConfigFlags;

	GraphicsItemDesignerExportConfig();
	GraphicsItemDesignerExportConfig(const GraphicsItemDesignerExportConfig& _other);
	virtual ~GraphicsItemDesignerExportConfig();

	GraphicsItemDesignerExportConfig& operator = (const GraphicsItemDesignerExportConfig& _other);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setExportConfigFlag(ExportConfigFlag _flag, bool _active = true) { m_exportConfigFlags.setFlag(_flag, _active); };
	void setExportConfigFlags(const ExportConfigFlags& _flags) { m_exportConfigFlags = _flags; };
	const ExportConfigFlags& getExportConfigFlags(void) const { return m_exportConfigFlags; };

	void setFileName(const QString& _fileName) { m_fileName = _fileName; };
	const QString& getFileName(void) const { return m_fileName; };

private:
	QString m_fileName;
	ExportConfigFlags m_exportConfigFlags;
};

OT_ADD_FLAG_FUNCTIONS(GraphicsItemDesignerExportConfig::ExportConfigFlag)