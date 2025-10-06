//! @file StyledTextConverter.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/String.h"
#include "OTGui/Painter2D.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/StyledTextConverter.h"

// Qt header
#include <QtCore/qdatetime.h>

// std header
#include <sstream>

QString ot::StyledTextConverter::toHtml(const StyledTextBuilder& _builder, bool _silent) {
	QString result("<p>");

	bool evalTokens = _builder.getFlags() & StyledTextBuilder::EvaluateSubstitutionTokens;
	
	for (const StyledTextEntry& entry : _builder.getEntries()) {
		std::list<std::string> lst = ot::String::split(entry.getText(), "\n");
		bool first = true;
		for (const std::string& text : lst) {
			if (!first) {
				result = result % "</p>\n<p>";
			}

			QString finalText;
			if (evalTokens) {
				finalText = StyledTextConverter::evaluateSubstitutionTokens(text, _silent);
			}
			else {
				finalText = QString::fromStdString(text);
			}

			if (entry.getStyle().hasStyleSet()) {
				StyledTextConverter::addHtmlSpan(entry.getStyle(), result, finalText, _silent);
			}
			else {
				result = result % finalText;
			}
			first = false;
		}
	}

	result = result % "</p>";
	return result;
}

void ot::StyledTextConverter::addHtmlSpan(const StyledTextStyle& _style, QString& _destination, const QString& _text, bool _silent) {
	// Add custom closing tags for text size
	QString customClose;
	switch (_style.getTextSize()) {
	case ot::StyledTextStyle::TextSize::Regular: break;
	case ot::StyledTextStyle::TextSize::Header1:
		_destination = _destination % "<h1>";
		customClose = "</h1>";
		break;

	case ot::StyledTextStyle::TextSize::Header2:
		_destination = _destination % "<h2>";
		customClose = "</h2>";
		break;

	case ot::StyledTextStyle::TextSize::Header3:
		_destination = _destination % "<h3>";
		customClose = "</h3>";
		break;

	default:
		OTAssert(0, "Unknown text size");
		if (!_silent) {
			OT_LOG_E("Unknown TextSize (" + std::to_string((int)_style.getTextSize()) + ")");
		}
		break;
	}

	// Add custom closing tags for underline and line trough
	if (_style.getUnderline()) {
		_destination = _destination % "<u>";
		customClose.append("</u>");
	}
	if (_style.getLineTrough()) {
		_destination = _destination % "<s>";
		customClose.append("</s>");
	}

	// Add span if needed
	if (_style.getColorReference() != StyledText::Default || _style.getBold() || _style.getItalic()) {
		_destination = _destination % "<span style=\"";
		_destination = _destination % StyledTextConverter::getColorFromReference(_style.getColorReference(), _silent);
		if (_style.getBold()) {
			_destination = _destination % "font-weight:bold;";
		}
		if (_style.getItalic()) {
			_destination = _destination % "font-style:italic;";
		}
		_destination = _destination % "\">";
		customClose.append("</span>");
	}
	
	_destination = _destination % _text % customClose;
}

QString ot::StyledTextConverter::getColorFromReference(StyledText::ColorReference _colorReference, bool _silent) {
	ColorStyleValueEntry colorStyleValue = ColorStyleValueEntry::Transparent;

	switch (_colorReference) {
	case ot::StyledText::Default:
		return QString();

	case ot::StyledText::Highlight:
		colorStyleValue = ColorStyleValueEntry::StyledTextHighlight;
		break;

	case ot::StyledText::LightHighlight:
		colorStyleValue = ColorStyleValueEntry::StyledTextLightHighlight;
		break;

	case ot::StyledText::Comment:
		colorStyleValue = ColorStyleValueEntry::StyledTextComment;
		break;

	case ot::StyledText::Warning:
		colorStyleValue = ColorStyleValueEntry::StyledTextWarning;
		break;

	case ot::StyledText::Error:
		colorStyleValue = ColorStyleValueEntry::StyledTextError;
		break;

	default:
		OTAssert(0, "Unknown color reference");
		if (!_silent) {
			OT_LOG_E("Unknown ColorReference (" + std::to_string((int)_colorReference) + ")");
		}
		return QString();
	}

	if (!GlobalColorStyle::instance().getCurrentStyle().hasValue(colorStyleValue)) {
		OTAssert(0, "Unknown color style value");
		if (!_silent) {
			OT_LOG_E("ColorStyleValue \"" + toString(colorStyleValue) + "\" is not set in the current color style \"" + GlobalColorStyle::instance().getCurrentStyleName() + "\"");
		}
		return QString();
	}
	else {
		ot::Color color = GlobalColorStyle::instance().getCurrentStyle().getValue(colorStyleValue).painter()->getDefaultColor();
		std::ostringstream oss;
		oss << "color: #"
			<< std::setw(2) << std::setfill('0') << std::hex << color.a()
			<< std::setw(2) << std::setfill('0') << std::hex << color.r()
			<< std::setw(2) << std::setfill('0') << std::hex << color.g()
			<< std::setw(2) << std::setfill('0') << std::hex << color.b()
			<< ";";
		return QString::fromStdString(oss.str());
	}
}

QString ot::StyledTextConverter::evaluateSubstitutionTokens(const std::string& _text, bool _silent) {
	QString result;
	size_t ix = 0;
	size_t startIx = _text.find("$(", ix);

	while (startIx != std::string::npos) {
		// Append any text before the token
		if (startIx > ix) {
			result += QString::fromStdString(_text.substr(ix, startIx - ix));
			ix = startIx + 1;
		}

		size_t endIx = _text.find(")", startIx);
		if (endIx == std::string::npos) {
			// Unterminated token, append the rest as-is and break
			result += QString::fromStdString(_text.substr(ix + 1));
			break;
		}

		std::string tokenStr = _text.substr(startIx + 2, endIx - startIx - 2);
		StyledText::SubstitutionToken token = StyledText::stringToSubstitutionToken(tokenStr);

		QString replacement;
		switch (token) {
		case ot::StyledText::EmptyToken:
			replacement = "";
			break;

		case ot::StyledText::DateYYYYMMDD:
		{
			QDateTime dt = QDateTime::currentDateTime();
			replacement = dt.toString("yyyy-MM-dd");
			break;
		}
		case ot::StyledText::TimeHHMMSS:
		{
			QDateTime dt = QDateTime::currentDateTime();
			replacement = dt.toString("HH:mm:ss");
			break;
		}
		case ot::StyledText::TimeHHMMSSZZZZ:
		{
			QDateTime dt = QDateTime::currentDateTime();
			replacement = dt.toString("HH:mm:ss.zzz");
			break;
		}
		default:
			// Unknown token, keep it as-is
			OTAssert(0, "Unknown substitution token");
			if (!_silent) {
				OT_LOG_E("Unknown text token (" + std::to_string(static_cast<int>(token)) + ")");
			}
			replacement = QString::fromStdString(_text.substr(startIx, endIx - startIx + 1));
			break;
		}

		result += replacement;
		ix = endIx + 1;
		startIx = _text.find("$(", ix);
	}

	// Append any remaining text after last token
	if (ix < _text.length()) {
		result += QString::fromStdString(_text.substr(ix));
	}

	return result;
}