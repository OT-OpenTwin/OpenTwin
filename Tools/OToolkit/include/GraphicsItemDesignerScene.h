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

	void setItemSize(const QSizeF& _size) { m_itemSize = _size; };
	const QSizeF& getItemSize(void) const { return m_itemSize; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Protected: Event handler
protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;
	virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

private:
	void constrainItemToScene(QGraphicsItem* _item);

	GraphicsItemDesignerView* m_view;
	ot::GraphicsEllipseItem* m_cursorItem;
	Mode m_mode;
	QSizeF m_itemSize;
};