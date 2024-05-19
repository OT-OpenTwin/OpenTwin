//! @file TextEditor.cpp
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/ColorStyleTypes.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/TextEditorSearchPopup.h"

// Qt header
#include <QtGui/qevent.h>
#include <QtGui/qpainter.h>
#include <QtGui/qtextdocument.h>
#include <QtGui/qtextlist.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qshortcut.h>
#include <QtWidgets/qscrollbar.h>

/*

ot::TextEditorSyntaxHighlighterTest::TextEditorSyntaxHighlighterTest(TextEditor * _editor)
	: QSyntaxHighlighter(_editor->document())
{
	HighlightingRule rule;

	m_keywordFormat.setForeground(Qt::GlobalColor::blue);
	m_keywordFormat.setFontWeight(QFont::Bold);
	const QString keywordPatterns[] = {
		QStringLiteral("\\bchar\\b"), QStringLiteral("\\bclass\\b"), QStringLiteral("\\bconst\\b"),
		QStringLiteral("\\bdouble\\b"), QStringLiteral("\\benum\\b"), QStringLiteral("\\bexplicit\\b"),
		QStringLiteral("\\bfriend\\b"), QStringLiteral("\\binline\\b"), QStringLiteral("\\bint\\b"),
		QStringLiteral("\\blong\\b"), QStringLiteral("\\bnamespace\\b"), QStringLiteral("\\boperator\\b"),
		QStringLiteral("\\bprivate\\b"), QStringLiteral("\\bprotected\\b"), QStringLiteral("\\bpublic\\b"),
		QStringLiteral("\\bshort\\b"), QStringLiteral("\\bsignals\\b"), QStringLiteral("\\bsigned\\b"),
		QStringLiteral("\\bslots\\b"), QStringLiteral("\\bstatic\\b"), QStringLiteral("\\bstruct\\b"),
		QStringLiteral("\\btemplate\\b"), QStringLiteral("\\btypedef\\b"), QStringLiteral("\\btypename\\b"),
		QStringLiteral("\\bunion\\b"), QStringLiteral("\\bunsigned\\b"), QStringLiteral("\\bvirtual\\b"),
		QStringLiteral("\\bvoid\\b"), QStringLiteral("\\bvolatile\\b"), QStringLiteral("\\bbool\\b")
	};
	for (const QString &pattern : keywordPatterns) {
		rule.pattern = QRegularExpression(pattern);
		rule.format = m_keywordFormat;
		m_highlightingRules.append(rule);
	}

	m_classFormat.setFontWeight(QFont::Bold);
	m_classFormat.setForeground(Qt::darkMagenta);
	rule.pattern = QRegularExpression(QStringLiteral("\\bQ[A-Za-z]+\\b"));
	rule.format = m_classFormat;
	m_highlightingRules.append(rule);

	m_quotationFormat.setForeground(Qt::darkGreen);
	rule.pattern = QRegularExpression(QStringLiteral("\".*\""));
	rule.format = m_quotationFormat;
	m_highlightingRules.append(rule);

	m_functionFormat.setFontItalic(true);
	m_functionFormat.setForeground(Qt::blue);
	rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
	rule.format = m_functionFormat;
	m_highlightingRules.append(rule);

	m_singleLineCommentFormat.setForeground(Qt::green);
	rule.pattern = QRegularExpression(QStringLiteral("//[^\n]*"));
	rule.format = m_singleLineCommentFormat;
	m_highlightingRules.append(rule);

	m_multiLineCommentFormat.setForeground(Qt::green);
	m_commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
	m_commentEndExpression = QRegularExpression(QStringLiteral("\\* /"));  <- no space
}

*/

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
	: PlainTextEdit(_parent), m_syntaxHighlighter(nullptr), m_contentChanged(false), m_searchPopup(nullptr), 
	m_tabSpaces(4), m_newLineSamePrefix(false), m_enableDuplicateLineShortcut(false), m_enableSameTextHighlighting(false),
	m_sameTextHighlightingMinimum(2)
{
	this->setObjectName("OT_TextEditor");

	m_lineNumberArea = new TextEditorLineNumberArea(this);

	QFont f = font();
	f.setFixedPitch(true);
	f.setFamily("Consolas");
	setFont(f);
	QFontMetrics fm(f);
	this->setTabStopDistance(4);

	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

	QShortcut* saveShortcut = new QShortcut(QKeySequence("Ctrl+S"), this);
	QShortcut* findShortcut = new QShortcut(QKeySequence("Ctrl+F"), this);
	QShortcut* duplicateShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
	saveShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	findShortcut->setContext(Qt::WidgetWithChildrenShortcut);
	duplicateShortcut->setContext(Qt::WidgetWithChildrenShortcut);

	this->slotHighlightCurrentLine();

	connect(this, &TextEditor::blockCountChanged, this, &TextEditor::slotUpdateLineNumberAreaWidth);
	connect(this, &TextEditor::updateRequest, this, &TextEditor::slotUpdateLineNumberArea);
	connect(this, &TextEditor::cursorPositionChanged, this, &TextEditor::slotHighlightCurrentLine);
	connect(this, &TextEditor::textChanged, this, &TextEditor::slotTextChanged);
	connect(this, &TextEditor::selectionChanged, this, &TextEditor::slotSelectionChanged);
	connect(saveShortcut, &QShortcut::activated, this, &TextEditor::slotSaveRequested);
	connect(findShortcut, &QShortcut::activated, this, &TextEditor::slotFindRequested);
	connect(duplicateShortcut, &QShortcut::activated, this, &TextEditor::slotDuplicateLine);
	connect(&GlobalColorStyle::instance(), &GlobalColorStyle::currentStyleChanged, this, &TextEditor::slotCurrentColorStyleChanged);
}

ot::TextEditor::~TextEditor() {

}

int ot::TextEditor::lineNumberAreaWidth(void) const {
	int digits = 1;
	int max = qMax(1, blockCount());
	while (max >= 10) {
		max /= 10;
		++digits;
	}
	digits += 2;

	int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

	return space;
}

void ot::TextEditor::lineNumberAreaPaintEvent(QPaintEvent * _event) {
	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	QPainter painter(m_lineNumberArea);
	QBrush backBrush = cs.getValue(OT_COLORSTYLE_VALUE_TitleBackground).brush();
	QBrush frontBrush = cs.getValue(OT_COLORSTYLE_VALUE_TitleForeground).brush();
	QBrush borderBrush = cs.getValue(OT_COLORSTYLE_VALUE_ControlsBorderColor).brush();
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

void ot::TextEditor::setCode(const QString& _text) {
	setPlainText(_text);
	document()->clearUndoRedoStacks();
}

void ot::TextEditor::setCode(const QStringList& _lines) {
	clear();
	for (auto l : _lines) appendPlainText(l);
	document()->clearUndoRedoStacks();
}

QStringList ot::TextEditor::code(void) const {
	return toPlainText().split("\n", Qt::KeepEmptyParts);
}

void ot::TextEditor::setSyntaxHighlighter(QSyntaxHighlighter * _highlighter) {
	if (m_syntaxHighlighter) delete m_syntaxHighlighter;
	m_syntaxHighlighter = _highlighter;
}

bool ot::TextEditor::requiresRefreshing(ot::UID displayedTextEntityID, ot::UID displayedTextEntityVersion)
{
	bool refreshingNeeded = displayedTextEntityID != m_displayedTextEntityID || m_displayedTextEntityVersion != displayedTextEntityVersion;
	m_displayedTextEntityID = displayedTextEntityID;
	m_displayedTextEntityVersion = displayedTextEntityVersion;
	return refreshingNeeded;
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

void ot::TextEditor::slotSaveRequested(void) {
	if (!m_contentChanged) return;
	Q_EMIT saveRequested();
}

void ot::TextEditor::slotTextChanged(void) {
	m_contentChanged = true;
}

void ot::TextEditor::slotFindRequested(void) {
	if (!m_searchPopup) {
		m_searchPopup = new TextEditorSearchPopup(this);
		m_searchPopup->updatePosition(true);
		m_searchPopup->setVisible(true);
		
		this->connect(m_searchPopup, &TextEditorSearchPopup::popupClosing, this, &TextEditor::slotFindClosing);
	}
	
	m_searchPopup->setIndex(this->textCursor().position());
	m_searchPopup->focusInput();
}

void ot::TextEditor::slotFindClosing(void) {
	m_searchPopup = nullptr;
	this->setFocus();
}

void ot::TextEditor::slotDuplicateLine(void) {
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

void ot::TextEditor::slotCurrentColorStyleChanged(const ot::ColorStyle& _style) {
	this->slotHighlightCurrentLine();
}

void ot::TextEditor::slotSelectionChanged(void) {
	if (!m_enableSameTextHighlighting) return;
	this->slotHighlightCurrentLine();
}

void ot::TextEditor::getCurrentLineSelection(QList<QTextEdit::ExtraSelection>& _selections) {
	if (!isReadOnly()) {
		QTextEdit::ExtraSelection selection;

		const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();

		selection.format.setProperty(QTextFormat::OutlinePen, QPen(cs.getValue(OT_COLORSTYLE_VALUE_TextEditHighlightBorder).brush(), 1.));
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
		QBrush hb = GlobalColorStyle::instance().getCurrentStyle().getValue(OT_COLORSTYLE_VALUE_TextEditHighlight).brush();

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