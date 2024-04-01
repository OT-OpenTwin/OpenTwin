//! @file LogInDialog.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// Frontend header
#include "AppBase.h"
#include "LogInDialog.h"

// OpenTwin header
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ComboButton.h"

// Qt header
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtOpenGLWidgets/qopenglwidget.h>

LogInDialog::LogInDialog() {
	using namespace ot;

	// Create layouts
	QVBoxLayout* centralLayout = new QVBoxLayout(this);
	QHBoxLayout* midLayout = new QHBoxLayout;
	QWidget* inputBox = new QWidget;
	inputBox->setObjectName("InputBox");
	QVBoxLayout* inputBoxLayout = new QVBoxLayout(inputBox);
	QGridLayout* inputLayout = new QGridLayout;
	
	// Create controls
	QLabel* lGss = new QLabel("Global Session Service:");
	m_gss = new ComboButton;
	QLabel* lName = new QLabel("Username:");
	LineEdit* eName = new LineEdit;
	QLabel* lPassword = new QLabel("Password:");
	LineEdit* ePassword = new LineEdit;
	QLabel* lNewUser = new QLabel("Create new account");

	// Setup controls
	auto settings = AppBase::instance()->createSettingsInstance();

	eName->setText(settings->value("LastUsername", QString()).toString());
	m_lastPassword = settings->value("LastPassword", QString()).toString();
	QString lastSessionService = settings->value("SessionServiceURL", "").toString();

	// Setup layouts

	// Setup window
	this->setObjectName("LogInDialog");

	// Connect signals

}

LogInDialog::~LogInDialog() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// Private slots

void LogInDialog::slotLogIn(void) {

}

void LogInDialog::slotCancel(void) {

}

void LogInDialog::slotNewUser(void) {

}

void LogInDialog::slotGSSChanged(void) {

}