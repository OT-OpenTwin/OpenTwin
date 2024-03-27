//! @file ColorStyleEditor.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Color.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <map>

class QLabel;
class QWidget;
class QSplitter;
class QHBoxLayout;
class QVBoxLayout;

namespace ot {
	class LineEdit;
	class Painter2D;
	class TextEditor;
	class PropertyGrid; 
	class PropertyString;
	class PropertyPainter2D;
}

class ColorStyleEditor : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	ColorStyleEditor();
	virtual ~ColorStyleEditor();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Color Style Editor"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual QWidget* runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private slots:
	void slotReset(void);
	void slotGenerate(void);
	void slotExport(void);

private:
	QSplitter* m_root;
	ot::PropertyGrid* m_propertyGrid;
	ot::TextEditor* m_editor;

	QString m_currentName;
	std::map<std::string, ot::PropertyPainter2D*> m_styleValues;
	std::map<std::string, ot::PropertyPainter2D*> m_colors;
	std::map<std::string, ot::PropertyString*> m_files;

	void initializeStyleValues(void);
	void initializePropertyGrid(void);

};
