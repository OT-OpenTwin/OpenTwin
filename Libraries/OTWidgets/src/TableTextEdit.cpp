//! @file TableTextEdit.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTWidgets/Table.h"
#include "OTWidgets/TableTextEdit.h"

ot::TableTextEdit::TableTextEdit(QWidget* _parent, Table* _table, const QModelIndex& _modelIndex)
	: PlainTextEdit(_parent), m_table(_table), m_modelIndex(_modelIndex)
{
	OTAssertNullptr(_table);

	this->setAutoScrollToBottomEnabled(false);
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ot::TableTextEdit::~TableTextEdit() {

}

void ot::TableTextEdit::keyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Return) {
		_event->accept();

		if (_event->modifiers() == Qt::ControlModifier || _event->modifiers() == Qt::ShiftModifier) {
			if (!this->isReadOnly()) {
				this->insertPlainText("\n");
				this->document()->setModified(true);
			}
		}
		else {
			this->clearFocus();
			this->focusCell(m_modelIndex.row() + 1, m_modelIndex.column());
		}
	}
	else if (_event->key() == Qt::Key_Tab) {
		_event->accept();

		if (_event->modifiers() == Qt::ControlModifier || _event->modifiers() == Qt::ShiftModifier) {
			if (!this->isReadOnly()) {
				this->insertPlainText("\t");
				this->document()->setModified(true);
			}
		}
		else {
			this->clearFocus();
			if (!this->focusCell(m_modelIndex.row(), m_modelIndex.column() + 1)) {
				this->focusCell(m_modelIndex.row() + 1, 0);
			}
		}
	}
	else if (_event->key() == Qt::Key_Escape) {
		_event->accept();

		this->clearFocus();
	}
	else if (_event->key() == Qt::Key_Left && _event->modifiers() == Qt::ControlModifier) {
		_event->accept();
		this->clearFocus();
		if (!this->focusCell(m_modelIndex.row(), m_modelIndex.column() - 1)) {
			this->focusCell(m_modelIndex.row(), m_table->columnCount() - 1);
		}
	}
	else if (_event->key() == Qt::Key_Right && _event->modifiers() == Qt::ControlModifier) {
		_event->accept();
		this->clearFocus();
		if (!this->focusCell(m_modelIndex.row(), m_modelIndex.column() + 1)) {
			this->focusCell(m_modelIndex.row(), 0);
		}
	}
	else if (_event->key() == Qt::Key_Up && _event->modifiers() == Qt::ControlModifier) {
		_event->accept();
		this->clearFocus();
		if (!this->focusCell(m_modelIndex.row() - 1, m_modelIndex.column())) {
			this->focusCell(m_table->rowCount() - 1, m_modelIndex.column());
		}
	}
	else if (_event->key() == Qt::Key_Down && _event->modifiers() == Qt::ControlModifier) {
		_event->accept();
		this->clearFocus();
		if (!this->focusCell(m_modelIndex.row() + 1, m_modelIndex.column())) {
			this->focusCell(0, m_modelIndex.column());
		}
	}
	else {
		PlainTextEdit::keyPressEvent(_event);
	}
}

void ot::TableTextEdit::keyReleaseEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Return || _event->key() == Qt::Key_Tab || _event->key() == Qt::Key_Escape) {
		_event->accept();
	}
	else {
		PlainTextEdit::keyPressEvent(_event);
	}
}

bool ot::TableTextEdit::focusCell(int _row, int _column) {
	if (_row >= 0 && _row < m_table->rowCount() && _column >= 0 && _column < m_table->columnCount()) {
		m_table->setCurrentCell(_row, _column, QItemSelectionModel::Current);

		QTableWidgetItem* itm = m_table->item(_row, _column);
		if (itm) {
			m_table->editItem(itm);
		}
		else {
			QWidget* nextWidget = m_table->cellWidget(_row, _column);
			if (nextWidget) {
				nextWidget->setFocus();
			}
		}

		return true;
	}
	else {
		return false;
	}
}
