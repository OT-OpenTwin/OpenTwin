#include "OTBlockEditorAPI/BlockLayerConfigurationFactory.h"
#include "OTBlockEditorAPI/ImageBlockLayerConfiguration.h"
#include "OTBlockEditorAPI/RectangleBlockLayerConfiguration.h"
#include "OTBlockEditorAPI/TextBlockLayerConfiguration.h"
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/rJSONHelper.h"

ot::BlockLayerConfiguration* ot::BlockLayerConfigurationFactory::blockLayerConfigurationFromType(const std::string& _type) {
	if (_type == OT_TEXTBLOCKLAYERCONFIGURATION_TYPE) return new ot::TextBlockLayerConfiguration;
	else if (_type == OT_IMAGEBLOCKLAYERCONFIGURATION_TYPE) return new ot::ImageBlockLayerConfiguration;
	else if (_type == OT_RECTANGLEBLOCKLAYERCONFIGURATION_TYPE) return new ot::RectangleBlockLayerConfiguration;
	else {
		otAssert(0, "Unknown block type");
		throw std::exception("Invalid type");
	}
}

ot::BlockLayerConfiguration* ot::BlockLayerConfigurationFactory::blockLayerConfigurationFromJson(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "Type", String);
	ot::BlockLayerConfiguration* newBlockLayer = blockLayerConfigurationFromType(_object["Type"].GetString());
	try {
		newBlockLayer->setFromJsonObject(_object);
		return newBlockLayer;
	}
	catch (const std::exception& _e) {
		delete newBlockLayer;
		throw _e;
	}
	catch (...) {
		delete newBlockLayer;
		throw std::exception("Fatal: Unknown error");
	}
}