// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include "MeshDiscretization.h"

#include <stdexcept>

class EdgeDiscretization : public MeshDiscretization
{

private:
	const double* deltaX = nullptr;
	const double* deltaY = nullptr;
	const double* deltaZ = nullptr;
 
public:
	explicit EdgeDiscretization(){};
	~EdgeDiscretization()
	{
		deltaX = nullptr;
		deltaY = nullptr;
		deltaZ = nullptr;
	};
	EdgeDiscretization(EdgeDiscretization & other) = delete;
	EdgeDiscretization & operator=(const EdgeDiscretization & other) = delete;

	void ExtractComponentsFromSingleVector(const std::vector<double> & surfaceDiscretizations) override;
	
	inline const double GetDeltaXAtIndex(index_t index) const;
	inline const double GetDeltaYAtIndex(index_t index) const;
	inline const double GetDeltaZAtIndex(index_t index) const;
};

inline const double EdgeDiscretization::GetDeltaXAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaX[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double EdgeDiscretization::GetDeltaYAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaY[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}

inline const double EdgeDiscretization::GetDeltaZAtIndex(index_t index) const
{
	if (index < numberOfNodes)
	{
		return deltaZ[index];
	}
	else
	{
		throw std::out_of_range(outOfRangeMessage);
	}
}