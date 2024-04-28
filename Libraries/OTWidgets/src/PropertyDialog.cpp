//! @file PropertyDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/PropertyGroup.h"
#include "OTWidgets/Splitter.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PropertyGrid.h"
#include "OTWidgets/PropertyDialog.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/PropertyGridGroup.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qpushbutton.h>

// ###########################################################################################################################################################################################################################################################################################################################

// Property dialog tree

namespace ot {

		class PropertyDialog::PropertyDialogNavigation : public PropertyGrid {
		public:

		private:

		}; // class PropertyDialogNavigation;

} // namespace ot

// ###########################################################################################################################################################################################################################################################################################################################

ot::PropertyDialog::PropertyDialog(const PropertyDialogCfg& _config, QWidget* _parentWidget)
	: Dialog(_config, _parentWidget), m_changed(false)
{
	// Create layouts
	QVBoxLayout* cLay = new QVBoxLayout(this);
	QHBoxLayout* btnLay = new QHBoxLayout;

	// Create controls
	Splitter* cSplitter = new Splitter;
	m_grid = new PropertyGrid;
	m_navigation = new PropertyDialogNavigation;

	QPushButton* btnConfirm = new QPushButton("Confirm");
	QPushButton* btnCancel = new QPushButton("Cancel");

	// Setup layouts
	cLay->addWidget(cSplitter, 1);
	cLay->addLayout(btnLay);

	cSplitter->addWidget(m_navigation->getQWidget());
	cSplitter->addWidget(m_grid->getQWidget());
	btnLay->addStretch(1);
	btnLay->addWidget(btnConfirm);
	btnLay->addWidget(btnCancel);

	// Setup data
	
	m_grid->setupGridFromConfig(_config.gridConfig());
	m_navigation->setupGridFromConfig(_config.gridConfig());
	this->iniData();

	// Connect signals
	this->connect(btnConfirm, &QPushButton::clicked, this, &PropertyDialog::slotConfirm);
	this->connect(btnCancel, &QPushButton::clicked, this, &PropertyDialog::slotCancel);
}

ot::PropertyDialog::~PropertyDialog() {

}

void ot::PropertyDialog::slotConfirm(void) {
	if ((this->dialogFlags() & DialogCfg::CancelOnNoChange) && !m_changed) {
		this->close(Dialog::Cancel);
	}
	else {
		this->close(Dialog::Ok);
	}
}

void ot::PropertyDialog::slotCancel(void) {
	this->close(Dialog::Cancel);
}

void ot::PropertyDialog::iniData(void) {
	m_navigation->getTreeWidget()->setColumnCount(1);
}