// @otlicense

#pragma once

// OToolkit header
#include "GraphicsItemDesignerImageExportConfig.h"

// OpenTwin header
#include "OTWidgets/GraphicsScene.h"

// Qt header
#include <QtGui/qimage.h>

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

	QImage exportAsImage(const GraphicsItemDesignerImageExportConfig& _exportConfig);

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