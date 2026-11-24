// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/BlockAndFilter/UserInputSignalBlocker.h"

namespace ot
{

	class OT_WIDGETS_API_EXPORT GlobalUserInputSignalBlocker
	{
		OT_DECL_NOCOPY(GlobalUserInputSignalBlocker)
		OT_DECL_NOMOVE(GlobalUserInputSignalBlocker)
	public:
		static GlobalUserInputSignalBlocker& instance();

		static void install();
		static void uninstall();

		UserInputSignalBlocker* getBlocker() const { return m_blocker; };
		UserInputSignalBlocker* operator->() const { return m_blocker; };

	private:
		bool m_installed;
		bool m_wasInstalled;
		UserInputSignalBlocker* m_blocker;

		GlobalUserInputSignalBlocker();
		~GlobalUserInputSignalBlocker();
	};

}