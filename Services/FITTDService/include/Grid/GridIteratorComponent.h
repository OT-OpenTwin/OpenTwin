// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"

class GridIteratorComponent
{
private:
	index_t _resetDistance;
	index_t _stepWidth;

protected:
	index_t _currentPoint;
	index_t _startPoint;
	index_t _endPoint;

	void SetResetDistance() { _resetDistance = (-_endPoint + _startPoint) * _stepWidth; };
public:
	//Vll friend von grid um zu checken ob endpoint auch nicht die dimensionen überschreitet.
	GridIteratorComponent(index_t startPoint, index_t endPoint, index_t stepWidth)
		: _startPoint(startPoint), _endPoint(endPoint), _stepWidth(stepWidth), _currentPoint(startPoint)
	{
		SetResetDistance();
	};
	virtual ~GridIteratorComponent() {};

	bool HasNext() const 
	{
		bool hasNext = _currentPoint < _endPoint;
		return hasNext; 
	};

	/* 
		!ACHTUNG! Konzeptbruch !ACHTUNG! 
		HasNext bezieht sich auf _endPoint welches eine Information vom Subvolumen ist. HasPrior schaut nun auf das gesammte Rechengebiet.
		Hack für ResultFilterSourceVektorVolumeEdgeToNode.
	*/
	bool HasPrior()const
	{
		bool hasPrior = _currentPoint > 0;
		return hasPrior;
	}

	void SetToNext(index_t & index) 
	{
		index += _stepWidth; 
		_currentPoint++;
	};
	void SetFromLastToFirst(index_t & index) 
	{ 
		index += _resetDistance; 
		_currentPoint = _startPoint;
	};
	
	index_t GetNextTo(index_t index) { return index + _stepWidth; };
	index_t GetPreviousTo(index_t index) { return index - _stepWidth; };

	index_t GetStartPoint() const { return _startPoint; };
	index_t GetEndPoint()const { return _endPoint; };

	index_t GetStepwidth() const { return _stepWidth; };
};