// OpenTwin header
#include "OpenTwinCore/otAssert.h"
#include "OpenTwinCore/rJSONHelper.h"
#include "OTBlockEditorAPI/BlockConfigurationFactory.h"
#include "OTBlockEditorAPI/FlowBlockConfiguration.h"
#include "OTBlockEditorAPI/CustomBlockConfiguration.h"

ot::BlockConfiguration* ot::BlockConfigurationFactory::blockConfigurationFromType(const std::string& _type) {
	if (_type == OT_FLOWBLOCKCONFIGURATION_TYPE) return new FlowBlockConfiguration;
	else if (_type == OT_CUSTOMBLOCKCONFIGURATION_TYPE) return new CustomBlockConfiguration;
	else {
		otAssert(0, "Unknown block type");
		throw std::exception("Invalid type");
	}
}

ot::BlockConfiguration* ot::BlockConfigurationFactory::blockConfigurationFromJson(OT_rJSON_val& _object) {
	OT_rJSON_checkMember(_object, "Type", String);
	ot::BlockConfiguration* newBlock = blockConfigurationFromType(_object["Type"].GetString());
	try {
		newBlock->setFromJsonObject(_object);
		return newBlock;
	}
	catch (const std::exception& _e) {
		delete newBlock;
		throw _e;
	}
	catch (...) {
		delete newBlock;
		throw std::exception("Fatal: Unknown error");
	}
}