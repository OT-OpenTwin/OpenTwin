//! @file GraphicsItemDesignerScene.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

class GraphicsItemDesignerView;
namespace ot { class GraphicsEllipseItem; }

class GraphicsItemDesignerScene : public ot::GraphicsScene {
public:
	enum Mode {
		NoMode,
		PointPickingMode
	};

	GraphicsItemDesignerScene(GraphicsItemDesignerView* _view);
	virtual ~GraphicsItemDesignerScene();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void enablePickingMode(void);
	void disablePickingMode(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected: Event handler
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
	
private:
	GraphicsItemDesignerView* m_view;
	ot::GraphicsEllipseItem* m_cursorItem;
	Mode m_mode;
};