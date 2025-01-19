//! @file TableView.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/Table.h"
#include "OTWidgets/TableView.h"

ot::TableView::TableView(Table* _table)
	: WidgetView(WidgetViewBase::ViewTable), m_table(_table)
{
	if (!m_table) {
		m_table = new Table;
	}

	this->addWidgetToDock(this->getViewWidget());
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
