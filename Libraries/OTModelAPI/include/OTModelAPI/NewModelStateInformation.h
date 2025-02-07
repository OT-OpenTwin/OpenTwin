#pragma once
#include <list>
#include "OTCore/CoreTypes.h"

#pragma warning(disable: 4251)

namespace ot
{
	struct __declspec(dllexport) NewModelStateInformation
	{
		std::list<ot::UID> m_topologyEntityIDs;
		std::list<ot::UID> m_topologyEntityVersions;
		std::list<bool> m_forceVisible;
		std::list<ot::UID> m_dataEntityIDs;
		std::list<ot::UID> m_dataEntityVersions;
		std::list<ot::UID> m_dataEntityParentIDs;
	};
}


