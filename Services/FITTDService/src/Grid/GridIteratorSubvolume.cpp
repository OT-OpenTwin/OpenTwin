// @otlicense

#include "Grid/GridIteratorSubvolume.h"



GridIteratorSubvolume::GridIteratorSubvolume(GridIteratorVolume & volumeIterator)
{
	_iteratorXAxis = new GridIteratorComponentSubspace(volumeIterator.GetXAxisIterator()->GetStartPoint(),
		volumeIterator.GetXAxisIterator()->GetEndPoint(), volumeIterator.GetXAxisIterator()->GetStepwidth());
	_iteratorYAxis = new GridIteratorComponentSubspace(volumeIterator.GetYAxisIterator()->GetStartPoint(),
		volumeIterator.GetYAxisIterator()->GetEndPoint(), volumeIterator.GetYAxisIterator()->GetStepwidth());
	_iteratorZAxis = new GridIteratorComponentSubspace(volumeIterator.GetZAxisIterator()->GetStartPoint(),
		volumeIterator.GetZAxisIterator()->GetEndPoint(), volumeIterator.GetZAxisIterator()->GetStepwidth());
	CalculateSubvolumeSize();
}

bool GridIteratorSubvolume::SetXSubspace(index_t startPoint, index_t endPoint)
{
	bool success = dynamic_cast<GridIteratorComponentSubspace*>(_iteratorXAxis)->SetSubspace(startPoint, endPoint);
	if (success)
	{
		CalculateSubvolumeSize();
	}
	return success;
}

bool GridIteratorSubvolume::SetYSubspace(index_t startPoint, index_t endPoint)
{
	bool success = dynamic_cast<GridIteratorComponentSubspace*>(_iteratorYAxis)->SetSubspace(startPoint, endPoint);
	if (success)
	{
		CalculateSubvolumeSize();
	}
	return success;
}

bool GridIteratorSubvolume::SetZSubspace(index_t startPoint, index_t endPoint)
{
	bool success = dynamic_cast<GridIteratorComponentSubspace*>(_iteratorZAxis)->SetSubspace(startPoint, endPoint);
	if (success)
	{
		CalculateSubvolumeSize();
	}
	return success;
}

GridIteratorVolume * GridIteratorSubvolume::CreateIteratorVolumeFromSubvolume()
{
	index_t xDoF = _iteratorXAxis->GetEndPoint() - _iteratorXAxis->GetStartPoint();
	index_t yDoF = _iteratorYAxis->GetEndPoint() - _iteratorYAxis->GetStartPoint();
	index_t zDoF = _iteratorZAxis->GetEndPoint() - _iteratorZAxis->GetStartPoint();
	auto xIter = new GridIteratorComponent(0, xDoF, 1);
	auto yIter = new GridIteratorComponent(0, yDoF, xDoF);
	auto zIter = new GridIteratorComponent(0, zDoF, xDoF * yDoF);
	return new GridIteratorVolume(xIter, yIter, zIter, _volumeSize);
}

void GridIteratorSubvolume::CalculateSubvolumeSize()
{
	index_t xDoF = _iteratorXAxis->GetEndPoint() - _iteratorXAxis->GetStartPoint()+1;
	index_t yDoF = _iteratorYAxis->GetEndPoint() - _iteratorYAxis->GetStartPoint()+1;
	index_t zDoF = _iteratorZAxis->GetEndPoint() - _iteratorZAxis->GetStartPoint()+1;

	_volumeSize = xDoF * yDoF * zDoF;
	_startIndex = _currentIndex = _iteratorXAxis->GetStartPoint() + _iteratorYAxis->GetStartPoint() * _iteratorYAxis->GetStepwidth() + _iteratorZAxis->GetStartPoint() * _iteratorZAxis->GetStepwidth();
}