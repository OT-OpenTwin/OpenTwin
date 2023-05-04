// OpenTwin header
#include "OTBlockEditorAPI/TextBlockLayerConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_Text "Text"
#define OT_JSON_MEMBER_TextSize "TextSize"
#define OT_JSON_MEMBER_TextColor "TextColor"

ot::TextBlockLayerConfiguration::TextBlockLayerConfiguration(const std::string& _text, int _textSize, const ot::Color& _textColor) 
	: m_text(_text), m_textPixelSize(_textSize), m_textColor(_textColor) {}

ot::TextBlockLayerConfiguration::~TextBlockLayerConfiguration() {}

void ot::TextBlockLayerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockLayerConfiguration::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Text, m_text);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextSize, m_textPixelSize);
	OT_rJSON_createValueObject(colorObj);
	m_textColor.addToJsonObject(_document, colorObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextColor, colorObj);
}

void ot::TextBlockLayerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockLayerConfiguration::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Text, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextSize, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextColor, Object);

	m_text = _object[OT_JSON_MEMBER_Text].GetString();
	m_textPixelSize = _object[OT_JSON_MEMBER_TextSize].GetInt();
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_TextColor].GetObject();
}