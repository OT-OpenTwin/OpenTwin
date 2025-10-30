// @otlicense

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

// ###########################################################################################################################################################################################################################################################################################################################

// Setter

void ot::SyntaxHighlighter::addRule(const SyntaxHighlighterRule& _rule) {
	m_rules.push_back(std::move(this->toRuleEntry(_rule)));
}

void ot::SyntaxHighlighter::addRules(const std::list<SyntaxHighlighterRule>& _rules) {
	for (const SyntaxHighlighterRule& rule : _rules) {
		m_rules.push_back(std::move(this->toRuleEntry(rule)));
	}
}

void ot::SyntaxHighlighter::setRules(const std::list<SyntaxHighlighterRule>& _rules) {
	m_rules.clear();
	this->addRules(_rules);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Virtual methods

void ot::SyntaxHighlighter::highlightBlock(const QString& text) {
	for (const RuleEntry& rule : m_rules) {
		// Parse
		QRegularExpressionMatchIterator matchIterator = rule.regex.globalMatch(text);
		while (matchIterator.hasNext()) {
			QRegularExpressionMatch match = matchIterator.next();
			this->setFormat(match.capturedStart(), match.capturedLength(), rule.charFormat);
		}
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

ot::SyntaxHighlighter::RuleEntry ot::SyntaxHighlighter::toRuleEntry(const SyntaxHighlighterRule& _rule) {
	RuleEntry newEntry;
	// Create regex
	newEntry.regex = QRegularExpression(QString::fromStdString(_rule.getRegularExpression()));

	// Create format
	newEntry.charFormat.setForeground(QtFactory::toQBrush(_rule.getPainter()));
	newEntry.charFormat.setFontItalic(_rule.getFont().isItalic());
	newEntry.charFormat.setFontWeight((_rule.getFont().isBold() ? QFont::Bold : QFont::Normal));

	return newEntry;
}
