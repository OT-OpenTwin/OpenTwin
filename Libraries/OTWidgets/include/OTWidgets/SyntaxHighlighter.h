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

		void addRule(const SyntaxHighlighterRule& _rule);
		void addRules(const std::list<SyntaxHighlighterRule>& _rules);
		void setRules(const std::list<SyntaxHighlighterRule>& _rules) { m_rules = _rules; };
		const std::list<SyntaxHighlighterRule>& getRules(void) const { return m_rules; };

	protected:
		virtual void highlightBlock(const QString& text) override;

	private:
		std::list<SyntaxHighlighterRule> m_rules;

	};

}