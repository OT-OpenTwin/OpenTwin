#pragma once
#include "SystemDependencies/SystemDependentDefines.h"

class MaterialProperties
{
private:
	double * relativePermeability = nullptr;
	double * relativePermittivity = nullptr;
	const index_t numberOfNodes = 0;

	const double freeSpacePermittivity = 8.854187817E-12; //physical constant
	const double freeSpacePermeability = 1.256637061E-6; //physical constant

	index_t yOffset;
	index_t zOffset;		 
public:
	explicit MaterialProperties(double * permeability, double * permittivity, index_t numberOfNodes);
	~MaterialProperties() 
	{
		relativePermeability = nullptr;
		relativePermittivity = nullptr;
	};
	MaterialProperties(MaterialProperties & other) = delete;
	MaterialProperties & operator=(const MaterialProperties & other) = delete;

	inline const index_t GetNumberOfNodes() const;
	
	inline double GetPermeabilityInX(index_t index) const;
	inline double GetPermeabilityInY(index_t index) const;
	inline double GetPermeabilityInZ(index_t index) const;
	
	inline double GetPermittivityInX(index_t index) const;
	inline double GetPermittivityInY(index_t index) const;
	inline double GetPermittivityInZ(index_t index) const;												   
};
inline MaterialProperties::MaterialProperties(double * relativePermeability, double * relativePermittivity, index_t numberOfNodes)
:relativePermeability(relativePermeability), relativePermittivity(relativePermittivity), numberOfNodes(numberOfNodes), yOffset(numberOfNodes), zOffset(numberOfNodes * 2) {}

inline const index_t MaterialProperties::GetNumberOfNodes() const
{
	return numberOfNodes;
}

inline double MaterialProperties::GetPermeabilityInX(index_t index) const
{
	return relativePermeability[index] * freeSpacePermeability;
}
inline double MaterialProperties::GetPermeabilityInY(index_t index) const
{
	return relativePermeability[index+ yOffset] * freeSpacePermeability;
}
inline double MaterialProperties::GetPermeabilityInZ(index_t index) const
{
	return relativePermeability[index + zOffset] * freeSpacePermeability;
}

inline double MaterialProperties::GetPermittivityInX(index_t index) const
{
	return relativePermittivity[index] * freeSpacePermittivity;
}
inline double MaterialProperties::GetPermittivityInY(index_t index) const
{
	return relativePermittivity[index + yOffset] * freeSpacePermittivity;
}
inline double MaterialProperties::GetPermittivityInZ(index_t index) const
{
	return relativePermittivity[index + zOffset] * freeSpacePermittivity;
}
													
													