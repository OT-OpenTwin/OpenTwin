#pragma once
#include <OTCore/Color.h>

namespace SelectionCategorisationColours
{
	static const ot::Color getRMDColour()
	{
		return ot::Color(88, 175, 233, 100);
	}
	static const ot::Color getSMDColour()
	{
		return ot::Color(166, 88, 233, 100);
	}
	static const ot::Color getQuantityColour()
	{
		return ot::Color(233, 185, 88, 100);
	}
	static const ot::Color getParameterColour()
	{
		return ot::Color(88, 233, 122, 100);
	}
}
