//! @file SyntaxHighlighter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/SyntaxHighlighterRule.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qregularexpression.h>
#include <QtGui/qsyntaxhighlighter.h>

// std header
#include <list>

namespace ot {
	
	class OT_WIDGETS_API_EXPORT SyntaxHighlighter : public QSyntaxHighlighter {
		OT_DECL_NODEFAULT(SyntaxHighlighter)
		OT_DECL_NOCOPY(SyntaxHighlighter)
	public:
		SyntaxHighlighter(QTextDocument* _document);
		virtual ~SyntaxHighlighter();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter

		void addRule(const SyntaxHighlighterRule& _rule);
		void addRules(const std::list<SyntaxHighlighterRule>& _rules);
		void setRules(const std::list<SyntaxHighlighterRule>& _rules);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

	protected:
		virtual void highlightBlock(const QString& text) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		struct RuleEntry {
			QRegularExpression regex;
			QTextCharFormat charFormat;

		};
		std::list<RuleEntry> m_rules;

		RuleEntry toRuleEntry(const SyntaxHighlighterRule& _rule);

	};

}