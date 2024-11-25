#pragma once
#include <stdint.h>
namespace ot
{
	enum class ViewChangedStates : uint32_t
	{
		viewClosed,
		viewOpened,
		changesSaved
	};
}
