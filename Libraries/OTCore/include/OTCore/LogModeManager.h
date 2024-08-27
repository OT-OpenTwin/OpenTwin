//! @file LogModeManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"

namespace ot {

	class OT_CORE_API_EXPORT LogModeManager {
	public:
		LogModeManager();
		LogModeManager(const LogModeManager& _other);
		virtual ~LogModeManager();

		LogModeManager& operator = (const LogModeManager& _other);

		void setGlobalLogFlags(const LogFlags& _flags) { m_globalFlags = _flags; m_globalFlagsSet = true; };
		const LogFlags& getGlobalLogFlags(void) const { return m_globalFlags; };
		bool getGlobalLogFlagsSet(void) const { return m_globalFlagsSet; };

	private:
		bool m_globalFlagsSet;
		LogFlags m_globalFlags;
	};

}