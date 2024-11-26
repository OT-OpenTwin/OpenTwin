#include "OTGui/TableHeaderOrientation.h"
#include <assert.h>

namespace ot::TableHeaderOrientationNames
{
	const std::string orientationNameHorizontal = "First row";
	const std::string orientationNameVertical = "First column";
	const std::string orientationNameNone = "none";
}

std::string ot::toString(const ot::TableHeaderOrientation& _orientation)
{
	if (_orientation == TableHeaderOrientation::horizontal)
	{
		return ot::TableHeaderOrientationNames::orientationNameHorizontal;
	}
	else if (_orientation == TableHeaderOrientation::vertical)
	{
		return ot::TableHeaderOrientationNames::orientationNameVertical;
	}
	else
	{
		assert(_orientation == TableHeaderOrientation::none);
		return ot::TableHeaderOrientationNames::orientationNameNone;
	}
}

ot::TableHeaderOrientation ot::stringToHeaderOrientation(const std::string& _orientation)
{
	if (_orientation == ot::TableHeaderOrientationNames::orientationNameHorizontal)
	{
		return TableHeaderOrientation::horizontal;
	}
	else if (_orientation == ot::TableHeaderOrientationNames::orientationNameVertical)
	{
		return TableHeaderOrientation::vertical;
	}
	else
	{
		assert(_orientation == TableHeaderOrientationNames::orientationNameNone);
		return TableHeaderOrientation::none;
	}
}
