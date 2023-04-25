#include "ResultFilter/ResultContainerVector3DComplex.h"
#include "ResultFilter/IResultFilterEdge.h"

ResultContainerVector3DComplex::ResultContainerVector3DComplex(int currentTimeStep, index_t size, double * componentX, double * componentY, double * componentZ)
	:ResultContainer(currentTimeStep,size)
{
	_componentX = componentX;
	_componentY	= componentY;
	_componentZ	= componentZ;
}

ResultContainerVector3DComplex::~ResultContainerVector3DComplex()
{
	delete[] _componentX;
	delete[] _componentY;
	delete[] _componentZ;
}

void ResultContainerVector3DComplex::ApplyFilter(ResultFilter * filter)
{
	if (filter->FrequencyMatches(_currentTimeStep))
	{
		auto edgeSpecificFilter = dynamic_cast<IResultFilterEdgeSpecific*>(filter);
		if (edgeSpecificFilter == nullptr)
		{
			filter->ExecuteIfFrequencyMatches(_currentTimeStep, _componentX);
			filter->ExecuteIfFrequencyMatches(_currentTimeStep, _componentY);
			filter->ExecuteIfFrequencyMatches(_currentTimeStep, _componentZ);
		}
		else
		{
			edgeSpecificFilter->SetEdgeSepcialization(x_axis);
			filter->ExecuteIfFrequencyMatches(_currentTimeStep, _componentX);
			edgeSpecificFilter->SetEdgeSepcialization(y_axis);
			filter->ExecuteIfFrequencyMatches(_currentTimeStep, _componentY);
			edgeSpecificFilter->SetEdgeSepcialization(z_axis);
			filter->ExecuteIfFrequencyMatches(_currentTimeStep, _componentZ);
		}
	}
}