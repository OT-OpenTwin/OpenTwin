//! \file Positioning.cpp
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtGui/qscreen.h>
#include <QtWidgets/qapplication.h>

QRect ot::Positioning::calculateChildRect(const QRect& _parentRect, const QSize& _childSize, ot::Alignment _childAlignment) {
	// Get the top left point of the outer rectangle
	QPoint pt(_parentRect.topLeft());

	// Align inner rectangle
	switch (_childAlignment)
	{
	case ot::AlignCenter:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case ot::AlignTop:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		break;
	case ot::AlignTopRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		break;
	case ot::AlignRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case ot::AlignBottomRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottom:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottomLeft:
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignLeft:
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case ot::AlignTopLeft:
		break;
	default:
		OT_LOG_EA("Unknown Alignment");
		break;
	}

	return QRect(pt, _childSize);
}

QRectF ot::Positioning::calculateChildRect(const QRectF& _parentRect, const QSizeF& _childSize, ot::Alignment _childAlignment) {
	// Get the top left point of the outer rectangle
	QPointF pt(_parentRect.topLeft());

	// Align inner rectangle
	switch (_childAlignment)
	{
	case ot::AlignCenter:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case ot::AlignTop:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		break;
	case ot::AlignTopRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		break;
	case ot::AlignRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case ot::AlignBottomRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottom:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignBottomLeft:
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case ot::AlignLeft:
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case ot::AlignTopLeft:
		break;
	default:
		OT_LOG_EA("Unknown Alignment");
		break;
	}

	return QRectF(pt, _childSize);
}

QRect ot::Positioning::fitOnScreen(const QRect& _sourceRect, bool _primaryScreenOnly) {
	// Check if the rect is already in a screen
	if (_primaryScreenOnly) {
		if (QGuiApplication::primaryScreen()->availableGeometry().contains(_sourceRect)) {
			return _sourceRect;
		}
	}
	else {	
		for (QScreen* s : QGuiApplication::screens()) {
			if (s->availableGeometry().contains(_sourceRect)) {
				return _sourceRect;
			}
		}
	}

	// Get the screen closest to the initial position of the rect
	QScreen* screen = nullptr;

	if (_primaryScreenOnly) {
		screen = QGuiApplication::primaryScreen();
	}
	else {
		double closestDistance = std::numeric_limits<double>::max();
		for (QScreen* s : QGuiApplication::screens()) {
			double distanceLeft = _sourceRect.left() - s->availableGeometry().left();
			double distanceRight = _sourceRect.right() - s->availableGeometry().right();
			double distanceTop = _sourceRect.top() - s->availableGeometry().top();
			double distanceBottom = _sourceRect.bottom() - s->availableGeometry().bottom();

			if (distanceLeft < 0.) distanceLeft *= (-1);
			if (distanceRight < 0.) distanceRight *= (-1);
			if (distanceTop < 0.) distanceTop *= (-1);
			if (distanceBottom < 0.) distanceBottom *= (-1);

			double distance = std::min({ distanceLeft, distanceRight, distanceTop, distanceBottom });
			if (distance < closestDistance) {
				closestDistance = distance;
				screen = s;
			}
		}
	}

	if (!screen) {
		OT_LOG_E("No target screen found");
		return _sourceRect;
	}

	QRect screenGeometry = screen->availableGeometry();

	// Adjust the rectangle to fit within the screen geometry
	QRect fittedRect = _sourceRect;

	if (fittedRect.width() > screenGeometry.width() || fittedRect.height() > screenGeometry.height()) {
		// Fitted rect is bigger than screen
		return fittedRect;
	}

	// Ensure the rectangle fits within the screen horizontally
	if (fittedRect.left() < screenGeometry.left()) {
		fittedRect.moveLeft(screenGeometry.left());
	}
	if (fittedRect.right() > screenGeometry.right()) {
		fittedRect.moveRight(screenGeometry.right());
	}

	// Ensure the rectangle fits within the screen vertically
	if (fittedRect.top() < screenGeometry.top()) {
		fittedRect.moveTop(screenGeometry.top());
	}
	if (fittedRect.bottom() > screenGeometry.bottom()) {
		fittedRect.moveBottom(screenGeometry.bottom());
	}
	return fittedRect;
}

bool ot::Positioning::centerWidgetOnParent(const QWidgetInterface* _parentWidget, QWidgetInterface* _childWidget) {
	OTAssertNullptr(_childWidget);
	const QWidget* parentWidget = nullptr;
	if (_parentWidget) {
		parentWidget = _parentWidget->getQWidget();
	}
	QWidget* childWidget = nullptr;
	if (_childWidget) {
		childWidget = _childWidget->getQWidget();
	}
	return Positioning::centerWidgetOnParent(parentWidget, childWidget);
}

bool ot::Positioning::centerWidgetOnParent(const QWidget* _parentWidget, QWidget* _childWidget) {
	QRect newRect = Positioning::getCenterWidgetOnParentRect(_parentWidget, _childWidget);
	if (newRect.isValid()) {
		_childWidget->move(newRect.topLeft());
		return true;
	}
	else {
		return false;
	}
}

QRect ot::Positioning::getCenterWidgetOnParentRect(const QWidgetInterface* _parentWidget, QWidgetInterface* _childWidget) {
	OTAssertNullptr(_childWidget);
	const QWidget* parentWidget = nullptr;
	if (_parentWidget) {
		parentWidget = _parentWidget->getQWidget();
	}
	QWidget* childWidget = nullptr;
	if (_childWidget) {
		childWidget = _childWidget->getQWidget();
	}
	return Positioning::getCenterWidgetOnParentRect(parentWidget, childWidget);
}

QRect ot::Positioning::getCenterWidgetOnParentRect(const QWidget* _parentWidget, QWidget* _childWidget) {
	if (!_childWidget) {
		OT_LOG_EA("No child rect provided");
		return QRect();
	}
	if (_parentWidget) {
		return Positioning::calculateChildRect(_parentWidget->rect(), _childWidget->size(), ot::AlignCenter);
	}
	else {
		QScreen* screen = QApplication::primaryScreen();
		if (screen) {
			return Positioning::calculateChildRect(screen->geometry(), _childWidget->size(), ot::AlignCenter);
		}
		else {
			OT_LOG_EA("Primary screen not found");
			return QRect();
		}		
	}
}
