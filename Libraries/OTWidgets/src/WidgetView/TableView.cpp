// @otlicense
// File: TableView.cpp
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
#include "OTWidgets/WidgetView/TableView.h"
#include "OTWidgets/WidgetView/WidgetViewDock.h"

// Qt header
#include <QtWidgets/qlayout.h>

ot::TableView::TableView(QWidget* _parent) : TableView(nullptr, _parent) {}

ot::TableView::TableView(Table* _table, QWidget* _parent)
	: WidgetView(WidgetViewBase::ViewTable, _parent), m_table(_table)
{
	// Create a custom root widget to wrap the table, this fixes a problem with the
	// focus handling of the table view when it is in a tabbed dock widget
	QWidget* rootWidget = getViewDockWidget();
	QWidget* layoutW = new QWidget(rootWidget);
	QLayout* layout = new QVBoxLayout(layoutW);
	layout->setContentsMargins(0, 0, 0, 0);
	
	if (!m_table) {
		m_table = new Table(layoutW);
	}
	else {
		m_table->setParent(layoutW);
	}
	layout->addWidget(m_table);
		
	this->addWidgetToDock(layoutW);
	this->connect(m_table, &Table::modifiedChanged, this, &TableView::slotModifiedChanged);
}

ot::TableView::~TableView() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Base class functions

QWidget* ot::TableView::getViewWidget(void) {
	return m_table;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void ot::TableView::slotModifiedChanged(bool _isModified) {
	this->setViewContentModified(_isModified);
}
