// OpenTwin header
#include "OTBlockEditorAPI/ImageBlockLayerConfiguration.h"
#include "OpenTwinCore/rJSONHelper.h"

#define OT_JSON_MEMBER_ImagePath "ImagePath"

ot::ImageBlockLayerConfiguration::ImageBlockLayerConfiguration(const std::string& _imageSubPath) : m_imageSubPath(_imageSubPath) {}

ot::ImageBlockLayerConfiguration::~ImageBlockLayerConfiguration() {}

void ot::ImageBlockLayerConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	BlockLayerConfiguration::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ImagePath, m_imageSubPath);
}

void ot::ImageBlockLayerConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockLayerConfiguration::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ImagePath, String);
	m_imageSubPath = _object[OT_JSON_MEMBER_ImagePath].GetString();
}