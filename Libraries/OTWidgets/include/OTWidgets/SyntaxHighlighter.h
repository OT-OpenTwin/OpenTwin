// @otlicense
// File: SyntaxHighlighter.h
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

#pragma once

// OpenTwin header
#include "OTGui/SyntaxHighlighterRule.h"
#include "OTWidgets/WidgetTypes.h"

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