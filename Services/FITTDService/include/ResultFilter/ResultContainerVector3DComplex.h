#pragma once
#include "ResultFilter/ResultContainer.h"
#include "SystemDependencies/SystemDependentDefines.h"

class ResultContainerVector3DComplex : public ResultContainer
{
public:
	ResultContainerVector3DComplex(int currentTimeStep , index_t size, double * componentX, double * componentY, double * componentZ);
	~ResultContainerVector3DComplex();

	double * GetRealComponentX() const { return _componentX; };
	double * GetRealComponentY() const { return _componentY; };
	double * GetRealComponentZ() const { return _componentZ; };
	double * GetImagComponentX() const { return &_componentX[_size]; };
	double * GetImagComponentY() const { return &_componentY[_size]; };
	double * GetImagComponentZ() const { return &_componentZ[_size]; };

	void ApplyFilter(ResultFilter * filter) override;

private:
	double * _componentX;
	double * _componentY;
	double * _componentZ;

	index_t _size;
};