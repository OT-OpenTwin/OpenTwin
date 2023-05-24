//! @file BlockHelper.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockHelper.h"

QRectF ot::calculateChildRect(const QRectF& _parentRect, ot::BlockComponentPosition _childPosition, const QSizeF& _childSize) {
	QPointF TL;

	if (_childPosition == ot::ComponentCenter) {
		QPointF c;
		c.setX(_parentRect.topLeft().x() + (_parentRect.width() / 2.));
		c.setY(_parentRect.topLeft().y() + (_parentRect.height() / 2.));

		TL.setX(c.x() - _childSize.width() / 2.);
		TL.setY(c.y() - _childSize.height() / 2.);

		return QRectF(TL, _childSize);
	}


}