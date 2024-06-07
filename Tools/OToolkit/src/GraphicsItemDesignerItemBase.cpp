//! @file GraphicsItemDesignerItemBase.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "GraphicsItemDesignerItemBase.h"

GraphicsItemDesignerItemBase::GraphicsItemDesignerItemBase() 
	: m_designerItemFlags(DesignerItemFlag::NoDesignerItemFlags)
{

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

void GraphicsItemDesignerItemBase::graphicsItemWasMoved(const QPointF& _newPos) {
	if (m_designerItemFlags & DesignerItemFlag::DesignerItemIgnoreEvents) return;

	// Calculate move delta
	QPointF delta = _newPos - m_lastPos;
	
	// Check if the item was moved
	if (delta.x() == 0 && delta.y() == 0) return;

	// Move all control points
	for (QPointF& pt : m_controlPoints) {
		pt += delta;
	}

	// Set the last pos
	m_lastPos = _newPos;
}

void GraphicsItemDesignerItemBase::initializeBaseData(const QList<QPointF>& _controlPoints, const QPointF& _pos) {
	m_controlPoints = _controlPoints;
	m_lastPos = _pos;
}