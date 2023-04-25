#pragma once
#include "CurlCoefficients3D.h"
#include <algorithm>

template<class T>
inline void CurlCoefficients3D<T>::TestValues(T value)
{
	index_t numberOfDoF = _grid.GetDegreesOfFreedomNumberTotalPadded();
	for (int i = 0; i < numberOfDoF; i++)
	{
		_coefficientComponentXCurlY[i] = value;
		_coefficientComponentXCurlZ[i] = value;
		_coefficientComponentYCurlX[i] = value;
		_coefficientComponentYCurlZ[i] = value;
		_coefficientComponentZCurlX[i] = value;
		_coefficientComponentZCurlY[i] = value;
	}
}

template<class T>
inline CurlCoefficients3D<T>::~CurlCoefficients3D()
{
	if (
		_coefficientComponentXCurlY != nullptr ||
		_coefficientComponentXCurlZ != nullptr ||
		_coefficientComponentYCurlX != nullptr ||
		_coefficientComponentYCurlZ != nullptr ||
		_coefficientComponentZCurlX != nullptr ||
		_coefficientComponentZCurlY != nullptr
		)
	{
		_aligned_free(_coefficientComponentXCurlY);
		_aligned_free(_coefficientComponentXCurlZ);
		_aligned_free(_coefficientComponentYCurlX);
		_aligned_free(_coefficientComponentYCurlZ);
		_aligned_free(_coefficientComponentZCurlX);
		_aligned_free(_coefficientComponentZCurlY);

		_coefficientComponentXCurlY = nullptr;
		_coefficientComponentXCurlZ = nullptr;
		_coefficientComponentYCurlX = nullptr;
		_coefficientComponentYCurlZ = nullptr;
		_coefficientComponentZCurlX = nullptr;
		_coefficientComponentZCurlY = nullptr;
	}
}


template<class T>
void CurlCoefficients3D<T>::CheckConsistency()
{
	if (

		_edgeDiscretization->GetNumberOfNodes() !=
		_surfaceDiscretization->GetNumberOfNodes() ||
		_surfaceDiscretization->GetNumberOfNodes() !=
		(_grid.GetDegreesOfFreedomNumberTotal()) ||
		_surfaceDiscretization->GetNumberOfNodes() !=
		_materialProperties->GetNumberOfNodes()
		)
	{
		throw std::invalid_argument("The given component dimensions, discretization dimensions or material property dimensions are not matching.");
	}

	if (_grid.GetDegreesOfFreedomNumberInX() == 0 || _grid.GetDegreesOfFreedomNumberInY() == 0 || _grid.GetDegreesOfFreedomNumberInZ() == 0)
	{
		throw  std::logic_error("3D _grid with less then 3 dimensions detected.");
	}
}

template<class T>
void CurlCoefficients3D<T>::CreateCoefficients()
{
	T zero = static_cast<T>(0.);
	for (index_t z = 0; z < _grid.GetDegreesOfFreedomNumberInZ(); z++)
	{
		for (index_t y = 0; y < _grid.GetDegreesOfFreedomNumberInY(); y++)
		{
			for (index_t x = 0; x < _grid.GetDegreesOfFreedomNumberInX(); x++)
			{
				index_t index = _grid.GetIndexToCoordinateNotPadded(x, y, z);
				index_t indexPedded = _grid.GetIndexToCoordinate(x, y, z);
				if ((*_materialProperties.*GetMaterialInXAtIndex)(index) != 0 && (*_materialProperties.*GetMaterialInYAtIndex)(index) != 0 && (*_materialProperties.*GetMaterialInZAtIndex)(index) != 0)
				{
					if (_surfaceDiscretization->GetDeltaYZAtIndex(index) != 0)
					{

						_coefficientComponentXCurlY[indexPedded] = (static_cast<T>(timeDiscretization) * static_cast<T>(_edgeDiscretization->GetDeltaYAtIndex(index))) /
							(static_cast<T>((*_materialProperties.*GetMaterialInYAtIndex)(index)) * static_cast<T>(_surfaceDiscretization->GetDeltaYZAtIndex(index)));
						_coefficientComponentXCurlZ[indexPedded] = (static_cast<T>(timeDiscretization) * static_cast<T>(_edgeDiscretization->GetDeltaZAtIndex(index))) /
							(static_cast<T>((*_materialProperties.*GetMaterialInZAtIndex)(index)) * static_cast<T>(_surfaceDiscretization->GetDeltaYZAtIndex(index)));					}
					else
					{
						_coefficientComponentXCurlY[indexPedded] = zero;
						_coefficientComponentXCurlZ[indexPedded] = zero;
					}

					if (_surfaceDiscretization->GetDeltaXZAtIndex(index) != 0)
					{
						_coefficientComponentYCurlX[indexPedded] = (static_cast<T>(timeDiscretization) * static_cast<T>(_edgeDiscretization->GetDeltaXAtIndex(index))) /
							(static_cast<T>((*_materialProperties.*GetMaterialInXAtIndex)(index)) * static_cast<T>(_surfaceDiscretization->GetDeltaXZAtIndex(index)));
						_coefficientComponentYCurlZ[indexPedded] = (static_cast<T>(timeDiscretization) * static_cast<T>(_edgeDiscretization->GetDeltaZAtIndex(index))) /
							(static_cast<T>((*_materialProperties.*GetMaterialInZAtIndex)(index)) * static_cast<T>(_surfaceDiscretization->GetDeltaXZAtIndex(index)));

					}
					else
					{
						_coefficientComponentYCurlX[indexPedded] = zero;
						_coefficientComponentYCurlZ[indexPedded] = zero;
					}

					if (_surfaceDiscretization->GetDeltaXYAtIndex(index) != 0)
					{
								_coefficientComponentZCurlX[indexPedded] = (static_cast<T>(timeDiscretization) * static_cast<T>(_edgeDiscretization->GetDeltaXAtIndex(index))) /
							(static_cast<T>((*_materialProperties.*GetMaterialInXAtIndex)(index)) * static_cast<T>(_surfaceDiscretization->GetDeltaXYAtIndex(index)));
						_coefficientComponentZCurlY[indexPedded] = (static_cast<T>(timeDiscretization) * static_cast<T>(_edgeDiscretization->GetDeltaYAtIndex(index))) /
							(static_cast<T>((*_materialProperties.*GetMaterialInYAtIndex)(index)) * static_cast<T>(_surfaceDiscretization->GetDeltaXYAtIndex(index)));
					}
					else
					{
						_coefficientComponentZCurlX[indexPedded] = zero;
						_coefficientComponentZCurlY[indexPedded] = zero;
					}
				}
				else
				{
					_coefficientComponentXCurlY[indexPedded] = zero;
					_coefficientComponentXCurlZ[indexPedded] = zero;
					_coefficientComponentYCurlX[indexPedded] = zero;
					_coefficientComponentYCurlZ[indexPedded] = zero;
					_coefficientComponentZCurlX[indexPedded] = zero;
					_coefficientComponentZCurlY[indexPedded] = zero;
				}
			}
		}
	}

}

#undef min
template<class T>
void CurlCoefficients3D<T>::CalculateTimeStep()
{
	T one = static_cast<T>(1.);
	T two = static_cast<T>(2.);

	index_t x(0), y(0), z(0), index(0);
	//find first index that is no PEC respectively PMC
	while (HasZeroValueAtIndex(index) && !(x == _grid.GetDegreesOfFreedomNumberInX() - 1 && y == _grid.GetDegreesOfFreedomNumberInY() - 1 && z == _grid.GetDegreesOfFreedomNumberInZ() - 1))
	{
		if (x < _grid.GetDegreesOfFreedomNumberInX())
		{
			x++;
		}
		else
		{
			x = 0;
			if (y < _grid.GetDegreesOfFreedomNumberInY())
			{
				y++;
			}
			else
			{
				y = 0;
				z++;
			}
		}
		index = _grid.GetIndexToCoordinateNotPadded(x, y, z);
	}
	//No such element was found
	if (HasZeroValueAtIndex(index) && (x == _grid.GetDegreesOfFreedomNumberInX() - 1 && y == _grid.GetDegreesOfFreedomNumberInY() - 1 && z == _grid.GetDegreesOfFreedomNumberInZ() - 1))
	{
		throw std::logic_error("All nodes are either PEC or PMC elements");
	}							   
																																										 
	//Smallest material value leads to biggest propagation speed, leads to smallest, biggest stable time step.
	double permeability(0), permittivity(0);
	permeability = std::min({ 
	_materialProperties->GetPermeabilityInX(index),
	_materialProperties->GetPermeabilityInY(index),
	_materialProperties->GetPermeabilityInZ(index)
	});
	permittivity = std::min({ _materialProperties->GetPermittivityInX(index), _materialProperties->GetPermittivityInY(index), _materialProperties->GetPermittivityInZ(index) });
		
	T propagationSpeed = SqrRoot(
		one / (static_cast<T>(permittivity)* static_cast<T>(permeability))
	);
	timeDiscretization = one / SqrRoot(
		Potenz(one / static_cast<T>(_edgeDiscretization->GetDeltaXAtIndex(index)), two) +
		Potenz(one / static_cast<T>(_edgeDiscretization->GetDeltaYAtIndex(index)), two) +
		Potenz(one / static_cast<T>(_edgeDiscretization->GetDeltaZAtIndex(index)), two))
		* _stabilityFactor / propagationSpeed;

	for (z = 0; z < _grid.GetDegreesOfFreedomNumberInZ(); z++)
	{
		for (y = 0; y < _grid.GetDegreesOfFreedomNumberInY(); y++)
		{
			for (x = 0; x < _grid.GetDegreesOfFreedomNumberInX(); x++)
			{
				index = _grid.GetIndexToCoordinateNotPadded(x, y, z);
				if (!HasZeroValueAtIndex(index))
				{
					//permeability = std::min({_materialProperties->GetPermeabilityInX(index), _materialProperties->GetPermeabilityInY(index), _materialProperties->GetPermeabilityInZ(index)});
					//permittivity = std::min({_materialProperties->GetPermittivityInX(index), _materialProperties->GetPermittivityInY(index), _materialProperties->GetPermittivityInZ(index)});
					propagationSpeed = SqrRoot(
						one / (static_cast<T>(permeability) * static_cast<T>(permittivity))
					);

					T temp = one / SqrRoot(
						Potenz(static_cast<T>(one / _edgeDiscretization->GetDeltaXAtIndex(index)), two) +
						Potenz(static_cast<T>(one / _edgeDiscretization->GetDeltaYAtIndex(index)), two) +
						Potenz(static_cast<T>(one / _edgeDiscretization->GetDeltaZAtIndex(index)), two)
						* _stabilityFactor / propagationSpeed
					);

					if (temp < timeDiscretization)
					{
						timeDiscretization = temp;
					}
				}
			}
		}
	}
	//timeDiscretization = 6.0182*pow(10, -10);
}

template<class T>
void CurlCoefficients3D<T>::InitiateCoefficientContainer()
{
	index_t numberOfDoF = _grid.GetDegreesOfFreedomNumberTotalPadded();
	_coefficientComponentXCurlY = static_cast<T*>(_aligned_malloc(numberOfDoF * static_cast<index_t>(sizeof(T)), _grid.GetAlignment()));
	_coefficientComponentXCurlZ = static_cast<T*>(_aligned_malloc(numberOfDoF * static_cast<index_t>(sizeof(T)), _grid.GetAlignment()));
	_coefficientComponentYCurlX = static_cast<T*>(_aligned_malloc(numberOfDoF * static_cast<index_t>(sizeof(T)), _grid.GetAlignment()));
	_coefficientComponentYCurlZ = static_cast<T*>(_aligned_malloc(numberOfDoF * static_cast<index_t>(sizeof(T)), _grid.GetAlignment()));
	_coefficientComponentZCurlX = static_cast<T*>(_aligned_malloc(numberOfDoF * static_cast<index_t>(sizeof(T)), _grid.GetAlignment()));
	_coefficientComponentZCurlY = static_cast<T*>(_aligned_malloc(numberOfDoF * static_cast<index_t>(sizeof(T)), _grid.GetAlignment()));

	T zero = static_cast <T>(0.);
	for (int i = 0; i < numberOfDoF; i++)
	{
		_coefficientComponentXCurlY[i] = zero;
		_coefficientComponentXCurlZ[i] = zero;
		_coefficientComponentYCurlX[i] = zero;
		_coefficientComponentYCurlZ[i] = zero;
		_coefficientComponentZCurlX[i] = zero;
		_coefficientComponentZCurlY[i] = zero;
	}
}


