// @otlicense

#include "Grid/EdgeDiscretization.h"

void EdgeDiscretization::ExtractComponentsFromSingleVector(const std::vector<double>& surfaceDiscretizations)
{
	if (surfaceDiscretizations.size() % 3 != 0 || surfaceDiscretizations.empty())
	{
		throw  std::invalid_argument(composedVectorWrongSizeMessage);
	}
	const index_t offsetY = numberOfNodes = surfaceDiscretizations.size() / 3;
	const index_t offsetZ = offsetY * 2;
	deltaX = &surfaceDiscretizations[0];
	deltaY = &surfaceDiscretizations[offsetY];
	deltaZ = &surfaceDiscretizations[offsetZ];
}