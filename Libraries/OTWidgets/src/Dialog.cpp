// @otlicense
// File: Dialog.cpp
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
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/Positioning.h"
#include "OTWidgets/IconManager.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtWidgets/qlayout.h>

ot::Dialog::Dialog(QWidget* _parent)
	: QDialog(_parent), m_flags(DialogCfg::NoFlags), m_result(DialogResult::Cancel), m_state(DialogState::NoState)
{
	// Setup window
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
	this->setWindowIcon(IconManager::getApplicationIcon());
}

ot::Dialog::Dialog(const DialogCfg& _config, QWidget* _parent) 
	: QDialog(_parent), m_flags(_config.getFlags()), m_dialogName(_config.getName()), m_result(DialogResult::Cancel)
{
	// Setup window
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
	this->setWindowTitle(QString::fromStdString(_config.getTitle()));
	this->setWindowIcon(IconManager::getApplicationIcon());

	// Initialize size constrains and initial size
	if (_config.getMinSize().width() >= 0) { this->setMinimumWidth(_config.getMinSize().width()); }
	if (_config.getMinSize().height() >= 0) { this->setMinimumHeight(_config.getMinSize().height()); }
	if (_config.getMaxSize().width() >= 0) { this->setMaximumWidth(_config.getMaxSize().width()); }
	if (_config.getMaxSize().height() >= 0) { this->setMaximumHeight(_config.getMaxSize().height()); }
	QSize iniSize = this->size();
	if (_config.getInitialSize().width() >= 0) { iniSize.setWidth(_config.getInitialSize().width()); }
	if (_config.getInitialSize().height() >= 0) { iniSize.setHeight(_config.getInitialSize().height()); }
	this->resize(iniSize);
}

ot::Dialog::~Dialog() {}

ot::Dialog::DialogResult ot::Dialog::showDialog(const ShowFlags& _showFlags) {
	if (_showFlags & CenterOnParent && this->parentWidget()) {
		Positioning::centerWidgetOnParent(this->parentWidget(), this);
	}
	if (_showFlags & FitOnScreen) {
		QRect newRec = Positioning::fitOnScreen(QRect(this->pos(), this->size()));
		if (this->pos() != newRec.topLeft()) {
			this->move(newRec.topLeft());
		}
	}

	this->exec();
	return m_result;
}

// ###########################################################################################################################################################################################################################################################################################################################

// Public slots

void ot::Dialog::closeDialog(DialogResult _result) {
	m_result = _result;
	this->close();
}

std::list<ot::PushButton*> ot::Dialog::generateDefaultButtons(const std::list<std::pair<QString, DialogResult>>& _buttonTextToResultList, QLayout* _layout) {
	std::list<PushButton*> result;
	PushButton* btn = nullptr;
	for (const auto& info : _buttonTextToResultList) {
		result.push_back(btn = new PushButton(info.first));
		if (_layout) {
			_layout->addWidget(btn);
		}

		switch (info.second) {
		case ot::Dialog::Ok: this->connect(btn, &PushButton::clicked, this, &Dialog::closeOk); break;
		case ot::Dialog::Yes: this->connect(btn, &PushButton::clicked, this, &Dialog::closeYes); break;
		case ot::Dialog::No: this->connect(btn, &PushButton::clicked, this, &Dialog::closeNo); break;
		case ot::Dialog::Retry: this->connect(btn, &PushButton::clicked, this, &Dialog::closeRetry); break;
		case ot::Dialog::Cancel: this->connect(btn, &PushButton::clicked, this, &Dialog::closeCancel); break;
		default:
			OT_LOG_EAS("Unknown dialog result (" + std::to_string((int)info.second) + ")");
			break;
		}
	}

	return result;
}

std::list<ot::PushButton*> ot::Dialog::generateDefaultButtons(const std::initializer_list<DialogResult>& _buttonResults, QLayout* _layout) {
	std::list<DialogResult> lst(_buttonResults);
	std::list<PushButton*> ret;
	PushButton* btn = nullptr;

	for (DialogResult result : _buttonResults) {
		switch (result) {
		case ot::Dialog::Ok:
			ret.push_back(btn = new PushButton("Ok"));
			this->connect(btn, &PushButton::clicked, this, &Dialog::closeOk); 
			break;

		case ot::Dialog::Confirm:
			ret.push_back(btn = new PushButton("Confirm"));
			this->connect(btn, &PushButton::clicked, this, &Dialog::closeConfirm);
			break;

		case ot::Dialog::Yes:
			ret.push_back(btn = new PushButton("Yes"));
			this->connect(btn, &PushButton::clicked, this, &Dialog::closeYes);
			break;

		case ot::Dialog::No:
			ret.push_back(btn = new PushButton("No"));
			this->connect(btn, &PushButton::clicked, this, &Dialog::closeNo);
			break;

		case ot::Dialog::Retry:
			ret.push_back(btn = new PushButton("Retry"));
			this->connect(btn, &PushButton::clicked, this, &Dialog::closeRetry);
			break;

		case ot::Dialog::Cancel:
			ret.push_back(btn = new PushButton("Cancel"));
			this->connect(btn, &PushButton::clicked, this, &Dialog::closeCancel);
			break;

		default:
			OT_LOG_EAS("Unknown dialog result (" + std::to_string((int)result) + ")");
			break;
		}

		if (_layout && btn) {
			_layout->addWidget(btn);
		}
	}

	return ret;
}

void ot::Dialog::closeOk(void) {
	this->closeDialog(Dialog::Ok);
}

void ot::Dialog::closeConfirm(void) {
	this->closeDialog(Dialog::Confirm);
}

void ot::Dialog::closeYes(void) {
	this->closeDialog(Dialog::Yes);
}

void ot::Dialog::closeNo(void) {
	this->closeDialog(Dialog::No);
}

void ot::Dialog::closeRetry(void) {
	this->closeDialog(Dialog::Retry);
}

void ot::Dialog::closeCancel(void) {
	this->closeDialog(Dialog::Cancel);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::Dialog::keyPressEvent(QKeyEvent* _event) {
	QDialog::keyPressEvent(_event);
	if ((_event->key() == Qt::Key_F11) && (m_flags & DialogCfg::DialogFlag::RecenterOnF11)) {
		Positioning::centerWidgetOnParent(nullptr, static_cast<QWidget*>(this));
	}
}

void ot::Dialog::mousePressEvent(QMouseEvent* _event) {
	QDialog::mousePressEvent(_event);
	if (_event->button() == Qt::LeftButton && m_flags & DialogCfg::MoveGrabAnywhere) {
		m_lastMousePos = _event->globalPos();
		m_state.set(DialogState::MousePressed, true);
	}
}

void ot::Dialog::mouseMoveEvent(QMouseEvent* _event) {
	QDialog::mouseMoveEvent(_event);
	if (m_flags & DialogCfg::MoveGrabAnywhere && m_state & DialogState::MousePressed) {
		QPoint delta = _event->globalPos() - m_lastMousePos;
		if (delta.isNull()) return;
		this->move(this->pos() + delta);
		m_lastMousePos = _event->globalPos();
	}
}

void ot::Dialog::mouseReleaseEvent(QMouseEvent* _event) {
	QDialog::mouseReleaseEvent(_event);
	if (_event->button() == Qt::LeftButton) {
		m_state.set(DialogState::MousePressed, false);
	}
}

void ot::Dialog::closeEvent(QCloseEvent* _event) {
	m_state.set(DialogState::Closing, true);

	if (this->mayCloseDialogWindow()) {
		QDialog::closeEvent(_event);
	}
	else {
		_event->accept();
	}
}

bool ot::Dialog::mayCloseDialogWindow(void) {
	bool continueClose = true;
	
	Q_EMIT closingDialog(continueClose);

	return continueClose;
}
