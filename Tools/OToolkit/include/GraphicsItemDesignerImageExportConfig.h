// @otlicense

//! @file GraphicsItemDesignerImageExportConfig.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qmargins.h>

class GraphicsItemDesignerImageExportConfig {
public:
	GraphicsItemDesignerImageExportConfig();
	GraphicsItemDesignerImageExportConfig(const GraphicsItemDesignerImageExportConfig& _other);
	virtual ~GraphicsItemDesignerImageExportConfig();

	GraphicsItemDesignerImageExportConfig& operator = (const GraphicsItemDesignerImageExportConfig& _other);

	void setFileName(const QString& _fileName) { m_fileName = _fileName; };
	const QString& getFileName(void) const { return m_fileName; };

	void setMargins(const QMarginsF& _margins) { m_margins = _margins; };
	const QMarginsF& getMargins(void) const { return m_margins; };

	void setShowGrid(bool _showGrid) { m_showGrid = _showGrid; };
	bool getShowGrid(void) const { return m_showGrid; };

private:
	QString m_fileName;
	QMarginsF m_margins;
	bool m_showGrid;
};