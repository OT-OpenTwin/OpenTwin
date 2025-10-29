// @otlicense

//! @file DefaultSyntaxHighlighterRules.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/SyntaxHighlighterRule.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT DefaultSyntaxHighlighterRules {
	public:
		static std::list<SyntaxHighlighterRule> create(DocumentSyntax _syntax);

		static std::list<SyntaxHighlighterRule> createPython();

	private:
		DefaultSyntaxHighlighterRules() {};
		~DefaultSyntaxHighlighterRules() {};
	};

}