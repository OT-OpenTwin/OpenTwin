// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include "Grid/EdgeDiscretization.h"
#include "Grid/SurfaceDiscretization.h"
#include "Grid/MaterialProperties.h"

template <class T>
class CurlCoefficients
{
protected:
	SurfaceDiscretization * _surfaceDiscretization = nullptr;
	EdgeDiscretization * _edgeDiscretization = nullptr;
	MaterialProperties * _materialProperties = nullptr;
	float _stabilityFactor = 0.98f;

	double(MaterialProperties::*GetMaterialInXAtIndex)(index_t index) const = nullptr;
	double(MaterialProperties::*GetMaterialInYAtIndex)(index_t index) const = nullptr;
	double(MaterialProperties::*GetMaterialInZAtIndex)(index_t index) const = nullptr;

	inline bool HasZeroValueAtIndex(index_t index);
	inline float Potenz(float base, float exp) { return powf(base, exp); }
	inline double Potenz(double base, double exp) { return pow(base, exp); }
	inline double SqrRoot(double value) { return sqrt(value); }
	inline float SqrRoot(float value) { return sqrtf(value); }

	virtual void CheckConsistency() = 0;
	virtual void CalculateTimeStep() = 0;
	virtual void CreateCoefficients() = 0;
	virtual void InitiateCoefficientContainer() = 0;

public:
	void SummarizeDescretizationAsCoefficients(SurfaceDiscretization & surfaceDisc, EdgeDiscretization & edgeDisc, MaterialProperties & materialProp, double(MaterialProperties::*GetActiveMaterialInXAtIndex)(index_t index) const, double(MaterialProperties::*GetActiveMaterialInYAtIndex)(index_t index) const, double(MaterialProperties::*GetActiveMaterialInZAtIndex)(index_t index) const)
	{
		GetMaterialInXAtIndex = GetActiveMaterialInXAtIndex;
		GetMaterialInYAtIndex = GetActiveMaterialInYAtIndex;
		GetMaterialInZAtIndex = GetActiveMaterialInZAtIndex;							  
		
		_surfaceDiscretization = &surfaceDisc;
		_edgeDiscretization = &edgeDisc;
		_materialProperties = &materialProp;

		CheckConsistency();
		CalculateTimeStep();
		InitiateCoefficientContainer();
		CreateCoefficients();

		GetMaterialInXAtIndex = nullptr;
		GetMaterialInYAtIndex = nullptr;
		GetMaterialInZAtIndex = nullptr;
		_surfaceDiscretization = nullptr;
		_edgeDiscretization = nullptr;
		_materialProperties = nullptr;
	};

};

template<class T>
inline bool CurlCoefficients<T>::HasZeroValueAtIndex(index_t index)
{
	return (
		_materialProperties->GetPermeabilityInX(index) == 0 || _materialProperties->GetPermittivityInX(index) == 0 ||
		_materialProperties->GetPermeabilityInY(index) == 0 || _materialProperties->GetPermittivityInY(index) == 0 ||
		_materialProperties->GetPermeabilityInZ(index) == 0 || _materialProperties->GetPermittivityInZ(index) == 0 ||																							   
		_edgeDiscretization->GetDeltaXAtIndex(index) == 0 || _edgeDiscretization->GetDeltaYAtIndex(index) == 0 ||
		_edgeDiscretization->GetDeltaZAtIndex(index) == 0
		);
}