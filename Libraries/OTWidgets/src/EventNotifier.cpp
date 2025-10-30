// @otlicense
// File: EventNotifier.cpp
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
#include "OTSystem/OTAssert.h"
#include "OTWidgets/EventNotifier.h"

// Qt header
#include <QtGui/qevent.h>

ot::EventNotifierMousePressed::EventNotifierMousePressed(QObject* _parent) 
	: QObject(_parent)
{}

bool ot::EventNotifierMousePressed::eventFilter(QObject* _obj, QEvent* _event) {
	if (_event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* e = dynamic_cast<QMouseEvent*>(_event);
		OTAssertNullptr(e);
		if (e->button() == Qt::LeftButton) {
			Q_EMIT leftMouseButtonPressed();
		}
		else if (e->button() == Qt::MiddleButton) {
			Q_EMIT middleMouseButtonPressed();
		}
		else if (e->button() == Qt::RightButton) {
			Q_EMIT rightMouseButtonPressed();
		}
	}
	return false;
}
