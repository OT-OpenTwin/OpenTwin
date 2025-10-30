// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/OverlayWidgetBase.h"

// Qt header
#include <QtCore/qstring.h>

class QLabel;
class GraphicsItemDesigner;

class GraphicsItemDesignerViewStatusOverlay : public ot::OverlayWidgetBase {
public:
	GraphicsItemDesignerViewStatusOverlay(GraphicsItemDesignerView* _view);

	void setMousePosition(const QPointF& _pt);

private:
	QLabel* m_posLabel;

};
