//! @file SyntaxHighlighter.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qsyntaxhighlighter.h>

namespace ot {
	
	class OT_WIDGETS_API_EXPORT SyntaxHighlighter : public QSyntaxHighlighter {
		OT_DECL_NODEFAULT(SyntaxHighlighter)
		OT_DECL_NOCOPY(SyntaxHighlighter)
	public:
		SyntaxHighlighter(QTextDocument* _document);
		virtual ~SyntaxHighlighter();

	protected:
		virtual void highlightBlock(const QString& text) override;
	};

}