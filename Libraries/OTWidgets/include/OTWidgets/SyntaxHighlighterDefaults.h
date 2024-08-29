//! @file SyntaxHighlighterDefaults.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTWidgets/SyntaxHighlighter.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT SyntaxHighlighterDefaults {
		OT_DECL_NOCOPY(SyntaxHighlighterDefaults)
	public:
		static SyntaxHighlighter* create(DocumentSyntax _syntax, QTextDocument* _document);

		static void setupPython(SyntaxHighlighter* _highlighter);

	private:
		SyntaxHighlighterDefaults() {};
		~SyntaxHighlighterDefaults() {};
	};

}