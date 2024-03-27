//! @file ColorStyleEditor.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Color.h"
#include "OTGui/PropertyGridCfg.h"
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
	class TabWidget;
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
	void slotImportConfig(void);
	void slotExportConfig(void);
	void slotReset(void);
	void slotGenerate(void);
	void slotApplyAsCurrent(void);
	void slotExport(void);

private:
	QByteArray m_sheetBase;

	QWidget* m_root;
	ot::PropertyGrid* m_propertyGrid;
	ot::TabWidget* m_editorTab;
	ot::TextEditor* m_baseEditor;
	ot::TextEditor* m_editor;
	ot::PropertyGridCfg m_propertyGridConfig;

	ot::PropertyString* m_nameProp;
	ot::PropertyGroup* m_styleValuesGroup;
	ot::PropertyGroup* m_colorsGroup;
	ot::PropertyGroup* m_fileGroup;
	std::map<std::string, ot::PropertyPainter2D*> m_styleValues;
	std::map<std::string, ot::PropertyPainter2D*> m_colors;
	std::map<std::string, ot::PropertyString*> m_files;

	void initializeStyleSheetBase(void);
	void initializeStyleValues(void);
	void parseStyleSheetBaseFile(void);
	void initializePropertyGrid(void);
	bool generateFile(std::string& _result);
};
