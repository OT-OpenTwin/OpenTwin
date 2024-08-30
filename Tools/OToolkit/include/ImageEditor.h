//! @file ImageEditor.h
//! @author Alexander Kuester (alexk95)
//! @date April 2024
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
class QTimer;
class QWidget;
class QHBoxLayout;
class QVBoxLayout;

namespace ot {
	class SpinBox;
	class CheckBox;
	class ImagePreview;
	class ColorPickButton;
}

class ImageEditorToolBar : public QToolBar {
	Q_OBJECT
public:
	ImageEditorToolBar();
	virtual ~ImageEditorToolBar();

	void setFromColor(const QColor& _color);
	QColor fromColor(void) const;
	QColor toColor(void) const;
	int tolerance(void) const;
	bool useDifference(void) const;
	bool checkAlpha(void) const;
	bool useOriginalAlpha(void) const;

Q_SIGNALS:
	void dataChanged(void);

private Q_SLOTS:
	void slotDataChanged(void);
	void slotSpinChanged(void);

private:
	QTimer* m_spinDelayTimer;
	ot::CheckBox* m_useDiff;
	ot::CheckBox* m_checkAlpha;
	ot::CheckBox* m_useOriginalAlpha;
	ot::SpinBox* m_tolerance;
	ot::ColorPickButton* m_fromColor;
	ot::ColorPickButton* m_toColor;

};

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

class ImageEditor : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	ImageEditor();
	virtual ~ImageEditor();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString getToolName(void) const override { return QString("Image Editor"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotOriginClicked(const QPoint& _px);
	void slotImport(void);
	void slotCalculate(void);
	void slotExport(void);

private:
	QString m_currentFileName;
	ot::WidgetView* m_root;
	ImageEditorToolBar* m_toolBar;
	ot::ImagePreview* m_original;
	ot::ImagePreview* m_converted;
};
