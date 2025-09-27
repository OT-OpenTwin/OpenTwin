//! @file TextEditorSearchPopup.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TextEditorSearchPopup.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qtextdocument.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qscrollbar.h>

ot::TextEditorSearchPopup::TextEditorSearchPopup(ot::TextEditor* _editor)
	: OverlayWidgetBase(_editor, AlignTopRight, QMargins(5, 2, 25, 2)), m_editor(_editor), m_search(nullptr), m_ix(0)
{
	OTAssertNullptr(m_editor);
	 
	this->setObjectName("OT_TextEditSearchPopup");
	this->setWindowFlag(Qt::FramelessWindowHint);
	this->setWindowFlag(Qt::Tool);
	this->setFrameShape(QFrame::StyledPanel);
	this->setAttribute(Qt::WA_DeleteOnClose);
	this->setFocusPolicy(Qt::ClickFocus);

	// Create layouts and controls
	QHBoxLayout* centralLayout = new QHBoxLayout(this);
	centralLayout->setContentsMargins(1, 1, 1, 1);

	m_search = new ot::LineEdit(this);
	m_search->setFocusPolicy(Qt::StrongFocus);
	m_search->setObjectName("OT_TextEditSearchPopupSearchInput");
	PushButton* btnFind = new PushButton("Find", this);
	btnFind->setObjectName("OT_TextEditSearchPopupFindButton");
	PushButton* btnClose = new PushButton("Close", this);
	btnClose->setObjectName("OT_TextEditSearchPopupCloseButton");

	centralLayout->addWidget(m_search, 1);
	centralLayout->addWidget(btnFind);
	centralLayout->addWidget(btnClose);

	// Setup event filter
	m_search->installEventFilter(this);
	btnFind->installEventFilter(this);
	btnClose->installEventFilter(this);

	// Setup view
	this->setMinimumSize(200, 30);
	this->setMaximumSize(350, 30);
	this->setFocusProxy(m_search);

	// Connect signals
	this->connect(m_search, &LineEdit::textChanged, this, &TextEditorSearchPopup::slotTextChanged);
	this->connect(m_search, &LineEdit::returnPressed, this, &TextEditorSearchPopup::slotFindNext);
	this->connect(btnFind, &PushButton::clicked, this, &TextEditorSearchPopup::slotFindNext);
	this->connect(btnClose, &PushButton::clicked, this, &TextEditorSearchPopup::close);
}

void ot::TextEditorSearchPopup::focusInput(void) {
	m_search->setFocus();
	m_search->activateWindow();
}

bool ot::TextEditorSearchPopup::eventFilter(QObject* _watched, QEvent* _event) {
	if (_event->type() == QEvent::KeyPress) {
		QKeyEvent* ev = dynamic_cast<QKeyEvent*>(_event);
		if (ev) {
			if (ev->key() == Qt::Key::Key_Escape) {
				this->close();
				return true;
			}
		}
	}
	return OverlayWidgetBase::eventFilter(_watched, _event);
}

void ot::TextEditorSearchPopup::closeEvent(QCloseEvent* _event) {
	Q_EMIT popupClosing();
	QWidget::closeEvent(_event);
}

void ot::TextEditorSearchPopup::updatePosition(bool _forceUpdate) {
	return;
	QPoint p = m_editor->mapToGlobal(m_editor->rect().topRight());
	if (p == m_lastTR && !_forceUpdate) return;
	m_lastTR = p;

	p.setX(p.x() - this->width());
	if (m_editor->verticalScrollBar()->isVisible()) {
		p.setX(p.x() - m_editor->verticalScrollBar()->width());
	}
	this->move(p);
}

void ot::TextEditorSearchPopup::setCurrentText(const QString& _text) {
	m_search->setText(_text);
}

void ot::TextEditorSearchPopup::slotTextChanged(void) {
	int ix = m_ix;
	this->slotFindNext();
	m_ix = ix;
}

void ot::TextEditorSearchPopup::slotFindNext(void) {
	QTextCursor cursor = m_editor->document()->find(m_search->text(), m_ix);
	if (!cursor.isNull()) {
		m_ix = cursor.position();
		m_editor->setTextCursor(cursor);
		m_editor->ensureCursorVisible();
	}
	else if (m_ix > 0) {
		m_ix = 0;
		this->slotFindNext();
	}
}
