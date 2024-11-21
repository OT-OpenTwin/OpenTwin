#include "OTGui/TableHeaderOrientation.h"
#include <assert.h>

std::string ot::toString(const ot::TableHeaderOrientation& _orientation)
{
	if (_orientation == TableHeaderOrientation::horizontal)
	{
		return "First row";
	}
	else if (_orientation == TableHeaderOrientation::vertical)
	{
		return "First column";
	}
	else
	{
		assert(_orientation == TableHeaderOrientation::none);
		return "none";
	}
}