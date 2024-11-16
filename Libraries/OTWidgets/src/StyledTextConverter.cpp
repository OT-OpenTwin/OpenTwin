//! @file StyledTextConverter.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/Painter2D.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/StyledTextConverter.h"

// std header
#include <sstream>

QString ot::StyledTextConverter::toHtml(const StyledTextBuilder& _builder) {
	QString result("<p>");
	
	for (const StyledTextEntry& entry : _builder.getEntries()) {
		if (entry.getStyle().hasStyleSet()) {
			StyledTextConverter::addHtmlSpan(entry.getStyle(), result, QString::fromStdString(entry.getText()));
		}
		else {
			result = result % QString::fromStdString(entry.getText());
		}
	}

	result = result % "</p>";
	return result;
}

void ot::StyledTextConverter::addHtmlSpan(const StyledTextStyle& _style, QString& _destination, const QString& _text) {
	_destination = _destination % "<span style=\"";
	_destination = _destination % StyledTextConverter::getColorFromReference(_style.getColorReference());
	if (_style.getBold()) {
		_destination = _destination % "font-weight:bold;";
	}
	if (_style.getItalic()) {
		_destination = _destination % "font-style:italic;";
	}
	_destination = _destination % "\">" % _text % "</span>";
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
