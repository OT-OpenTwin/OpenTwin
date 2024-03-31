/*
 *	File:		aLogInDialog.cpp
 *	Package:	akDialogs
 *
 *  Created on: October 06, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// Corresponding header
#include <akDialogs/aLogInDialog.h>

// AK Core header
#include <akCore/akCore.h>
#include <akCore/aMessenger.h>
#include <akCore/aAssert.h>

// AK Widgets header
#include <akWidgets/aLineEditWidget.h>
#include <akWidgets/aLabelWidget.h>
#include <akWidgets/aPushButtonWidget.h>
#include <akWidgets/aCheckBoxWidget.h>
#include <akWidgets/aAnimationOverlayWidget.h>
#include <akWidgets/aComboButtonWidget.h>

// Qt header
#include <qcryptographichash.h>		// Hashing the password
#include <qwidget.h>				// QWidget
#include <qlayout.h>				// QLayout
#include <qtooltip.h>				// QToolTip
#include <qpainter.h>
#include <qevent.h>

ak::aLogInDialog::aLogInDialog(
	bool										_showSavePassword,
	const QPixmap &								_backgroundImage,
	const QString &								_username,
	const QString &								_hashedPassword,
	QWidget *									_parent
) : aDialog(otLogInDialog, _parent), m_hashedPw(_hashedPassword),
	m_buttonLogIn(nullptr), m_buttonRegister(nullptr), m_savePassword(nullptr), m_mainLayout(nullptr), m_inputPassword(nullptr), m_inputUsername(nullptr),
	m_currentID(ak::invalidID), m_rowCounter(0), m_showSavePassword(_showSavePassword), m_passwordHashAlgorithm(hashSha_256), m_returnHashedPassword(true)
{
	// Create layouts
	m_centralLayout = new QVBoxLayout(this);
	m_centralLayout->setContentsMargins(0, 0, 0, 0);
	m_animationWidget = new aAnimationOverlayWidget;
	m_centralLayout->addWidget(m_animationWidget);

	m_vLayoutW = new QWidget;
	m_vLayout = new QVBoxLayout(m_vLayoutW);
	m_animationWidget->setChild(m_vLayoutW);

	m_hLayoutW = new QWidget;
	m_hLayout = new QHBoxLayout(m_hLayoutW);
	m_vLayout->addStretch(1);
	m_vLayout->addWidget(m_hLayoutW);

	m_mainLayoutW = new QWidget;
	m_mainLayout = new QVBoxLayout(m_mainLayoutW);
	m_hLayout->addStretch(1);
	m_hLayout->addWidget(m_mainLayoutW);
	m_hLayout->addStretch(1);

	m_gridLayoutW = new QWidget;
	m_gridLayout = new QGridLayout(m_gridLayoutW);
	m_gridLayout->setContentsMargins(0, 0, 0, 0);
	m_mainLayout->addWidget(m_gridLayoutW);

	// Create username objects
	m_inputUsername = new aLogInDialogInputField(new aLineEditWidget(_username), new aLabelWidget("Username:"));

	// Create password objects
	aLineEditWidget * pwInput = new aLineEditWidget;
	m_inputPassword = new aLogInDialogInputField(pwInput, new aLabelWidget("Password:"));
	pwInput->setEchoMode(QLineEdit::EchoMode::Password);
	if (_hashedPassword.length() > 0) { pwInput->setText("xxxxxxxxxx"); }

	// Add inputs to layout
	m_gridLayout->addWidget(m_inputUsername->getLabel(), m_rowCounter, 0);
	m_gridLayout->addWidget(m_inputUsername->getInputWidget(), m_rowCounter++, 1);
	m_gridLayout->addWidget(m_inputPassword->getLabel(), m_rowCounter, 0);
	m_gridLayout->addWidget(m_inputPassword->getInputWidget(), m_rowCounter++, 1);

	// Check if required to create save password
	if (m_showSavePassword) {
		m_savePassword = new aCheckBoxWidget("Save password");
		m_savePassword->setChecked(true);
		m_gridLayout->addWidget(m_savePassword, m_rowCounter++, 1);
	}

	// Create log in button
	m_buttonLogIn = new aPushButtonWidget("Log-in", this);
	m_mainLayout->addWidget(m_buttonLogIn);

	// Create register button
	m_buttonRegister = new aLabelWidget("New user? Register", this);
	m_buttonRegister->setAlignment(Qt::AlignRight);
	m_mainLayout->addWidget(m_buttonRegister);

	// Connect signals
	connect(m_buttonLogIn, &QPushButton::clicked, this, &aLogInDialog::slotClicked);
	connect(m_buttonRegister, &aLabelWidget::clicked, this, &aLogInDialog::registerClicked);
	connect(dynamic_cast<aLineEditWidget *>(m_inputPassword->getInputWidget()), &QLineEdit::textChanged, this, &aLogInDialog::slotPasswordChanged);
	connect(dynamic_cast<aLineEditWidget *>(m_inputUsername->getInputWidget()), &QLineEdit::textChanged, this, &aLogInDialog::slotUsernameChanged);

	// Create main layout and display data
	m_mainLayoutW->setObjectName("LogInDialogMainLayoutW");
	m_mainLayoutW->setStyleSheet("#LogInDialogMainLayoutW{"
		"background-color:#90000000;"
		"border-radius:10px;"
		"}\n"
		"QLabel{color:#FFFFFF}\n"
		"QCheckBox{color:#FFFFFF}\n");
	m_bgImage = new QPixmap(_backgroundImage);

	setWindowTitle("Welcome");

	hideInfoButton();

	resize(m_bgImage->size());
}

ak::aLogInDialog::~aLogInDialog() {
	A_OBJECT_DESTROYING

	delete m_inputPassword;
	delete m_inputUsername;

	for (auto itm : m_customInputFields) {
		delete itm.second;
	}
	m_customInputFields.clear();

	if (m_savePassword != nullptr) { delete m_savePassword; }

	if (m_buttonLogIn != nullptr) {
		disconnect(m_buttonLogIn, SIGNAL(clicked()), this, SLOT(slotClicked()));
		disconnect(m_buttonRegister, SIGNAL(clicked()), this, SLOT(registerClicked()));
		delete m_buttonLogIn;
		delete m_buttonRegister;
	}

	delete m_gridLayout;
	delete m_gridLayoutW;
	delete m_mainLayout;
	delete m_mainLayoutW;
	delete m_hLayout;
	delete m_hLayoutW;
	delete m_vLayout;
	delete m_vLayoutW;
	delete m_animationWidget;
	delete m_centralLayout;
}

void ak::aLogInDialog::paintEvent(QPaintEvent *pe)
{
	QPainter paint(this);
	m_currentImage = m_bgImage->scaled(width(), height(), Qt::KeepAspectRatioByExpanding);
	QPoint centerOfWidget = rect().center();
	QRect rectOfPixmap = m_currentImage.rect();
	rectOfPixmap.moveCenter(centerOfWidget);
	paint.drawPixmap(rectOfPixmap.topLeft(), m_currentImage);
}

ak::ID ak::aLogInDialog::addCustomInput(
	const QString &						_labelText,
	const QString &						_initialInputText
) {
	// Create new item
	aLogInDialogInputField * newItem = new aLogInDialogInputField(new aLineEditWidget(_initialInputText), new aLabelWidget(_labelText));
	m_customInputFields.insert_or_assign(++m_currentID, newItem);
	
	rebuildInputLayout();

	return m_currentID;
}

ak::ID ak::aLogInDialog::addCustomInput(aLogInDialogInputField * _input) {
	aAssert(_input, "The provided input field is NULL");
	m_customInputFields.insert_or_assign(++m_currentID, _input);

	rebuildInputLayout();

	return m_currentID;
}

// #############################################################################################################

// Getter

QString ak::aLogInDialog::username(void) const { return m_inputUsername->text(); }

QString ak::aLogInDialog::password(void) const {
	if (m_hashedPw.length() > 0) { return m_hashedPw; }

	if (m_returnHashedPassword)
	{
		return hashString(m_inputPassword->text(), m_passwordHashAlgorithm);
	}

	return m_inputPassword->text();
}

bool ak::aLogInDialog::encryptedPassword(void) const {
	return (m_hashedPw.length() > 0);
}

QString ak::aLogInDialog::customInputText(ak::ID _id) {
	auto itm = m_customInputFields.find(_id);
	assert(itm != m_customInputFields.end());	// Invalid id
	return itm->second->text();
}

bool ak::aLogInDialog::savePassword(void) const {
	if (m_savePassword == nullptr) { return false; }
	else { return m_savePassword->isChecked(); }
}

// #############################################################################################################

// Tool tip

void ak::aLogInDialog::showToolTipAtUsername(
	const QString &							_text
) { createToolTip(m_inputUsername->getInputWidget(), _text); }

void ak::aLogInDialog::showToolTipAtPassword(
	const QString &							_text
) {
	createToolTip(m_inputPassword->getInputWidget(), _text);
}

void ak::aLogInDialog::showToolTipAtCustomInput(
	ak::ID									_inputID,
	const QString &							_text
) {
	auto itm = m_customInputFields.find(_inputID);
	assert(itm != m_customInputFields.end());	// Invalid id
	createToolTip(itm->second->getInputWidget(), _text);
}

// #############################################################################################################

// Waiting animation

void ak::aLogInDialog::setWaitingAnimationDelay(int _delay) {
	m_animationWidget->setAnimationDelay(_delay);
}

void ak::aLogInDialog::showWaitingAnimation(QMovie * _animation) {
	m_mainLayoutW->setEnabled(false);
	m_animationWidget->setWaitingAnimation(_animation);
	m_animationWidget->setWaitingAnimationVisible(true);
}

void ak::aLogInDialog::hideWaitingAnimation(void) {
	m_mainLayoutW->setEnabled(true);
	m_animationWidget->setWaitingAnimationVisible(false);
}

// #############################################################################################################

// Slots

void ak::aLogInDialog::slotClicked(void) {
	QString pw(m_inputPassword->text());
	QString user(m_inputUsername->text());
	
	// Check if something was entered
	if (user.length() == 0) {
		createToolTip(m_inputUsername->getInputWidget(), "Please enter a username");
		return;
	}
	if (pw.length() == 0) {
		createToolTip(m_inputPassword->getInputWidget(), "Please enter a password");
		return;
	}

	Q_EMIT logInRequested();
}

void ak::aLogInDialog::registerClicked(void) {

	Q_EMIT registerRequested();
}

void ak::aLogInDialog::slotUsernameChanged(const QString & _text) {
	if (m_hashedPw.length() > 0) { m_hashedPw.clear(); m_inputPassword->clearInput(); }
}

void ak::aLogInDialog::slotPasswordChanged(const QString &	_text) {
	if (m_hashedPw.length() > 0) {
		m_hashedPw.clear();
		if (m_inputPassword->text().length() == 0) { return; }
		for (auto c : _text) {
			if (c != 'x') {
				((aLineEditWidget *)m_inputPassword->getInputWidget())->setText(c);
				return;
			}
		}
		((aLineEditWidget *)m_inputPassword->getInputWidget())->setText("x");
	}
}

// #############################################################################################################

// Private functions

void ak::aLogInDialog::rebuildInputLayout(void) {
	// Reset grid layout
	assert(m_gridLayout);
	delete m_gridLayout;
	m_gridLayout = nullptr;
	m_gridLayout = new QGridLayout(m_gridLayoutW);
	m_rowCounter = 0;

	// Place all widgets
	for (auto itm : m_customInputFields) {
		m_gridLayout->addWidget(itm.second->getLabel(), m_rowCounter, 0);
		m_gridLayout->addWidget(itm.second->getInputWidget(), m_rowCounter++, 1);
	}

	m_gridLayout->addWidget(m_inputUsername->getLabel(), m_rowCounter, 0);
	m_gridLayout->addWidget(m_inputUsername->getInputWidget(), m_rowCounter++, 1);

	m_gridLayout->addWidget(m_inputPassword->getLabel(), m_rowCounter, 0);
	m_gridLayout->addWidget(m_inputPassword->getInputWidget(), m_rowCounter++, 1);

	if (m_showSavePassword) {
		m_gridLayout->addWidget(m_savePassword, m_rowCounter++, 1);
	}
}

void ak::aLogInDialog::createToolTip(
	QWidget *				_parent,
	const QString &			_text
) const {
	QToolTip::showText(_parent->mapToGlobal(QPoint(0, 0)), _text, nullptr, QRect(), 3000);
}

// #######################################################################################################################################################################

// #######################################################################################################################################################################

// #######################################################################################################################################################################

ak::aLogInDialogInputField::aLogInDialogInputField(
	aLineEditWidget *				_input,
	aLabelWidget *					_label
) : m_input(_input), m_label(_label), m_comboButton(nullptr)
{
	assert(m_input != nullptr);
	assert(m_label != nullptr);
	m_label->setBuddy(m_input);
}

ak::aLogInDialogInputField::aLogInDialogInputField(
	aComboButtonWidget *	_input,
	aLabelWidget *			_label
) : m_input(nullptr), m_label(_label), m_comboButton(_input)
{

}

ak::aLogInDialogInputField::~aLogInDialogInputField() {
	if (m_input) { delete m_input; }
	if (m_comboButton) { delete m_comboButton; }
	if (m_label) { delete m_label; }
}

// #############################################################################################################

// Getter

QWidget * ak::aLogInDialogInputField::getInputWidget(void) const {
	if (m_input) { return m_input; }
	if (m_comboButton) { return m_comboButton; }
	aAssert(0, "No input widget assigned");
	return nullptr;
}

void ak::aLogInDialogInputField::clearInput(void) {
	assert(m_input != nullptr);
	m_input->clear();
}

QString ak::aLogInDialogInputField::text(void) const {
	assert(m_input != nullptr);
	return m_input->text();
}
