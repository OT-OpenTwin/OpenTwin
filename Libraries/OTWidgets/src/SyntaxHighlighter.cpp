//! @file SyntaxHighlighter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/SyntaxHighlighter.h"

// Qt header
#include <QtCore/qregularexpression.h>

ot::SyntaxHighlighter::SyntaxHighlighter(QTextDocument* _document)
	: QSyntaxHighlighter(_document)
{

}

ot::SyntaxHighlighter::~SyntaxHighlighter() {

}

void ot::SyntaxHighlighter::addRule(const SyntaxHighlighterRule& _rule) {
	m_rules.push_back(_rule);
}

void ot::SyntaxHighlighter::addRules(const std::list<SyntaxHighlighterRule>& _rules) {
	for (const SyntaxHighlighterRule& rule : _rules) {
		m_rules.push_back(rule);
	}
}

void ot::SyntaxHighlighter::highlightBlock(const QString& text) {
	for (const SyntaxHighlighterRule& rule : m_rules) {
		// Create regex
		QRegularExpression regex(QString::fromStdString(rule.getRegularExpression()));
		
		// Create format
		QTextCharFormat format;
		format.setForeground(QtFactory::toQBrush(rule.getPainter()));
		format.setFontItalic(rule.getFont().isItalic());
		format.setFontWeight((rule.getFont().isBold() ? QFont::Bold : QFont::Normal));

		// Parse
		QRegularExpressionMatchIterator matchIterator = regex.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			this->setFormat(match.capturedStart(), match.capturedLength(), format);
		}
	}
}
