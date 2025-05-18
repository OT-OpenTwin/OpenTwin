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
		//! @brief Convert the provided StyledTextBuilder to HTML.
		//! @param _builder Builder to convert.
		//! @param _silent If true no error messages are printed if the conversion fails.
		static QString toHtml(const StyledTextBuilder& _builder, bool _silent = false);

	private:
		static void addHtmlSpan(const StyledTextStyle& _style, QString& _destination, const QString& _text, bool _silent);
		static QString getColorFromReference(StyledText::ColorReference _colorReference, bool _silent);
		static QString evaluateSubstitutionTokens(const std::string& _text, bool _silent);

		StyledTextConverter() = delete;
	};

}