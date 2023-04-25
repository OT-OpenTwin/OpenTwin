#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include <string>
#include <vector>

class MeshDiscretization
{
protected:
	index_t numberOfNodes = 0;
	const std::string outOfRangeMessage = "Index is larger then the numberOfNodes in the discretization vector.";
	const std::string composedVectorWrongSizeMessage = "Composed surface vector requires to be a multiple of 3.";
	
public:
	virtual void ExtractComponentsFromSingleVector(const std::vector<double> & edgeDiscretization) = 0;
	inline index_t GetNumberOfNodes() const;
};

inline index_t MeshDiscretization::GetNumberOfNodes() const
{
	return numberOfNodes;
}
