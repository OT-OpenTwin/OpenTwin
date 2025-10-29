// @otlicense

#include "LTSpiceConnector/CommitMessageDialog.h"
#include "OTWidgets/IconManager.h"

#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qtablewidget.h>
#include <qtablewidget.h>
#include <qpushbutton.h>
#include <qlistwidget.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qstringlist.h>
#include <qheaderview.h>
#include <qevent.h>

CommitMessageDialog::CommitMessageDialog(QString type, QString message)
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{
	// Create controls
	my_buttonCancel = new QPushButton{ "Cancel" };
	my_buttonConfirm = new QPushButton{ type };
	my_input = new QLineEdit(message);
	my_label = new QLabel("Change message");
	my_label->setBuddy(my_input);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	// Create input layout
	my_widgetInput = new QWidget;
	my_layoutInput = new QHBoxLayout{ my_widgetInput };
	my_layoutInput->addWidget(my_label);
	my_layoutInput->addWidget(my_input);
	my_layout->addWidget(my_widgetInput);

	// Create checkboxes
	my_includeResults = new QCheckBox("Include results");

	my_includeResults->setChecked(true);

	my_layout->addWidget(my_includeResults);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonConfirm);
	my_layoutButtons->addWidget(my_buttonCancel);
	my_layout->addWidget(my_widgetButtons);

	setWindowTitle(type + " Project");
	setWindowIcon(ot::IconManager::getApplicationIcon());

	my_input->setMinimumWidth(300);

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	connect(my_input, SIGNAL(returnPressed()), this, SLOT(slotReturnPressed()));
	connect(my_buttonCancel, SIGNAL(clicked()), this, SLOT(slotButtonCancelPressed()));
	connect(my_buttonConfirm, SIGNAL(clicked()), this, SLOT(slotButtonConfirmPressed()));
}

CommitMessageDialog::~CommitMessageDialog() {
	delete my_buttonCancel;
	delete my_buttonConfirm;
	delete my_input;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutInput;
	delete my_widgetInput;
	
	delete my_includeResults;

	delete my_layout;
}

QString CommitMessageDialog::changeMessage(void) { return my_input->text(); }
bool CommitMessageDialog::includeResults(void) { return my_includeResults->isChecked(); }

void CommitMessageDialog::reset(const QString& _projectToCopy) {
	my_input->setText(_projectToCopy);
	my_confirmed = false;
	my_projectToCopy = _projectToCopy;
}

void CommitMessageDialog::slotButtonConfirmPressed() { my_confirmed = true; Close(); }

void CommitMessageDialog::slotButtonCancelPressed() { my_confirmed = false;  close(); }

void CommitMessageDialog::slotReturnPressed() { my_confirmed = true; Close(); }

void CommitMessageDialog::Close(void) {
	if (my_input->text().length() == 0) { return; }

	my_cancelClose = false;
	Q_EMIT isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}
