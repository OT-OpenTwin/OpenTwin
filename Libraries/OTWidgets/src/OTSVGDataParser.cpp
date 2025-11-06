// @otlicense
// File: OTSVGDataParser.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTWidgets/OTSVGDataParser.h"
#include "OTWidgets/GlobalColorStyle.h"

// std header
#include <map>
#include <list>

QByteArray ot::OTSVGDataParser::parse(const QByteArray& _rawData) {
	return parse(_rawData, GlobalColorStyle::instance().getCurrentStyle());
}
QByteArray ot::OTSVGDataParser::parse(const QByteArray& _rawData, const ColorStyle& _style) {
	QByteArray processedData = _rawData;

	std::map<QByteArray, QByteArray> replacements;
	QByteArray gradientsData;

	int defIx = 0;
	processPlaceholders(processedData, _style, PlaceholderType_Int, replacements, defIx, gradientsData);
	processPlaceholders(processedData, _style, PlaceholderType_Color, replacements, defIx, gradientsData);
	processPlaceholders(processedData, _style, PlaceholderType_Double, replacements, defIx, gradientsData);

	// Apply replacements
	for (const auto& replacement : replacements) {
		processedData.replace(replacement.first, replacement.second);
	}

	// Insert gradients data
	if (!gradientsData.isEmpty()) {
		int svgDefsEndIx = processedData.indexOf("</defs>");
		if (svgDefsEndIx != -1) {
			processedData.insert(svgDefsEndIx, gradientsData);
		}
		else {
			int svgTagIx = processedData.indexOf("<svg>");
			if (svgTagIx != -1) {
				QByteArray defsStart = "<defs>";
				QByteArray defsEnd = "</defs>";
				QByteArray defsContent = defsStart + gradientsData + defsEnd;
				processedData.insert(svgTagIx + 5, defsContent);
			}
			else {
				OT_LOG_EA("Invalid SVG data: svg start tag not found.");
			}
		}
	}

	return processedData;
}

void ot::OTSVGDataParser::processPlaceholders(const QByteArray& _data, const ColorStyle& _style, PlaceholderType _type, std::map<QByteArray, QByteArray>& _replacements, int& _defIx, QByteArray& _gradientsData) {
	QByteArray placeholderPrefix;
	std::string placeholderTypeStr;
	switch (_type) {
	case ot::OTSVGDataParser::PlaceholderType_Int:
		placeholderPrefix = "$(int:";
		placeholderTypeStr = "integer";
		break;
	case ot::OTSVGDataParser::PlaceholderType_Color:
		placeholderPrefix = "$(color:";
		placeholderTypeStr = "color";
		break;
	case ot::OTSVGDataParser::PlaceholderType_Double:
		placeholderPrefix = "$(double:";
		placeholderTypeStr = "double";
		break;
	default:
		OT_LOG_EAS("Unknown placeholder type (" + std::to_string(static_cast<int>(_type)) + ")");
		return;
	}

	int ix = _data.indexOf(placeholderPrefix);
	while (ix != -1) {
		int endIx = _data.indexOf(')', ix);
		if (endIx == -1) {
			OT_LOG_EAS("Invalid " + placeholderTypeStr + " placeholder in SVG data at position " + std::to_string(ix) + ": Closing bracket missing");
			break;
		}

		QByteArray placeholder = _data.mid(ix, endIx - ix + 1);

		if (_replacements.find(placeholder) != _replacements.end()) {
			// Already processed
			ix = _data.indexOf(placeholderPrefix, endIx);
			continue;
		}

		QByteArray key = _data.mid(ix + placeholderPrefix.length(), endIx - (ix + placeholderPrefix.length()));

		switch (_type) {
		case ot::OTSVGDataParser::PlaceholderType_Int:
		{
			int value = _style.getInteger(stringToColorStyleIntegerEntry(key.toStdString()));
			_replacements.emplace(placeholder, QByteArray::number(value));
		}
			break;
		case ot::OTSVGDataParser::PlaceholderType_Color:
		{
			ColorStyleValueEntry entry = stringToColorStyleValueEntry(key.toStdString());
			Painter2D* painter = _style.getValue(entry).painter();
			if (painter) {
				std::string gradientId = "__ot_key_grad_" + std::to_string(_defIx++);
				_gradientsData.append(QByteArray::fromStdString(painter->generateSvgColorString(gradientId)));
				_replacements.emplace(placeholder, QByteArray::fromStdString("url(#" + gradientId + ")"));
			}
			else {
				OT_LOG_W("Painter for color style entry \"" + key.toStdString() + "\" not found.");
			}
		}
			break;
		case ot::OTSVGDataParser::PlaceholderType_Double:
		{
			double value = _style.getDouble(stringToColorStyleDoubleEntry(key.toStdString()));
			_replacements.emplace(placeholder, QByteArray::number(value));
		}
			break;
		default:
			break;
		}

		ix = _data.indexOf(placeholderPrefix, endIx);
	}
}
