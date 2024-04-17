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
#include <QtWidgets/qtoolbar.h>

// std header
#include <map>

class QLabel;
class QWidget;
class QHBoxLayout;
class QVBoxLayout;

namespace ot {
	class ImagePreview;
}

class ImageEditorToolBar : public QToolBar {

};

class ImageEditor : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	ImageEditor();
	virtual ~ImageEditor();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Image Editor"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	virtual void toolWasShown(void) override;

	virtual void toolWasHidden(void) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotImport(void);
	void slotCalculate(void);
	void slotExport(void);

private:
	QWidget* m_root;
	ImageEditorToolBar* m_toolBar;
	ot::ImagePreview* m_original;
	ot::ImagePreview* m_converted;
};
