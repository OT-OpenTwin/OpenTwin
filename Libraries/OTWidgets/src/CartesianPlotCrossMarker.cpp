// @otlicense

//! @file CartesianPlotCrossMarker.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/CartesianPlotCrossMarker.h"

// Qwt header
#include <qwt_symbol.h>

ot::CartesianPlotCrossMarker::CartesianPlotCrossMarker() : m_symbol(nullptr) {
	this->setLineStyle(QwtPlotMarker::LineStyle::Cross);
	this->setLabelAlignment(Qt::AlignRight | Qt::AlignBottom);
	this->setLinePen(QPen(QColor(200, 150, 0), 0, Qt::DashDotLine));
	m_symbol = new QwtSymbol(QwtSymbol::Diamond, QColor(Qt::yellow), QColor(Qt::green), QSize(7, 7));
	this->setSymbol(m_symbol);
}

ot::CartesianPlotCrossMarker::~CartesianPlotCrossMarker() {
	delete m_symbol;
}

void ot::CartesianPlotCrossMarker::setStyle(const QColor& _innerColor, const QColor& _outerColor, int _size, double _outerColorSize) {
	m_symbol->setBrush(_innerColor);
	m_symbol->setPen(_outerColor, _outerColorSize);
	m_symbol->setSize(_size);
}
