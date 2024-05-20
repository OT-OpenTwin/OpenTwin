//! @file GraphicsItemDesignerDrawHandler.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Qt header
#include <QtCore/qobject.h>

class GraphicsItemDesignerView;
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

	DrawMode m_mode;
	GraphicsItemDesignerView* m_view;
	GraphicsItemDesignerInfoOverlay* m_overlay;
};