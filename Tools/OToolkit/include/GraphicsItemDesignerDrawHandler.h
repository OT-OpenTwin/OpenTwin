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

	void updatePosition(const QPointF& _pos);
	void positionSelected(const QPointF& _pos);

Q_SIGNALS:
	void drawCompleted();
	void drawCancelled();

private:
	QString modeString(void);
	void createPreviewItem(void);
	GraphicsItemDesignerItemBase* createLineItem(void);

	ot::UID m_currentUid;

	DrawMode m_mode;
	GraphicsItemDesignerView* m_view;
	GraphicsItemDesignerInfoOverlay* m_overlay;
	GraphicsItemDesignerItemBase* m_previewItem;
};