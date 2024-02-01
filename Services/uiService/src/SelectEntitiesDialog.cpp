//! @file SelectEntitiesDialog.cpp
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "SelectEntitiesDialog.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PushButton.h"

// Qt header
#include <QtWidgets/qlayout.h>

SelectEntitiesDialog::SelectEntitiesDialog(const std::list<ot::NavigationTreeItem>& _availableItems, const std::list<ot::NavigationTreeItem>& _selectedItems, QWidget* _parent)
	: ot::Dialog(_parent)
{
	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout(this);
	QHBoxLayout* inpLay = new QHBoxLayout;
	QHBoxLayout* btnLay = new QHBoxLayout;

	// Create controls
	m_available = new ot::TreeWidget;
	m_selected = new ot::TreeWidget;

	ot::PushButton* btnApply = new ot::PushButton("Apply");
	ot::PushButton* btnCancel = new ot::PushButton("Cancel");

	// Setup layouts
	cLay->addLayout(inpLay, 1);
	cLay->addLayout(btnLay);

	inpLay->addWidget(m_available);
	inpLay->addWidget(m_selected);

	btnLay->addStretch(1);
	btnLay->addWidget(btnApply);
	btnLay->addWidget(btnCancel);

	// Connect signals
	this->connect(btnApply, &QPushButton::clicked, this, &SelectEntitiesDialog::slotApply);
	this->connect(btnCancel, &QPushButton::clicked, this, &SelectEntitiesDialog::slotCancel);
}

SelectEntitiesDialog::~SelectEntitiesDialog() {

}

void SelectEntitiesDialog::slotApply(void) {
	this->close(ot::Dialog::Ok);
}

void SelectEntitiesDialog::slotCancel(void) {
	this->close(ot::Dialog::Cancel);
}