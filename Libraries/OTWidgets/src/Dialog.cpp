//! @file Dialog.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/Positioning.h"

// Qt header
#include <QtGui/qevent.h>

ot::Dialog::Dialog(QWidget* _parent)
	: QDialog(_parent), m_flags(DialogCfg::NoFlags), m_result(DialogResult::Cancel), m_state(DialogState::NoState)
{
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
}

ot::Dialog::Dialog(const DialogCfg& _config, QWidget* _parent) 
	: QDialog(_parent), m_flags(_config.getFlags()), m_dialogName(_config.getName()), m_result(DialogResult::Cancel)
{
	this->setWindowFlags(this->windowFlags() & (~Qt::WindowContextHelpButtonHint));
	this->setWindowTitle(QString::fromStdString(_config.getTitle()));
	if (_config.getMinSize().width() >= 0) { this->setMinimumWidth(_config.getMinSize().width()); };
	if (_config.getMinSize().height() >= 0) { this->setMinimumHeight(_config.getMinSize().height()); };
	if (_config.getMaxSize().width() >= 0) { this->setMaximumWidth(_config.getMaxSize().width()); };
	if (_config.getMaxSize().height() >= 0) { this->setMaximumHeight(_config.getMaxSize().height()); };
}

ot::Dialog::~Dialog() {}

ot::Dialog::DialogResult ot::Dialog::showDialog(const ShowFlags& _showFlags) {
	if (_showFlags & CenterOnParent && this->parentWidget()) {
		this->centerOnParent(this->parentWidget());
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

void ot::Dialog::close(DialogResult _result) {
	m_result = _result;
	this->QDialog::close();
}

void ot::Dialog::closeOk(void) {
	this->close(Dialog::Ok);
}

void ot::Dialog::closeYes(void) {
	this->close(Dialog::Yes);
}

void ot::Dialog::closeNo(void) {
	this->close(Dialog::No);
}

void ot::Dialog::closeRetry(void) {
	this->close(Dialog::Retry);
}

void ot::Dialog::closeCancel(void) {
	this->close(Dialog::Cancel);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Protected

void ot::Dialog::keyPressEvent(QKeyEvent* _event) {
	QDialog::keyPressEvent(_event);
	if ((_event->key() == Qt::Key_F11) && (m_flags & DialogCfg::DialogFlag::RecenterOnF11)) {
		this->centerOnParent(nullptr);
	}
}

void ot::Dialog::mousePressEvent(QMouseEvent* _event) {
	QDialog::mousePressEvent(_event);
	if (_event->button() == Qt::LeftButton && m_flags & DialogCfg::MoveGrabAnywhere) {
		m_lastMousePos = _event->globalPos();
		m_state.setFlag(DialogState::MousePressed, true);
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
		m_state.setFlag(DialogState::MousePressed, false);
	}
}
