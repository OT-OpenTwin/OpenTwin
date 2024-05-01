//! @file ColorStyleEditor.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OTCore/Color.h"
#include "OTCore/PropertyGridCfg.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <map>

class QLabel;
class QWidget;
class QHBoxLayout;
class QVBoxLayout;

namespace ot {
	class LineEdit;
	class Painter2D;
	class TabWidget;
	class TextEditor;
	class PropertyInt;
	class PropertyGrid;
	class PropertyDouble;
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
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotBright(void);
	void slotDark(void);
	void slotBlue(void);
	void slotGenerate(void);
	void slotApplyAsCurrent(void);
	void slotGenerateAndApply(void);
	void slotExport(void);
	void slotImportBase(void);
	void slotBaseChanged(void);
	bool slotExportBase(void);

private:
	QString m_lastBaseFile;
	ot::WidgetView* m_root;
	ot::PropertyGrid* m_propertyGrid;
	ot::TabWidget* m_editorTab;
	ot::TextEditor* m_baseEditor;
	ot::TextEditor* m_editor;
	ot::PropertyGridCfg m_propertyGridConfig;

	ot::PropertyString* m_nameProp;
	ot::PropertyGroup* m_styleFilesGroup;
	ot::PropertyGroup* m_styleIntGroup;
	ot::PropertyGroup* m_styleDoubleGroup;
	ot::PropertyGroup* m_styleValuesGroup;
	ot::PropertyGroup* m_intGroup;
	ot::PropertyGroup* m_doubleGroup;
	ot::PropertyGroup* m_colorsGroup;
	ot::PropertyGroup* m_fileGroup;
	std::map<std::string, ot::PropertyString*> m_styleFiles;
	std::map<std::string, ot::PropertyInt*> m_styleInts;
	std::map<std::string, ot::PropertyDouble*> m_styleDoubles;
	std::map<std::string, ot::PropertyPainter2D*> m_styleValues;
	std::map<std::string, ot::PropertyPainter2D*> m_colors;
	std::map<std::string, ot::PropertyString*> m_files;
	std::map<std::string, ot::PropertyInt*> m_integer;
	std::map<std::string, ot::PropertyDouble*> m_double;

	void selectStyleSheetBase(void);
	void initializeStyleSheetBase(void);
	void cleanUpData(void);
	void initializeBrightStyleValues(void);
	void initializeDarkStyleValues(void);
	void initializeBlueStyleValues(void);
	void parseStyleSheetBaseFile(void);
	void initializePropertyGrid(void);
	bool generateFile(std::string& _result);
};
