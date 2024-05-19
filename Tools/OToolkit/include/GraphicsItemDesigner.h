//! @file GraphicsItemDesigner.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// ToolkitAPI header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;
class GraphicsItemDesignerView;
class GraphicsItemDesignerToolBar;
class GraphicsItemDesignerInfoOverlay;
namespace ot { class PropertyGrid; };

class GraphicsItemDesigner : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	enum DesignerMode {
		NoMode,
		LineStartMode,
		LineEndMode,
		SquareStartMode,
		SquareEndMode,
		RectStartMode,
		RectEndMode,
		TriangleStartMode,
		TriangleEndMode,
		PolygonStartMode,
		PolygonStepMode,
		ShapeStartMode,
		ShapeStepMode
	};

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

	// Setter / Getter

	GraphicsItemDesignerView* getView(void) const { return m_view; };
	ot::PropertyGrid* getPropertyGrid(void) const { return m_props; };
	GraphicsItemDesignerToolBar* getToolBar(void) const { return m_toolBar; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotModeRequested(DesignerMode _mode);
	void slotPointSelected(const QPointF& _pt);
	void cancelModeRequested(void);

private:
	DesignerMode m_mode;
	GraphicsItemDesignerView* m_view;
	ot::PropertyGrid* m_props;
	GraphicsItemDesignerToolBar* m_toolBar;
	GraphicsItemDesignerInfoOverlay* m_overlay;
};
