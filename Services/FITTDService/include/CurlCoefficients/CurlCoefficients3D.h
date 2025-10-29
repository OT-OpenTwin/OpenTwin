// @otlicense

#pragma once
#include "CurlCoefficients/CurlCoefficients.h"
#include "Grid/Grid.h"


template <class T>
class CurlCoefficients3D : public CurlCoefficients<T>
{
private:
	using CurlCoefficients<T>::_edgeDiscretization;
	using CurlCoefficients<T>::_materialProperties;
	using CurlCoefficients<T>::_stabilityFactor;
	using CurlCoefficients<T>::_surfaceDiscretization;
	using CurlCoefficients<T>::GetMaterialInXAtIndex;
	using CurlCoefficients<T>::GetMaterialInYAtIndex;
	using CurlCoefficients<T>::GetMaterialInZAtIndex;
	using CurlCoefficients<T>::HasZeroValueAtIndex;
	using CurlCoefficients<T>::Potenz;
	using CurlCoefficients<T>::SqrRoot;

	const Grid<T> &_grid;

	T timeDiscretization = 0;

	T* _coefficientComponentXCurlY = nullptr;
	T* _coefficientComponentXCurlZ = nullptr;
	T* _coefficientComponentYCurlX = nullptr;
	T* _coefficientComponentYCurlZ = nullptr;
	T* _coefficientComponentZCurlX = nullptr;
	T* _coefficientComponentZCurlY = nullptr;

	virtual void CheckConsistency() override;
	virtual void CalculateTimeStep() override;
	virtual void InitiateCoefficientContainer() override;
	virtual void CreateCoefficients() override;

public:
	explicit CurlCoefficients3D(const Grid<T> &grid) : _grid(grid) {};
	CurlCoefficients3D(CurlCoefficients3D& other) = delete;
	CurlCoefficients3D & operator=(CurlCoefficients3D &other) = delete;

	void TestValues(T value);

	~CurlCoefficients3D();

	inline T* GetCoefficientComponentXCurlYAtIndex(index_t index) const { return &_coefficientComponentXCurlY[index]; };
	inline T* GetCoefficientComponentXCurlZAtIndex(index_t index) const { return &_coefficientComponentXCurlZ[index]; };
	inline T* GetCoefficientComponentYCurlXAtIndex(index_t index) const { return &_coefficientComponentYCurlX[index]; };
	inline T* GetCoefficientComponentYCurlZAtIndex(index_t index) const { return &_coefficientComponentYCurlZ[index]; };
	inline T* GetCoefficientComponentZCurlXAtIndex(index_t index) const { return &_coefficientComponentZCurlX[index]; };
	inline T* GetCoefficientComponentZCurlYAtIndex(index_t index) const { return &_coefficientComponentZCurlY[index]; };
	
	inline T GetTimeDiscretization() const { return timeDiscretization; };
};

#include "CurlCoefficients/CurlCoefficients3D.hpp"