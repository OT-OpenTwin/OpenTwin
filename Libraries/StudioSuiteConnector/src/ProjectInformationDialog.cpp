#include "StudioSuiteConnector/ProjectInformationDialog.h"
//#include "AppBase.h"

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
#include <akGui/aColorStyle.h>

ProjectInformationDialog::ProjectInformationDialog(const QIcon &windowIcon, const std::string& localFileName, const std::string& serverVersion, const std::string& localVersion)
{
	ak::aColorStyle* _colorStyle = ak::uiAPI::getCurrentColorStyle();

	// Create controls
	my_buttonClose = new QPushButton{ "Close" };

	my_fileName = new QLineEdit;
	my_serverVersion = new QLineEdit;
	my_localVersion = new QLineEdit;

	my_lFileName = new QLabel("Local File Name:");
	my_lServerVersion = new QLabel("Server Version:");
	my_lLocalVersion = new QLabel("Local Version:");

	my_lFileName->setBuddy(my_fileName);
	my_lServerVersion->setBuddy(my_serverVersion);
	my_lLocalVersion->setBuddy(my_localVersion);

	// Create main layout
	my_layout = new QVBoxLayout{ this };

	// Create input layout
	my_widgetFileName = new QWidget;
	my_layoutFileName = new QHBoxLayout{ my_widgetFileName };
	my_layoutFileName->addWidget(my_lFileName);
	my_layoutFileName->addWidget(my_fileName);

	my_widgetServerVersion = new QWidget;
	my_layoutServerVersion = new QHBoxLayout{ my_widgetServerVersion };
	my_layoutServerVersion->addWidget(my_lServerVersion);
	my_layoutServerVersion->addWidget(my_serverVersion);

	my_widgetLocalVersion = new QWidget;
	my_layoutLocalVersion = new QHBoxLayout{ my_widgetLocalVersion };
	my_layoutLocalVersion->addWidget(my_lLocalVersion);
	my_layoutLocalVersion->addWidget(my_localVersion);

	my_layout->addWidget(my_widgetFileName);
	my_layout->addWidget(my_widgetServerVersion);
	my_layout->addWidget(my_widgetLocalVersion);

	// Create button layout
	my_widgetButtons = new QWidget;
	my_layoutButtons = new QHBoxLayout{ my_widgetButtons };
	my_layoutButtons->addWidget(my_buttonClose);
	my_layout->addWidget(my_widgetButtons);

	my_fileName->setText(localFileName.c_str());
	my_serverVersion->setText(serverVersion.c_str());
	my_localVersion->setText(localVersion.c_str());

	my_fileName->setReadOnly(true);
	my_serverVersion->setReadOnly(true);
	my_localVersion->setReadOnly(true);

	my_fileName->setMinimumWidth(300);

	setWindowTitle("Project Information");
	setWindowIcon(windowIcon);

	// Hide info button
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	if (_colorStyle != nullptr) { setColorStyle(_colorStyle); }

	connect(my_buttonClose, SIGNAL(clicked()), this, SLOT(slotButtonClosePressed()));
}

ProjectInformationDialog::~ProjectInformationDialog() {
	delete my_buttonClose;
	delete my_fileName;
	delete my_serverVersion;
	delete my_localVersion;
	delete my_lFileName;
	delete my_lServerVersion;
	delete my_lLocalVersion;

	delete my_layoutButtons;
	delete my_widgetButtons;

	delete my_layoutFileName;
	delete my_widgetFileName;
	delete my_layoutLocalVersion;
	delete my_widgetLocalVersion;
	delete my_layoutServerVersion;
	delete my_widgetServerVersion;

	delete my_layout;
}


void ProjectInformationDialog::setColorStyle(ak::aColorStyle* _colorStyle) {
	if (_colorStyle == nullptr) {
		setStyleSheet("");
		my_buttonClose->setStyleSheet("");
		my_fileName->setStyleSheet("");
		my_serverVersion->setStyleSheet("");
		my_lLocalVersion->setStyleSheet("");
		my_lFileName->setStyleSheet("");
		my_lServerVersion->setStyleSheet("");
		my_lLocalVersion->setStyleSheet("");
	}
	else {
		setStyleSheet(_colorStyle->toStyleSheet(ak::cafBackgroundColorDialogWindow | ak::cafForegroundColorDialogWindow, "QDialog {", "}"));

		QString Color = _colorStyle->getControlsBorderColor().toHexString(true);
		my_fileName->setStyleSheet(_colorStyle->toStyleSheet(ak::cafForegroundColorControls |
			ak::cafBackgroundColorControls | ak::cafBorderColorControls, "QLineEdit{", "border: 1px solid #" + Color + ";}"));
		my_serverVersion->setStyleSheet(_colorStyle->toStyleSheet(ak::cafForegroundColorControls |
			ak::cafBackgroundColorControls | ak::cafBorderColorControls, "QLineEdit{", "border: 1px solid #" + Color + ";}"));
		my_localVersion->setStyleSheet(_colorStyle->toStyleSheet(ak::cafForegroundColorControls |
			ak::cafBackgroundColorControls | ak::cafBorderColorControls, "QLineEdit{", "border: 1px solid #" + Color + ";}"));

		QString sheet(_colorStyle->toStyleSheet(ak::cafForegroundColorButton |
			ak::cafBackgroundColorButton, "QPushButton{", "}\n"));
		sheet.append(_colorStyle->toStyleSheet(ak::cafForegroundColorFocus |
			ak::cafBackgroundColorFocus, "QPushButton:hover:!pressed{", "}\n"));
		sheet.append(_colorStyle->toStyleSheet(ak::cafForegroundColorSelected |
			ak::cafBackgroundColorSelected, "QPushButton:pressed{", "}\n"));
		my_buttonClose->setStyleSheet(sheet);

		my_lFileName->setStyleSheet(_colorStyle->toStyleSheet(ak::cafForegroundColorControls |
			ak::cafBackgroundColorTransparent));
		my_lServerVersion->setStyleSheet(_colorStyle->toStyleSheet(ak::cafForegroundColorControls |
			ak::cafBackgroundColorTransparent));
		my_lLocalVersion->setStyleSheet(_colorStyle->toStyleSheet(ak::cafForegroundColorControls |
			ak::cafBackgroundColorTransparent));
	}
}

void ProjectInformationDialog::slotButtonClosePressed() { Close(); }



void ProjectInformationDialog::Close(void) {

	emit isClosing();
	close();
}
