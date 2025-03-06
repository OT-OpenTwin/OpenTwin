//! @file GraphicsItemDesignerInfoOverlay.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesigner.h"
#include "GraphicsItemDesignerView.h"
#include "GraphicsItemDesignerInfoOverlay.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qscrollbar.h>

GraphicsItemDesignerInfoOverlay::GraphicsItemDesignerInfoOverlay(const QString& _text, GraphicsItemDesignerView* _view, const QMargins& _overlayMargins)
	: ot::OverlayWidgetBase(_view, ot::AlignTop, _overlayMargins)
{
	QHBoxLayout* lay = new QHBoxLayout(this);
	m_label = new QLabel(_text);
	m_label->setAlignment(Qt::AlignCenter);
	lay->addWidget(m_label);

	this->setMaximumHeight(40);
}

void GraphicsItemDesignerInfoOverlay::setText(const QString& _text) {
	m_label->setText(_text);
}