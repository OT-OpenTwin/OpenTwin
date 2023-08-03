// OpenTwin header
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockLayerConfiguration.h"
#include "OTBlockEditorAPI/BlockLayerConfigurationFactory.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OpenTwinCore/otAssert.h"

#define JSON_MEMBER_Title "Title"
#define JSON_MEMBER_Type "Type"
#define JSON_MEMBER_Layers "Layers"

ot::BlockConfiguration::BlockConfiguration(const std::string& _name, const std::string& _title) 
	: BlockConfigurationGraphicsObject(_name), m_title(_title), m_parentCategory(nullptr) 
{

}

ot::BlockConfiguration::~BlockConfiguration() {
	clearLayers();
}

void ot::BlockConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	if (m_layers.empty()) {
		OT_LOG_WA("Exporting invalid block configuration:\n\tNo layers provided");
	}
	BlockConfigurationGraphicsObject::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, JSON_MEMBER_Type, type());
	ot::rJSON::add(_document, _object, JSON_MEMBER_Title, m_title);

	OT_rJSON_createValueArray(layerArr);
	for (auto l : m_layers) {
		OT_rJSON_createValueObject(layerObj);
		l->addToJsonObject(_document, layerObj);
		layerArr.PushBack(layerObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, JSON_MEMBER_Layers, layerArr);
}

void ot::BlockConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	clearLayers();

	BlockConfigurationGraphicsObject::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, JSON_MEMBER_Title, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Type, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Layers, Array);
	m_title = _object[JSON_MEMBER_Title].GetString();

	OT_rJSON_val layerArr = _object[JSON_MEMBER_Layers].GetArray();
	for (rapidjson::SizeType i = 0; i < layerArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(layerArr, i, Object);
		OT_rJSON_val layerObj = layerArr[i].GetObject();
		BlockLayerConfiguration* newLayer = BlockLayerConfigurationFactory::blockLayerConfigurationFromJson(layerObj);
		if (newLayer) m_layers.push_back(newLayer);
	}
}

void ot::BlockConfiguration::addLayer(BlockLayerConfiguration* _layer) {
	otAssert(_layer, "nullptr provided");
	m_layers.push_back(_layer);
}

void ot::BlockConfiguration::addLayers(const std::vector<BlockLayerConfiguration*>& _layers) {
	for (auto l : _layers) addLayer(l);
}

void ot::BlockConfiguration::setLayers(const std::vector<BlockLayerConfiguration*>& _layers) {
	clearLayers();
	m_layers = _layers;
}

void ot::BlockConfiguration::clearLayers(void) {
	for (auto l : m_layers) delete l;
	m_layers.clear();
}
