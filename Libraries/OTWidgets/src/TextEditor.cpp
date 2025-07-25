//! @file TextEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/RuntimeTests.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTGui/DefaultSyntaxHighlighterRules.h"
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
	m_sameTextHighlightingMinimum(2), m_documentSyntax(DocumentSyntax::PlainText), m_fileExtensionFilter(FileExtension::toFilterString(FileExtension::AllFiles))
{
	this->setObjectName("OT_TextEditor");

	m_lineNumberArea = new TextEditorLineNumberArea(this);

	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("Consolas");
	setFont(f);

	QFontMetrics fm(f);
	this->setTabStopDistance(4 * fm.horizontalAdvance(QChar(' ')));

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
			this->setCode(newText);
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

void ot::TextEditor::setCode(const QString& _text) {
	bool tmp = this->signalsBlocked();
	bool tmpSyn = false;
	if (m_syntaxHighlighter) {
		tmpSyn = m_syntaxHighlighter->signalsBlocked();
		m_syntaxHighlighter->blockSignals(true);
	}
	this->blockSignals(true);
		
	this->setPlainText(_text);
	this->document()->clearUndoRedoStacks();
	
	this->setContentSaved();

	this->slotUpdateLineNumberAreaWidth(0);

	if (m_syntaxHighlighter) {
		m_syntaxHighlighter->blockSignals(tmpSyn);
	}
	this->blockSignals(tmp);

	this->setContentChanged();
}

void ot::TextEditor::setCode(const QStringList& _lines) {
	bool tmp = this->signalsBlocked();
	this->blockSignals(true);

	bool tmpSyn = false;
	if (m_syntaxHighlighter) {
		tmpSyn = m_syntaxHighlighter->signalsBlocked();
		m_syntaxHighlighter->blockSignals(true);
	}

	this->clear();
	for (auto l : _lines) this->appendPlainText(l);
	this->document()->clearUndoRedoStacks();

	this->setContentSaved();
	
	this->slotUpdateLineNumberAreaWidth(0);

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

QStringList ot::TextEditor::code() const {
	return this->toPlainText().split("\n", Qt::KeepEmptyParts);
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
