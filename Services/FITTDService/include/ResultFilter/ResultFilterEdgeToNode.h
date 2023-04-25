#pragma once
#include "ResultFilter/ResultFilter.h"
#include "ResultFilter/IResultFilterEdge.h"
#include "Grid/GridIteratorComponent.h"
#include "Ports/SourceTarget.h"

class ResultFilterEdgeToNode : public ResultFilter, public IResultFilterEdgeSpecific
{
public:
	ResultFilterEdgeToNode(int executionFrequency, int totalTimeSteps);
	virtual void SetVolumeIterator(IGridIteratorVolume * volumenFilter) override;

private:
	GridIteratorComponent * _gridComponentIterator;
	virtual void ExecuteFilter(int currentTimestep, double * input) override;

	// Inherited via IResultFilterEdgeSpecific
	virtual void SetEdgeSepcialization(axis targetComponent) override;
};