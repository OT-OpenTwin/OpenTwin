// @otlicense

#include "Grid/SurfaceDiscretization.h"

void SurfaceDiscretization::ExtractComponentsFromSingleVector(const std::vector<double>& surfaceDiscretizations)
{
	if (surfaceDiscretizations.size() % 3 != 0)
	{
		throw std::invalid_argument(composedVectorWrongSizeMessage);
	}
	const index_t offsetY = numberOfNodes = surfaceDiscretizations.size() / 3;
	const index_t offsetZ = offsetY * 2;
	deltaXY = &surfaceDiscretizations[0];
	deltaXZ = &surfaceDiscretizations[offsetY];
	deltaYZ = &surfaceDiscretizations[offsetZ];
}

