// OpenTwin header
#include "OTBlockEditorAPI/TextBlockLayerConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_Text "Text"
#define OT_JSON_MEMBER_TextFont "TextFont"
#define OT_JSON_MEMBER_TextColor "TextColor"

ot::TextBlockLayerConfiguration::TextBlockLayerConfiguration(const std::string& _text, const ot::Color& _textColor) 
	: m_text(_text), m_textColor(_textColor) {}

ot::TextBlockLayerConfiguration::~TextBlockLayerConfiguration() {}

void ot::TextBlockLayerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockLayerConfiguration::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Text, m_text);

	OT_rJSON_createValueObject(fontObj);
	OT_rJSON_createValueObject(colorObj);
	
	m_textFont.addToJsonObject(_document, fontObj);
	m_textColor.addToJsonObject(_document, colorObj);
	
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextFont, fontObj);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_TextColor, colorObj);
}

void ot::TextBlockLayerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockLayerConfiguration::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Text, String);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextFont, Object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_TextColor, Object);

	m_text = _object[OT_JSON_MEMBER_Text].GetString();

	OT_rJSON_val fontObj = _object[OT_JSON_MEMBER_TextFont].GetObject();
	OT_rJSON_val colorObj = _object[OT_JSON_MEMBER_TextColor].GetObject();

	m_textFont.setFromJsonObject(fontObj);
	m_textColor.setFromJsonObject(colorObj);
}