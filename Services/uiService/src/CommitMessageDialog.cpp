#include "CommitMessageDialog.h"
#include "AppBase.h"

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

// AK header
#include <akAPI/uiAPI.h>

CommitMessageDialog::CommitMessageDialog()
	: my_buttonCancel{ nullptr }, my_buttonConfirm{ nullptr }, my_cancelClose{ false }, my_confirmed{ false }, my_input{ nullptr },
	my_layout{ nullptr }, my_layoutButtons{ nullptr }, my_layoutInput{ nullptr }, my_widgetButtons{ nullptr }, my_widgetInput{ nullptr }
{
	// Create controls
	my_buttonCancel = new QPushButton{ "Cancel" };
	my_buttonConfirm = new QPushButton{ "Commit" };
	my_input = new lineEdit;
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

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonConfirm);
	my_layoutButtons->addWidget(my_buttonCancel);
	my_layout->addWidget(my_widgetButtons);

	setWindowTitle("Commit Project");
	setWindowIcon(AppBase::instance()->mainWindow()->windowIcon());

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

	delete my_layout;
}

QString CommitMessageDialog::changeMessage(void) { return my_input->text(); }

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
	emit isClosing();
	if (!my_cancelClose) { close(); }
	else { my_confirmed = false; }
}
