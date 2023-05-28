//! @file BlockEditorAPI.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockEditorAPI.h"
#include "OTBlockEditor/BlockPickerWidget.h"
#include "OTBlockEditor/BlockNetworkEditor.h"
#include "OpenTwinCore/Singleton.h"
#include "OpenTwinCore/ObjectManagerTemplate.h"
#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/Owner.h"
#include "OpenTwinCore/otAssert.h"

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// API Manager

namespace ot {
	namespace intern {
		static ot::BlockPickerWidget* g_blockPickerWidget{ nullptr };

		class BlockEditorAPIManager : public ServiceOwnerManagerTemplate<ot::BlockNetworkEditor>, public ot::Singleton<ot::intern::BlockEditorAPIManager> { OT_SINGLETON(ot::intern::BlockEditorAPIManager) };
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

ot::BlockNetworkEditor* ot::BlockEditorAPI::createEmptyBlockEditor(const ot::ServiceOwner_t& _owner, const ot::BlockEditorConfigurationPackage& _config) {
	otAssert(ot::intern::g_blockPickerWidget != nullptr, "No global block picker widget set");

	BlockNetworkEditor* newEditor = new BlockNetworkEditor;

	ot::intern::BlockEditorAPIManager::instance().store(_owner, newEditor);

	ot::intern::g_blockPickerWidget->addTopLevelCategories(_config.topLevelBlockCategories());

	return newEditor;
}

void ot::BlockEditorAPI::setGlobalBlockPickerWidget(ot::BlockPickerWidget* _picker) {
	ot::intern::g_blockPickerWidget = _picker;
}

void ot::BlockEditorAPI::clean(const ot::ServiceOwner_t& _owner) {
	ot::intern::BlockEditorAPIManager::instance().free(_owner);
}

std::list<ot::BlockNetworkEditor*> ot::BlockEditorAPI::blockEditors(const ot::ServiceOwner_t& _owner) {
	std::list<ot::BlockNetworkEditor*> ret;

	if (ot::intern::BlockEditorAPIManager::instance().contains(_owner)) {
		ret = *ot::intern::BlockEditorAPIManager::instance()[_owner];
	}
	return ret;
}