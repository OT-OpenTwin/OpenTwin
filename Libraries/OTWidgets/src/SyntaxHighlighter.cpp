//! @file SyntaxHighlighter.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/SyntaxHighlighter.h"

ot::SyntaxHighlighter::SyntaxHighlighter(QTextDocument* _document)
	: QSyntaxHighlighter(_document)
{

}

ot::SyntaxHighlighter::~SyntaxHighlighter() {

}

void ot::SyntaxHighlighter::highlightBlock(const QString& text) {

}
