// @otlicense
// File: VerticalScrollArea.cpp
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
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Widgets/VerticalScrollArea.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qscrollbar.h>

ot::VerticalScrollArea::VerticalScrollArea(QWidget* _parent) : ScrollArea(_parent)
{
	this->setWidgetResizable(true);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

bool ot::VerticalScrollArea::eventFilter(QObject* _watched, QEvent* _event)
{
	if (_watched && _watched == widget() && _event->type() == QEvent::Resize)
	{
		OTAssertNullptr(verticalScrollBar());
		setMinimumWidth(widget()->minimumSizeHint().width() + verticalScrollBar()->sizeHint().width());
	}

	return ScrollArea::eventFilter(_watched, _event);
}
