// @otlicense

// OpenTwin header
#include "OTWidgets/CartesianPlotLegendView.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::CartesianPlotLegendView::CartesianPlotLegendView(QWidget* _parent) :
	QScrollArea(_parent)
{
	m_contentsWidget = new QWidget(this);
	this->setWidget(m_contentsWidget);

	m_contentsLayout = new QVBoxLayout(m_contentsWidget);
}
