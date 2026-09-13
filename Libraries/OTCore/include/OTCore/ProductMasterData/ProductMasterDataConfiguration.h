#pragma once
//@otlicense
#include <set>

#include "OTCore/ProductMasterData/ZoneTag.h"

namespace ot
{
	struct ProductMasterDataConfiguration
	{
		std::set<ot::ZoneTag> m_zoneTags;
	};
};
