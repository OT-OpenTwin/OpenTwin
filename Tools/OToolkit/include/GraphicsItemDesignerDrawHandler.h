//! @file GraphicsItemDesignerDrawHandler.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Qt header
#include <QtCore/qobject.h>

class GraphicsItemDesignerView;
class GraphicsItemDesignerItemBase;
class GraphicsItemDesignerInfoOverlay;

class GraphicsItemDesignerDrawHandler : public QObject {
	Q_OBJECT
public:
	enum DrawMode {
		NoMode,
		Line,
		Square,
		Rect,
		Circle,
		Ellipse,
		Triangle,
		Polygon,
		Shape
	};

	GraphicsItemDesignerDrawHandler(GraphicsItemDesignerView* _view);
	virtual ~GraphicsItemDesignerDrawHandler();

	void startDraw(DrawMode _mode);
	void cancelDraw(void);

	//! \brief Stops the current draw and returns the drawn item if valid.
	//! The current preview item will be returned and forgotten by the handler.
	//! The caller takes ownership of the item.
	//! The returned item is still placed in the scene.
	GraphicsItemDesignerItemBase* stopDraw(void);

	void updatePosition(const QPointF& _pos);
	void positionSelected(const QPointF& _pos);

Q_SIGNALS:
	void drawCompleted();
	void drawCancelled();

private:
	QPointF constainPosition(const QPointF& _pos) const;
	QString modeString(void);
	void createPreviewItem(void);
	GraphicsItemDesignerItemBase* createLineItem(void);
	GraphicsItemDesignerItemBase* createSquareItem(void);
	GraphicsItemDesignerItemBase* createRectItem(void);

	ot::UID m_currentUid;

	DrawMode m_mode;
	GraphicsItemDesignerView* m_view;
	GraphicsItemDesignerInfoOverlay* m_overlay;
	GraphicsItemDesignerItemBase* m_previewItem;
};