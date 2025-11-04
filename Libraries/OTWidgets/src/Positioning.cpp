// @otlicense
// File: Positioning.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtGui/qscreen.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qapplication.h>

QRect ot::Positioning::calculateChildRect(const QRect& _parentRect, const QSize& _childSize, ot::Alignment _childAlignment) {
	// Get the top left point of the outer rectangle
	QPoint pt(_parentRect.topLeft());

	// Align inner rectangle
	switch (_childAlignment)
	{
	case Alignment::Center:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case Alignment::Top:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		break;
	case Alignment::TopRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		break;
	case Alignment::Right:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case Alignment::BottomRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case Alignment::Bottom:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case Alignment::BottomLeft:
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case Alignment::Left:
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2));
		break;
	case Alignment::TopLeft:
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
	case Alignment::Center:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case Alignment::Top:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		break;
	case Alignment::TopRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		break;
	case Alignment::Right:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case Alignment::BottomRight:
		pt.setX(pt.x() + (_parentRect.width() - _childSize.width()));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case Alignment::Bottom:
		pt.setX(pt.x() + ((_parentRect.width() - _childSize.width()) / 2.));
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case Alignment::BottomLeft:
		pt.setY(pt.y() + (_parentRect.height() - _childSize.height()));
		break;
	case Alignment::Left:
		pt.setY(pt.y() + ((_parentRect.height() - _childSize.height()) / 2.));
		break;
	case Alignment::TopLeft:
		break;
	default:
		OT_LOG_EA("Unknown Alignment");
		break;
	}

	return QRectF(pt, _childSize);
}

QRect ot::Positioning::fitOnScreen(const QRect& _sourceRect, FitMode _mode) {
	// Check if the rect is already in a screen
	for (QScreen* s : QGuiApplication::screens()) {
		if (s->availableGeometry().contains(_sourceRect)) {
			return _sourceRect;
		}
	}

	// Get the screen closest to the initial position of the rect
	QScreen* screen = nullptr;

	// Determine the reference point based on the fit mode
	QPoint referencePoint;
	switch (_mode) {
	case ot::Positioning::FitByTopLeft:
		referencePoint = _sourceRect.topLeft();
		break;
	case ot::Positioning::FitByCenter:
		referencePoint = _sourceRect.center();
		break;
	default:
		OT_LOG_EAS("Unknown FitMode (" + std::to_string(static_cast<int>(_mode)) + ")");
		referencePoint = _sourceRect.topLeft();
		break;
	}

	double closestDistance = std::numeric_limits<double>::max();
	for (QScreen* s : QGuiApplication::screens()) {
		double distanceLeft = referencePoint.x() - s->availableGeometry().left();
		double distanceRight = referencePoint.x() - s->availableGeometry().right();
		double distanceTop = referencePoint.y() - s->availableGeometry().top();
		double distanceBottom = referencePoint.y() - s->availableGeometry().bottom();

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

bool ot::Positioning::centerWidgetOnParent(const QWidget* _parentWidget, QWidget* _childWidget) {
	QRect childRect;
	if (_childWidget->layout()) {
		_childWidget->layout()->activate();
		childRect = QRect(QPoint(0, 0), _childWidget->layout()->totalSizeHint());
	}
	else {
		childRect = _childWidget->rect();
	}

	QRect newRect = Positioning::getCenterWidgetOnParentRect(_parentWidget, childRect);
	if (newRect.isValid()) {
		_childWidget->move(newRect.topLeft());
		return true;
	}
	else {
		return false;
	}
}

QRect ot::Positioning::getCenterWidgetOnParentRect(const QWidget* _parentWidget, const QRect& _childRect) {
	if (_parentWidget) {
		return Positioning::calculateChildRect(_parentWidget->rect(), _childRect.size(), Alignment::Center);
	}
	else {
		QScreen* screen = QApplication::primaryScreen();
		if (screen) {
			return Positioning::calculateChildRect(screen->geometry(), _childRect.size(), Alignment::Center);
		}
		else {
			OT_LOG_EA("Primary screen not found");
			return _childRect;
		}
	}
}
