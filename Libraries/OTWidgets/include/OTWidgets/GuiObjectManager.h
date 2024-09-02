//! @file GuiObjectManager.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <list>

namespace ot {

	class ManagedGuiObject;

	class OT_WIDGETS_API_EXPORT GuiObjectManager {
		OT_DECL_NOCOPY(GuiObjectManager)
	public:
		GuiObjectManager();
		virtual ~GuiObjectManager();

	private:
		//! Maps BSI to Keys to ManagedGuiObject
		std::map<BasicServiceInformation, std::map<std::string, ManagedGuiObject*>> m_objects;

		//! Maps BSI to ManagedGuiObjects to Disabled counter
		std::map<BasicServiceInformation, std::map<ManagedGuiObject*, int>> m_objectsDisabledCounter;

		//! Maps BSI ManagedGuiObjects to LockTypeFlags to Locked counter
		std::map<BasicServiceInformation, std::map<ManagedGuiObject*, std::map<LockTypeFlag, int>>> m_objectsLockCounter;
	};

}
