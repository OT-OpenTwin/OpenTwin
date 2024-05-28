//! @file GraphicsItemDesignerItemBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"

GraphicsItemDesignerItemBase::GraphicsItemDesignerItemBase() {

}

GraphicsItemDesignerItemBase::~GraphicsItemDesignerItemBase() {

}

void GraphicsItemDesignerItemBase::addControlPoint(const QPointF& _pt) {
	m_controlPoints.push_back(_pt);
	this->controlPointsChanged();
}

void GraphicsItemDesignerItemBase::setControlPoints(const QList<QPointF>& _points) {
	m_controlPoints = _points;
	this->controlPointsChanged();
}