// @otlicense

//! @file GraphicsItemDesignerViewStatusOverlay.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
