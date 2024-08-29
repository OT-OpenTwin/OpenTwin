//! @file SyntaxHighlighterDefaults.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/SyntaxHighlighterDefaults.h"

ot::SyntaxHighlighter* ot::SyntaxHighlighterDefaults::create(DocumentSyntax _syntax, QTextDocument* _document) {
	SyntaxHighlighter* newHighlighter = new SyntaxHighlighter(_document);

	switch (_syntax) {
	case ot::DocumentSyntax::PlainText:
		break;
	case ot::DocumentSyntax::PythonScript:
		SyntaxHighlighterDefaults::setupPython(newHighlighter);
		break;
	default:
		OT_LOG_EAS("Unknown document syntax (" + std::to_string((int)_syntax) + ")");
		break;
	}

	return newHighlighter;
}

void ot::SyntaxHighlighterDefaults::setupPython(SyntaxHighlighter* _highlighter) {

}
