//! @file BlockHelper.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockHelper.h"
#include "OpenTwinCore/otAssert.h"

QRect ot::calculateChildRect(const QRect& _parentRect, const QSize& _childSize, ot::Alignment _childAlignment) {
	switch (_childAlignment)
	{
	case ot::AlignCenter:
		return QRect(
			(_parentRect.topLeft().x() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			(_parentRect.topLeft().y() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignTop:
		return QRect(
			(_parentRect.left() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			_parentRect.top(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignTopRight:
		return QRect(
			_parentRect.right() - _childSize.width(),
			_parentRect.top(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignRight:
		return QRect(
			_parentRect.right() - _childSize.width(),
			(_parentRect.top() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignBottomRight:
		return QRect(
			_parentRect.right() - _childSize.width(),
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignBottom:
		return QRect(
			(_parentRect.topLeft().x() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignBottomLeft:
		return QRect(
			_parentRect.left(),
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignLeft:
		return QRect(
			_parentRect.left(),
			(_parentRect.topLeft().y() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignTopLeft:
		return QRect(
			_parentRect.left(),
			_parentRect.top(),
			_childSize.width(),
			_childSize.height()
		);
	default:
		otAssert(0, "Unknown alignment");
		return _parentRect;
	}
}

QRectF ot::calculateChildRect(const QRectF& _parentRect, const QSizeF& _childSize, ot::Alignment _childAlignment) {
	switch (_childAlignment)
	{
	case ot::AlignCenter:
		return QRectF(
			(_parentRect.topLeft().x() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			(_parentRect.topLeft().y() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignTop:
		return QRectF(
			(_parentRect.left() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			_parentRect.top(),
			_childSize.width(), 
			_childSize.height()
		);
	case ot::AlignTopRight:
		return QRectF(
			_parentRect.right() - _childSize.width(),
			_parentRect.top(),
			_childSize.width(), 
			_childSize.height()
		);
	case ot::AlignRight:
		return QRectF(
			_parentRect.right() - _childSize.width(),
			(_parentRect.top() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignBottomRight:
		return QRectF(
			_parentRect.right() - _childSize.width(),
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignBottom:
		return QRectF(
			(_parentRect.topLeft().x() + (_parentRect.width() / 2.)) - _childSize.width() / 2.,
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignBottomLeft:
		return QRectF(
			_parentRect.left(),
			_parentRect.bottom() - _childSize.height(),
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignLeft:
		return QRectF(
			_parentRect.left(),
			(_parentRect.topLeft().y() + (_parentRect.height() / 2.)) - _childSize.height() / 2.,
			_childSize.width(),
			_childSize.height()
		);
	case ot::AlignTopLeft:
		return QRectF(
			_parentRect.left(),
			_parentRect.top(),
			_childSize.width(),
			_childSize.height()
		);
	default:
		otAssert(0, "Unknown alignment");
		return _parentRect;
	}
}