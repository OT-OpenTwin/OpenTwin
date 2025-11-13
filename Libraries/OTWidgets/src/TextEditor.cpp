// @otlicense
// File: TextEditor.cpp
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
#include "OTCore/RuntimeTests.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/DefaultSyntaxHighlighterRules.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/SyntaxHighlighter.h"
#include "OTWidgets/TextEditorSearchPopup.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextlist.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qscrollbar.h>

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_TEXTEDITOR_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_TEXTEDITOR_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_TEXTEDITOR_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_TEXTEDITOR_PERFORMANCETEST_ENABLED==true
#define OT_TEST_TEXTEDITOR_Interval(___testText) OT_TEST_Interval(ot_intern_texteditor_lcl_performancetest, "TextEditor", ___testText)
#else
#define OT_TEST_TEXTEDITOR_Interval(___testText)
#endif

ot::TextEditorLineNumberArea::TextEditorLineNumberArea(TextEditor* _editor)
	: QWidget(_editor), m_editor(_editor)
{}

ot::TextEditorLineNumberArea::~TextEditorLineNumberArea() {}

QSize ot::TextEditorLineNumberArea::sizeHint() const {
	return QSize(m_editor->lineNumberAreaWidth(), 0);
}

void ot::TextEditorLineNumberArea::paintEvent(QPaintEvent * _event) {
	m_editor->lineNumberAreaPaintEvent(_event);
}

// ###################################################################################################################################

// ###################################################################################################################################

// ###################################################################################################################################

ot::TextEditor::TextEditor(QWidget* _parent)
	: PlainTextEdit(_parent), m_syntaxHighlighter(nullptr), m_searchPopup(nullptr),
	m_tabSpaces(4), m_newLineSamePrefix(false), m_enableDuplicateLineShortcut(false), m_enableSameTextHighlighting(false),
	m_sameTextHighlightingMinimum(2), m_documentSyntax(DocumentSyntax::PlainText), m_fileExtensionFilter(FileExtension::toFilterString(FileExtension::AllFiles)),
	m_loadedChunkCount(0), m_showMoreLabel(nullptr)
{
	this->setObjectName("OT_TextEditor");

	m_lineNumberArea = new TextEditorLineNumberArea(this);

	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("Consolas");
	setFont(f);

	QFontMetrics fm(f);
	this->setTabStopDistance(4 * fm.horizontalAdvance(QChar(' ')));

	// "Show more" overlay label
	m_showMoreLabel = new Label("Show more...", this);
	m_showMoreLabel->setObjectName("OT_TextEditor_ShowMoreLabel");
	m_showMoreLabel->hide();
	connect(m_showMoreLabel, &Label::mouseClicked, this, &TextEditor::slotShowMore);
	connect(this->verticalScrollBar(), &QScrollBar::valueChanged, this, &TextEditor::slotUpdateShowMorePosition);

	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	QShortcut* findShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
	QShortcut* duplicateShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	saveShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	findShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	duplicateShortcut->setContext(Qt::WidgetWithChildrenShortcut);

	this->slotUpdateLineNumberAreaWidth(0);
	this->slotHighlightCurrentLine();

	this->connect(this, &TextEditor::blockCountChanged, this, &TextEditor::slotUpdateLineNumberAreaWidth);
	this->connect(this, &TextEditor::updateRequest, this, &TextEditor::slotUpdateLineNumberArea);
	this->connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::slotHighlightCurrentLine);
	this->connect(this, &TextEditor::selectionChanged, this, &TextEditor::slotSelectionChanged);
	this->connect(saveShortcut, &QShortcut::activated, this, &TextEditor::slotSaveRequested);
	this->connect(findShortcut, &QShortcut::activated, this, &TextEditor::slotFindRequested);
	this->connect(duplicateShortcut, &QShortcut::activated, this, &TextEditor::slotDuplicateLine);
	this->connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &TextEditor::slotCurrentColorStyleChanged);
}

ot::TextEditor::~TextEditor() {
	if (m_syntaxHighlighter) {
		delete m_syntaxHighlighter;
		m_syntaxHighlighter = nullptr;
	}
}

void ot::TextEditor::setupFromConfig(const TextEditorCfg& _config, bool _replaceText) {
	OT_TEST_TEXTEDITOR_Interval("Setup from config");
	QSignalBlocker sigBlock(this);
	
	m_documentSyntax = _config.getDocumentSyntax();
	m_fileExtensionFilter = _config.getFileExtensionFilters();

	if (_replaceText) {
		QString newText = QString::fromStdString(_config.getPlainText());
		newText.remove('\r');
		if (newText != this->toPlainText()) {
			this->setPlainText(newText);
			this->setContentSaved();
		}
	}

	this->setReadOnly(_config.getTextReadOnly());

	this->updateDocumentSyntax();
}

int ot::TextEditor::lineNumberAreaWidth() const {
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	digits += 2;

	int space = 3 + (fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits);

	return space;
}

void ot::TextEditor::lineNumberAreaPaintEvent(QPaintEvent * _event) {
	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	QPainter painter(m_lineNumberArea);
	QBrush backBrush = cs.getValue(ColorStyleValueEntry::TitleBackground).toBrush();
	QBrush frontBrush = cs.getValue(ColorStyleValueEntry::TitleForeground).toBrush();
	QBrush borderBrush = cs.getValue(ColorStyleValueEntry::Border).toBrush();
	painter.fillRect(_event->rect(), backBrush);
	painter.setPen(QPen(borderBrush, 1.));
	painter.drawLine(_event->rect().x() + _event->rect().width() - 1, _event->rect().y(),
		_event->rect().x() + _event->rect().width() - 1, _event->rect().y() + _event->rect().height());

	QTextBlock block = firstVisibleBlock();
	int blockNumber = block.blockNumber();
	int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
	int bottom = top + qRound(blockBoundingRect(block).height());
	int ptSize = font().pointSize() - 2;

	while (block.isValid() && top <= _event->rect().bottom()) {
		if (block.isVisible() && bottom >= _event->rect().top()) {
			QString number = QString::number(blockNumber + 1) + "  ";
			painter.setPen(QPen(frontBrush, 1.));
			QFont f = painter.font();
			f.setPointSize(ptSize);
			painter.setFont(f);
			painter.drawText(0, top, m_lineNumberArea->width(), fontMetrics().height(),
				Qt::AlignRight, number);
		}

		block = block.next();
		top = bottom;
		bottom = top + qRound(blockBoundingRect(block).height());
		++blockNumber;
	}
}

void ot::TextEditor::setContentChanged() {
	this->document()->setModified(true);
}

void ot::TextEditor::setContentSaved() {
	this->document()->setModified(false);
}

bool ot::TextEditor::getContentChanged() const {
	return this->document()->isModified();
}

void ot::TextEditor::setPlainText(const QString& _text) {
	bool tmp = this->signalsBlocked();
	bool tmpSyn = false;
	if (m_syntaxHighlighter) {
		tmpSyn = m_syntaxHighlighter->signalsBlocked();
		m_syntaxHighlighter->blockSignals(true);
	}
	this->blockSignals(true);
	
	m_textBuffer = _text;
	splitTextBufferIntoChunks();
	
	if (!m_textChunks.empty()) {
		QPlainTextEdit::setPlainText(m_textChunks[0].toString());
		m_loadedChunkCount = 1;
	}

	this->document()->clearUndoRedoStacks();
	
	this->setContentSaved();

	this->slotUpdateLineNumberAreaWidth(0);

	this->slotUpdateShowMorePosition();

	if (m_syntaxHighlighter) {
		m_syntaxHighlighter->blockSignals(tmpSyn);
	}
	this->blockSignals(tmp);

	this->setContentChanged();
}

bool ot::TextEditor::saveToFile(const QString& _fileName) {
	bool result = false;
	QFile file(_fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		OT_LOG_E("Failed to open file for writing: \"" + _fileName.toStdString() + "\"");
		return false;
	}
	else {
		QByteArray data = QByteArray::fromStdString(this->toPlainText().toStdString());
		result = file.write(data) == data.size();
		file.close();
	}

	return result;
}

QString ot::TextEditor::toPlainText() const {
	QString txt = QPlainTextEdit::toPlainText();
	
	// Append remaining chunks
	for (size_t i = m_loadedChunkCount; i < m_textChunks.size(); ++i) {
		txt.append(m_textChunks[i].toString());
	}

	return txt;
}

void ot::TextEditor::slotSaveRequested() {
	if (!this->document()->isModified()) {
		return;
	}
	Q_EMIT saveRequested();
}

void ot::TextEditor::setFileExtensionFilter(const std::list<FileExtension::DefaultFileExtension>& _extensions) {
	m_fileExtensionFilter = FileExtension::toFilterString(_extensions);
}

// ###################################################################################################################################

// Protected: Event handling

void ot::TextEditor::keyPressEvent(QKeyEvent* _event) {
	if (_event->key() == Qt::Key_Tab && m_tabSpaces > 0) {
		QTextCursor cursor = this->textCursor();
		int l = m_tabSpaces - (cursor.columnNumber() % m_tabSpaces);
		if (l == 0) l = m_tabSpaces;
		this->insertPlainText(QString(l, ' '));
	}
	else if ((_event->key() == Qt::Key_Return) && m_newLineSamePrefix) {
		QTextCursor cursor = this->textCursor();
		QString prefix;
		cursor.movePosition(QTextCursor::StartOfLine);
		while (this->document()->characterAt(cursor.position()) == QChar(' ') || this->document()->characterAt(cursor.position()) == QChar('\t')) {
			prefix.append(this->document()->characterAt(cursor.position()));
			cursor.setPosition(cursor.position() + 1);
		}

		this->insertPlainText("\n" + prefix);
	}
	else if (_event->key() == Qt::Key_Home && !(_event->modifiers() & Qt::ControlModifier)) {
		QTextCursor cursor = this->textCursor();
		int col = cursor.columnNumber();
		int p = cursor.position();
		cursor.movePosition(QTextCursor::StartOfLine);
		// Skip leading spaces and tabs
		while (this->document()->characterAt(cursor.position()) == QChar(' ') || this->document()->characterAt(cursor.position()) == QChar('\t')) {
			cursor.setPosition(cursor.position() + 1);
		}
		// If the position did not change move to start of line
		if (col == cursor.columnNumber()) {
			cursor.movePosition(QTextCursor::StartOfLine);
		}
		// Check if the shift modifier is pressed
		if (_event->modifiers() & Qt::ShiftModifier) {
			int np = cursor.position();
			if (p != np) {
				cursor = this->textCursor();
			}
			while (p != np) {
				this->keyPressEvent(new QKeyEvent(QEvent::KeyPress, (p < np ? Qt::Key_Right : Qt::Key_Left), Qt::ShiftModifier, _event->text(), _event->isAutoRepeat(), _event->count()));
				cursor = this->textCursor();
				p = cursor.position();
			}
		}
		this->setTextCursor(cursor);
	}
	else {
		QPlainTextEdit::keyPressEvent(_event);
	}
}

void ot::TextEditor::resizeEvent(QResizeEvent * _event) {
	PlainTextEdit::resizeEvent(_event);

	QRect cr = contentsRect();
	m_lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));

	this->slotUpdateShowMorePosition();
}

void ot::TextEditor::wheelEvent(QWheelEvent* _event) {
	if (_event->modifiers() & Qt::ControlModifier) {
		int delta = _event->angleDelta().y();
		if (delta > 0) {
			zoomIn();
		}
		else {
			zoomOut();
		}
		_event->accept();
	}
	else {
		QPlainTextEdit::wheelEvent(_event);
	}
}

void ot::TextEditor::showEvent(QShowEvent* _event) {
	QPlainTextEdit::showEvent(_event);
	this->slotUpdateShowMorePosition();
}

// ###################################################################################################################################

// Private: Slots

void ot::TextEditor::slotUpdateLineNumberAreaWidth(int _newBlockCount) {
	setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void ot::TextEditor::slotHighlightCurrentLine() {
	QList<QTextEdit::ExtraSelection> extraSelections;
	this->getCurrentLineSelection(extraSelections);
	this->addAdditionalSelections(extraSelections);
}

void ot::TextEditor::slotUpdateLineNumberArea(const QRect & _rect, int _dy) {
	if (_dy) m_lineNumberArea->scroll(0, _dy);
	else m_lineNumberArea->update(0, _rect.y(), m_lineNumberArea->width(), _rect.height());

	if (_rect.contains(viewport()->rect())) slotUpdateLineNumberAreaWidth(0);
}

void ot::TextEditor::slotFindRequested() {
	if (!m_searchPopup) {
		m_searchPopup = new TextEditorSearchPopup(this);
		m_searchPopup->updatePosition(true);
		m_searchPopup->setVisible(true);
		
		this->connect(m_searchPopup, &TextEditorSearchPopup::popupClosing, this, &TextEditor::slotFindClosing);
	}

	QString selectedText = this->textCursor().selectedText();
	int ix = this->textCursor().position() - 1;
	if (ix < 0) {
		this->textCursor().movePosition(QTextCursor::End);
		ix = this->textCursor().position();
	}
	m_searchPopup->setIndex(ix);

	this->clearFocus();
	m_searchPopup->focusInput();

	if (!selectedText.isEmpty()) {
		m_searchPopup->setCurrentText(selectedText);
	}
}

void ot::TextEditor::slotFindClosing() {
	m_searchPopup = nullptr;
	this->setFocus();
}

void ot::TextEditor::slotDuplicateLine() {
	if (!m_enableDuplicateLineShortcut) return;
	QTextCursor cursor = this->textCursor();
	int column = cursor.columnNumber();
	cursor.select(QTextCursor::LineUnderCursor);
	QString txt = cursor.selectedText();
	cursor.clearSelection();
	cursor.movePosition(QTextCursor::EndOfLine);
	this->setTextCursor(cursor);
	this->insertPlainText("\n" + txt);
}

void ot::TextEditor::slotCurrentColorStyleChanged() {
	this->slotHighlightCurrentLine();
	this->updateDocumentSyntax();
}

void ot::TextEditor::slotSelectionChanged() {
	if (!m_enableSameTextHighlighting) return;
	this->slotHighlightCurrentLine();
}

void ot::TextEditor::slotShowMore() {
	if (m_loadedChunkCount < m_textChunks.size()) {
		QSignalBlocker sigBlock(this);

		auto cursor = this->textCursor();
		cursor.movePosition(QTextCursor::End);
		cursor.insertText(m_textChunks[m_loadedChunkCount].toString());
		this->setTextCursor(cursor);

		m_loadedChunkCount++;

		if (m_loadedChunkCount >= m_textChunks.size()) {
			m_showMoreLabel->hide();
		}

		this->document()->clearUndoRedoStacks();
		this->document()->setModified(false);

		slotUpdateShowMorePosition();
	}
	else {
		m_showMoreLabel->hide();
	}
}

void ot::TextEditor::slotUpdateShowMorePosition() {
	QRect r = this->viewport()->rect();
	QPoint bottomRight = r.bottomRight();
	int y = bottomRight.y() - m_showMoreLabel->height() - 8;
	int x = bottomRight.x() - (m_showMoreLabel->width() + 10);
	m_showMoreLabel->move(x, y);
	if (this->isVisible()) {
		m_showMoreLabel->show();
	}
	m_showMoreLabel->raise();
}

// ###################################################################################################################################

// Private: Helper

void ot::TextEditor::getCurrentLineSelection(QList<QTextEdit::ExtraSelection>& _selections) {
	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

		selection.format.setProperty(QTextFormat::OutlinePen, QPen(cs.getValue(ColorStyleValueEntry::TextEditorLineBorder).toBrush(), 1.));
		selection.format.setProperty(QTextFormat::CssFloat, QVariant());
		selection.format.setProperty(QTextFormat::FullWidthSelection, true);
		selection.cursor = textCursor();
		selection.cursor.clearSelection();
		_selections.append(selection);
	}
}

void ot::TextEditor::addAdditionalSelections(QList<QTextEdit::ExtraSelection>& _selections) {
	QTextCursor cursor = textCursor();
	QString selectedText = cursor.selectedText();
	if (!selectedText.isEmpty() && selectedText.length() >= m_sameTextHighlightingMinimum) {
		QString emptyTest = selectedText;
		emptyTest.remove(' ');
		emptyTest.remove('\t');
		emptyTest.remove('\n');
		if (emptyTest.isEmpty()) return;

		this->blockSignals(true);
		int cursorPos = cursor.position();
		QBrush hb = GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::TextEditorHighlightBackground).toBrush();

		// Highlight the selected text
		QRegularExpression regex(selectedText, QRegularExpression::PatternOption::CaseInsensitiveOption);
		cursor.movePosition(QTextCursor::Start);
		
		while (!cursor.atEnd()) {
			cursor = document()->find(regex, cursor);
			if (!cursor.isNull()) {
				QTextEdit::ExtraSelection selection;

				selection.format.setBackground(hb);
				selection.cursor = cursor;
				//selection.cursor.clearSelection();
				_selections.append(selection);

				cursor.movePosition(QTextCursor::WordRight, QTextCursor::KeepAnchor);
			}
			else {
				break;
			}
		}
		cursor.setPosition(cursorPos);
		this->blockSignals(false);
	}
	this->setExtraSelections(_selections);
}

void ot::TextEditor::updateDocumentSyntax() {
	if (!m_syntaxHighlighter) {
		m_syntaxHighlighter = new SyntaxHighlighter(this->document());
	}
	m_syntaxHighlighter->setRules(DefaultSyntaxHighlighterRules::create(m_documentSyntax));
	m_syntaxHighlighter->rehighlight();
}

void ot::TextEditor::splitTextBufferIntoChunks() {
	m_textChunks.clear();
	const int totalLength = m_textBuffer.size();
	const int approxChunkSize = 100000;
	//const int approxChunkSize = 10;

	int start = 0;
	while (start < totalLength) {
		int end = qMin(start + approxChunkSize, totalLength);

		// Try to find a word boundary going backwards
		if (end < totalLength) {
			while (end > start && !m_textBuffer.at(end - 1).isSpace()) {
				end--;
			}

			// If no space found (very long word), just break hard
			if (end == start) {
				end = qMin(start + approxChunkSize, totalLength);
			}
		}

		m_textChunks.append(QStringView(m_textBuffer).mid(start, end - start));
		start = end;
	}

	m_loadedChunkCount = 0;
}
