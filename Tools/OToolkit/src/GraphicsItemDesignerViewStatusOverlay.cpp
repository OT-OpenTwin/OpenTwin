//! @file GraphicsItemDesignerViewStatusOverlay.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerViewStatusOverlay.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollbar.h>

GraphicsItemDesignerViewStatusOverlay::GraphicsItemDesignerViewStatusOverlay(GraphicsItemDesignerView* _view) :
	ot::OverlayWidgetBase(_view, ot::AlignBottomLeft, QMargins(0, 0, 0, _view->horizontalScrollBar()->height()))
{
	QHBoxLayout* lay = new QHBoxLayout(this);
	m_posLabel = new QLabel("Pos: 0; 0");
	m_posLabel->setAlignment(Qt::AlignLeft);
	lay->addWidget(m_posLabel);

	this->setMaximumHeight(40);
}

void GraphicsItemDesignerViewStatusOverlay::setMousePosition(const QPointF& _pt) {
	m_posLabel->setText("Pos: " + QString::number(_pt.x(), 'f', 2) + "; " + QString::number(_pt.y(), 'f', 2));
}