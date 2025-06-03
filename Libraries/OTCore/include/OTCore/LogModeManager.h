//! @file LogModeManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Logger.h"
#include "OTCore/OTClassHelper.h"

namespace ot {

	class OT_CORE_API_EXPORT LogModeManager {
		OT_DECL_DEFCOPY(LogModeManager)
		OT_DECL_DEFMOVE(LogModeManager)
	public:
		LogModeManager();
		virtual ~LogModeManager();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setGlobalLogFlags(const LogFlags& _flags) { m_globalFlags = _flags; m_globalFlagsSet = true; };
		const LogFlags& getGlobalLogFlags(void) const { return m_globalFlags; };
		bool getGlobalLogFlagsSet(void) const { return m_globalFlagsSet; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const;

	private:
		bool m_globalFlagsSet;
		LogFlags m_globalFlags;
	};

}