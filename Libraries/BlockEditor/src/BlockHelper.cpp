//! @file BlockHelper.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockHelper.h"
#include "OpenTwinCore/otAssert.h"

QRectF ot::calculateChildRect(const QRectF& _parentRect, ot::Orientation _childPosition, const QSizeF& _childSize) {
	switch (_childPosition)
	{
	case ot::OrientCenter:
		return QRectF(
			(_parentRect.topLeft().x() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			(_parentRect.topLeft().y() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::OrientTop:
		return QRectF(
			(_parentRect.left() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			_parentRect.top(),
			_childSize.width(), 
			_childSize.height()
		);
	case ot::OrientTopRight:
		return QRectF(
			_parentRect.right() - _childSize.width(),
			_parentRect.top(),
			_childSize.width(), 
			_childSize.height()
		);
	case ot::OrientRight:
		return QRectF(
			_parentRect.right() - _childSize.width(),
			(_parentRect.top() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::OrientBottomRight:
		return QRectF(
			_parentRect.right() - _childSize.width(),
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::OrientBottom:
		return QRectF(
			(_parentRect.topLeft().x() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::OrientBottomLeft:
		return QRectF(
			_parentRect.left(),
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::OrientLeft:
		return QRectF(
			_parentRect.left(),
			(_parentRect.topLeft().y() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::OrientTopLeft:
		return QRectF(
			_parentRect.left(),
			_parentRect.top(),
			_childSize.width(),
			_childSize.height()
		);
	default:
		otAssert(0, "Unknown orientation");
		return _parentRect;
	}
}