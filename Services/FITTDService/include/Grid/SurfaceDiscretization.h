#pragma once
#include <vector>
#include <string>
#include <stdexcept>

#include "SystemDependencies/SystemDependentDefines.h"
#include "MeshDiscretization.h"

class SurfaceDiscretization : public MeshDiscretization
{

private:
	const double* deltaXY =nullptr;
	const double* deltaXZ =nullptr;
	const double* deltaYZ =nullptr;
	
public:
	explicit SurfaceDiscretization(){};
	~SurfaceDiscretization() 
	{
		deltaXY = nullptr;
		deltaXZ = nullptr;
		deltaYZ = nullptr;
	};
	SurfaceDiscretization(SurfaceDiscretization & other) = delete;
	SurfaceDiscretization & operator=(const SurfaceDiscretization & other) = delete;

	void ExtractComponentsFromSingleVector(const std::vector<double> & surfaceDiscretizations) override;

	inline const double GetDeltaXYAtIndex(index_t index) const;
	inline const double GetDeltaXZAtIndex(index_t index) const;
	inline const double GetDeltaYZAtIndex(index_t index) const;
	
};

inline const double SurfaceDiscretization::GetDeltaXYAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaXY[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double SurfaceDiscretization::GetDeltaXZAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaXZ[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double SurfaceDiscretization::GetDeltaYZAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaYZ[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}