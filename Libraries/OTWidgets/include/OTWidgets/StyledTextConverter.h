//! @file StyledTextConverter.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/StyledTextBuilder.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT StyledTextConverter {
	public:
		static QString toHtml(const StyledTextBuilder& _builder);

	private:
		static void addHtmlSpan(const StyledTextStyle& _style, QString& _destination, const QString& _text);
		static QString getColorFromReference(StyledText::ColorReference _colorReference);
		static QString evaluateSubstitutionTokens(const std::string& _text);

		StyledTextConverter() = delete;
	};

}