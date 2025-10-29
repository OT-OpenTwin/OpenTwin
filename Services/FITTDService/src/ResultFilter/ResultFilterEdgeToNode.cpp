// @otlicense

#include "ResultFilter/ResultFilterEdgeToNode.h"

ResultFilterEdgeToNode::ResultFilterEdgeToNode(int executionFrequency, int totalTimeSteps)
	:ResultFilter(executionFrequency,totalTimeSteps) {}

void ResultFilterEdgeToNode::SetVolumeIterator(IGridIteratorVolume * volumenFilter)
{
	_volumeIterator = volumenFilter;
}

void ResultFilterEdgeToNode::ExecuteFilter(int currentTimestep, double * input)
{
	_volumeIterator->Reset();
	index_t size = _volumeIterator->GetVolumenSize();
	int i = 0;
	double * nodeValues = new double[size];

	index_t index = _volumeIterator->GetCurrentIndex();
	nodeValues[i] = (input[index] + 0 )/2;

	while (_volumeIterator->HasNext())
	{
		i++;
		index = _volumeIterator->GetNextIndex();
		if (_gridComponentIterator->HasPrior())
		{
			index_t neighbour = _gridComponentIterator->GetPreviousTo(index);
			nodeValues[i] = (input[index] - input[neighbour]) / 2;
		}
		else
		{
			nodeValues[i] = input[index];
		}
	}
}

void ResultFilterEdgeToNode::SetEdgeSepcialization(axis targetComponent)
{
	if (targetComponent == axis::x_axis)
	{
		_gridComponentIterator = _volumeIterator->GetXAxisIterator();
	}
	else if (targetComponent == axis::y_axis)
	{
		_gridComponentIterator = _volumeIterator->GetYAxisIterator();
	}
	else
	{
		_gridComponentIterator = _volumeIterator->GetZAxisIterator();
	}
}
