//! @file GraphicsItemDesignerScene.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

class GraphicsItemDesignerView;

class GraphicsItemDesignerScene : public ot::GraphicsScene {
	OT_DECL_NOCOPY(GraphicsItemDesignerScene)
	OT_DECL_NODEFAULT(GraphicsItemDesignerScene)
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

	void setItemSize(const QSizeF& _size);
	const QSizeF& getItemSize(void) const { return m_itemSize; };

	bool exportAsImage(const QString& _filePath);

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
	Mode m_mode;
	QSizeF m_itemSize;
};