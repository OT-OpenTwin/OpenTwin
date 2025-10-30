// @otlicense
// File: aLabelWidget.cpp
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

// AK header
#include <akWidgets/aLabelWidget.h>

#include <qevent.h>

ak::aLabelWidget::aLabelWidget(QWidget * _parent)
	: QLabel(_parent), aWidget(otLabel)
{
}

ak::aLabelWidget::aLabelWidget(const QString & _text, QWidget * _parent)
	: QLabel(_text, _parent), aWidget(otLabel)
{}

ak::aLabelWidget::~aLabelWidget() { A_OBJECT_DESTROYING }

QWidget * ak::aLabelWidget::widget(void) { return this; }

bool ak::aLabelWidget::event(QEvent *myEvent)  
{
	switch(myEvent->type())
	{        
	case(QEvent :: MouseButtonRelease):   // Identify Mouse press Event
		Q_EMIT clicked();
		break;
	}
	return QWidget::event(myEvent);
}
