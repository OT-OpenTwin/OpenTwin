//! @file StyledTextConverter.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTGui/Painter2D.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/StyledTextConverter.h"

// std header
#include <sstream>

QString ot::StyledTextConverter::toHtml(const StyledTextBuilder& _builder) {
	QString result("<p>");
	
	for (const StyledTextEntry& entry : _builder.getEntries()) {
		std::list<std::string> lst = ot::splitString(entry.getText(), "\n");
		bool first = true;
		for (const std::string& text : lst) {
			if (!first) {
				result = result % "</p>\n<p>";
			}

			if (entry.getStyle().hasStyleSet()) {
				StyledTextConverter::addHtmlSpan(entry.getStyle(), result, QString::fromStdString(text));
			}
			else {
				result = result % QString::fromStdString(text);
			}
			first = false;
		}
	}

	result = result % "</p>";
	return result;
}

void ot::StyledTextConverter::addHtmlSpan(const StyledTextStyle& _style, QString& _destination, const QString& _text) {
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
		OT_LOG_EAS("Unknown TextSize (" + std::to_string((int)_style.getTextSize()) + ")");
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
		_destination = _destination % StyledTextConverter::getColorFromReference(_style.getColorReference());
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

QString ot::StyledTextConverter::getColorFromReference(StyledText::ColorReference _colorReference) {
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
		OT_LOG_EAS("Unknown ColorReference (" + std::to_string((int)_colorReference) + ")");
		return QString();
	}

	if (!GlobalColorStyle::instance().getCurrentStyle().hasValue(colorStyleValue)) {
		OT_LOG_EAS("ColorStyleValue \"" + toString(colorStyleValue) + "\" is not set in the current color style \"" + GlobalColorStyle::instance().getCurrentStyleName() + "\"");
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
