//! @file GraphicsItemDesigner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// ToolkitAPI header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;
class GraphicsItemDesigner;
namespace ot { class PropertyGrid; };
namespace ot { class GraphicsView; };

class GraphicsItemDesignerToolBar : public QObject, public ot::QWidgetInterface {
	Q_OBJECT
public:
	enum DesignerMode {
		NoMode,
		LineMode,
		SquareMode,
		RectMode,
		TriangleMode,
		PolygonMode,
		ShapeMode
	};

	GraphicsItemDesignerToolBar(GraphicsItemDesigner* _designer);
	virtual ~GraphicsItemDesignerToolBar();

	virtual QWidget* getQWidget(void) override { return m_widget; };

	DesignerMode getMode(void) const { return m_mode; };

Q_SIGNALS:
	void modeChanged(void);

private Q_SLOTS:
	void slotLine(void);
	void slotSquare(void);
	void slotRect(void);
	void slotTriangle(void);
	void slotPolygon(void);
	void slotShape(void);

private:
	DesignerMode m_mode;
	QWidget* m_widget;
	GraphicsItemDesigner* m_designer;

};

class GraphicsItemDesigner : public QObject, public otoolkit::Tool {
public:
	GraphicsItemDesigner();
	virtual ~GraphicsItemDesigner() {};

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Graphics Item Designer"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private:
	ot::GraphicsView* m_view;
	ot::PropertyGrid* m_props;
	GraphicsItemDesignerToolBar* m_toolBar;
};
