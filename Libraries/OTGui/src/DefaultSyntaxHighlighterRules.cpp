//! @file DefaultSyntaxHighlighterRules.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/StyleRefPainter2D.h"
#include "OTGui/DefaultSyntaxHighlighterRules.h"

std::list<ot::SyntaxHighlighterRule> ot::DefaultSyntaxHighlighterRules::create(DocumentSyntax _syntax) {
	switch (_syntax) {
	case ot::DocumentSyntax::PlainText: return std::list<ot::SyntaxHighlighterRule>();
	case ot::DocumentSyntax::PythonScript: return DefaultSyntaxHighlighterRules::createPython();
	default:
		OT_LOG_EAS("Unknown syntax (" + std::to_string((int)_syntax) + ")");
		return std::list<ot::SyntaxHighlighterRule>();
	}
}

std::list<ot::SyntaxHighlighterRule> ot::DefaultSyntaxHighlighterRules::createPython() {
    std::list<SyntaxHighlighterRule> result;

    Font font;
    font.setBold(true);

    // Classes
    SyntaxHighlighterRule classRule;
    classRule.setFont(font);
    classRule.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::PythonClass));
    classRule.setRegularExpression("\\bclass [A-Za-z]+\\b");
    result.push_back(classRule);

    // Keywords  
    const std::list<std::string> keywordPatterns = {
        "\\bclass\\b", "\\bdef\\b",
        "\\breturn\\b","\\bif\\b",
        "\\belse\\b","\\bwhile\\b",
        "\\bfor\\b","\\bbreak\\b",
        "\\bcontinue\\b","\\bimport\\b",
        "\\bfrom\\b","\\bas\\b",
        "\\bpass\\b","\\braise\\b",
        "\\bwith\\b","\\btry\\b",
        "\\bexcept\\b","\\bfinally\\b",
        "\\blambda\\b","\\bNone\\b",
        "\\bTrue\\b","\\bFalse\\b",
        "\\band\\b","\\bor\\b",
        "\\bnot\\b"
    };

    for (const std::string& pattern : keywordPatterns) {
        SyntaxHighlighterRule newRule;
        newRule.setFont(font);
        newRule.setRegularExpression(pattern);
        newRule.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::PythonKeyword));
        result.push_back(newRule);
    }

    // Strings
    SyntaxHighlighterRule stringRule;
    stringRule.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::PythonString));
    stringRule.setFont(font);
    stringRule.setRegularExpression("\".*\"|'.*'");
    result.push_back(stringRule);

    // Functions
    font.setItalic(true);
    SyntaxHighlighterRule functionRule;
    functionRule.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::PythonFunction));
    functionRule.setFont(font);
    functionRule.setRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    result.push_back(functionRule);

    // Single-line comments
    font.setBold(false);
    font.setItalic(false);
    SyntaxHighlighterRule singleLineCommentRule;
    singleLineCommentRule.setFont(font);
    singleLineCommentRule.setPainter(new StyleRefPainter2D(ColorStyleValueEntry::PythonComment));
    singleLineCommentRule.setRegularExpression("\\bQ[A-Za-z]+\\b");
    result.push_back(singleLineCommentRule);

    // Multi-line comments
    SyntaxHighlighterRule multilineCommentRule = singleLineCommentRule;
    multilineCommentRule.setRegularExpression("#[^\n]*");
    result.push_back(multilineCommentRule);

	return result;
}
