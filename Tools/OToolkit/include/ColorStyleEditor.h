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
	class PropertyGrid; 
	class Painter2D;
}

class ColorStyleEditorEntry {
public:
	enum EntryType {
		ColorEntry,
		FileEntry
	};
	enum ColorMode {
		FillMode,
		GradientMode,
	};


public:
	ColorStyleEditorEntry();
	virtual ~ColorStyleEditorEntry();



private:
	std::string m_name;

};

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
	void slotImport(void);
	void slotExport(void);
	void slotShowPreview(void);

private:
	QSplitter* m_root;
	ot::PropertyGrid* m_propertyGrid;

	QString m_currentName;
	std::map<std::string, const ot::Painter2D*> m_styleValues;


	void initializeStyleValues(void);
	void rebuildProperties(void);

};
