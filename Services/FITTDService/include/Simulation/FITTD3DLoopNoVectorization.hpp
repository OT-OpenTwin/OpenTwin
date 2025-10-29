// @otlicense

#pragma once

#include "Simulation/FITTD3DLoopNoVectorization.h"

template<class T>
inline void FITTD3DLoopNoVectorization<T>::UpdateSimulationValuesH()
{
	index_t index;

	#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < _grid->GetDegreesOfFreedomNumberInX() - 1; x++)
			{
				T yCurl = *_coefficients->GetCoefficientComponentXCurlYAtIndex(index) * (*_doF.GetElectricVoltageYComponent(index) - *_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneZStep()));
				T zCurl = *_coefficients->GetCoefficientComponentXCurlZAtIndex(index) * (*_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneYStep()) - *_doF.GetElectricVoltageZComponent(index));
				_doF.SetMagneticVoltageXComponentAt(index, *_doF.GetMagneticVoltageXComponent(index) - yCurl - zCurl);
				index++;
				//_grid.SetHx(index,_grid.GetHx(index) - _grid.GetChy(index) * (_grid.GetEz(index + _grid.GetOneYStep()) - _grid.GetEz(index))- _grid.GetChz(index) * ( _grid.GetEy(index) - _grid.GetEy(index +1)));
			}
		}
	}
	#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < _grid->GetDegreesOfFreedomNumberInX() - 1; x++)
			{
				T zCurl = *_coefficients->GetCoefficientComponentYCurlZAtIndex(index) * (*_doF.GetElectricVoltageZComponent(index) - *_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneXStep()));
				T xCurl = *_coefficients->GetCoefficientComponentYCurlXAtIndex(index) * (*_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneZStep()) - *_doF.GetElectricVoltageXComponent(index));
				_doF.SetMagneticVoltageYComponentAt(index, *_doF.GetMagneticVoltageYComponent(index) - zCurl - xCurl);
				index++;
					//_grid.GetChz(index) * (_grid.GetEx(index + 1) - _grid.GetEx(index)) - _grid.GetChx(index) * (_grid.GetEz(index) - _grid.GetEz(index + _grid.GetOneXStep())));
			}
		}
	}
	#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < _grid->GetDegreesOfFreedomNumberInX() - 1; x++)
			{
				T xCurl = *_coefficients->GetCoefficientComponentZCurlXAtIndex(index) * (*_doF.GetElectricVoltageXComponent(index) - *_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneYStep()));
				T yCurl = *_coefficients->GetCoefficientComponentZCurlYAtIndex(index) * (*_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneXStep()) - *_doF.GetElectricVoltageYComponent(index));
				_doF.SetMagneticVoltageZComponentAt(index, *_doF.GetMagneticVoltageZComponent(index) - xCurl - yCurl);
				index++;
				//_grid.SetHz(index, _grid.GetHz(index) - _grid.GetChx(index) * (_grid.GetEy(index + _grid.GetOneXStep()) - _grid.GetEy(index)) - _grid.GetChy(index) * (_grid.GetEx(index) - _grid.GetEx(index + _grid.GetOneYStep())));
			}
		}
	}
}

template<class T>
inline void FITTD3DLoopNoVectorization<T>::UpdateSimulationValuesE()
{
index_t index;

	#pragma omp for nowait
	for (index_t z = 1; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 1; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			for (index_t x = 0; x < _grid->GetDegreesOfFreedomNumberInX()-1; x++)
			{
				T yCurl = *_coefficientsDual->GetCoefficientComponentXCurlYAtIndex(index) * (*_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneZStep()) - *_doF.GetMagneticVoltageYComponent(index));
				T zCurl = *_coefficientsDual->GetCoefficientComponentXCurlZAtIndex(index) * (*_doF.GetMagneticVoltageZComponent(index) - *_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneYStep()));
				_doF.SetElectricVoltageXComponentAt(index, *_doF.GetElectricVoltageXComponent(index) + yCurl + zCurl);
				index++;
				/*_grid.SetEx(index, _grid.GetEx(index) + _grid.GetCey(index) * (_grid.GetHz(index) - _grid.GetHz(index - _grid.GetOneYStep())) + _grid.GetCez(index) * (_grid.GetHy(index - 1) - _grid.GetHy(index)));*/
			}
		}
	}

	#pragma omp for nowait
	for (index_t z = 1; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY()-1; y++)
		{
			index = _grid->GetIndexToCoordinate(1, y, z);
			for (index_t x = 1; x < _grid->GetDegreesOfFreedomNumberInX() - 1; x++)
			{
				T zCurl = *_coefficientsDual->GetCoefficientComponentYCurlZAtIndex(index) * (*_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneXStep()) - *_doF.GetMagneticVoltageZComponent(index));
				T xCurl = *_coefficientsDual->GetCoefficientComponentYCurlXAtIndex(index) * (*_doF.GetMagneticVoltageXComponent(index) - *_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneZStep()));
				_doF.SetElectricVoltageYComponentAt(index, *_doF.GetElectricVoltageYComponent(index) + zCurl + xCurl);
				index++;
				//_grid.SetEy(index, _grid.GetEy(index) + _grid.GetCez(index) * (_grid.GetHx(index) - _grid.GetHx(index - 1)) + _grid.GetCex(index) * (_grid.GetHz(index - _grid.GetOneXStep()) - _grid.GetHz(index)));
			}
		}
	}
	
#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ()-1; z++)
	{
		for (index_t y = 1; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(1, y, z);
			for (index_t x = 1; x < _grid->GetDegreesOfFreedomNumberInX() - 1; x++)
			{
				T xCurl = *_coefficientsDual->GetCoefficientComponentZCurlXAtIndex(index) * (*_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneYStep()) - *_doF.GetMagneticVoltageXComponent(index));
				T yCurl = *_coefficientsDual->GetCoefficientComponentZCurlYAtIndex(index) * (*_doF.GetMagneticVoltageYComponent(index) - *_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneXStep()));
				_doF.SetElectricVoltageZComponentAt(index, *_doF.GetElectricVoltageZComponent(index) + yCurl + xCurl);
				index++;
				//_grid.SetEz(index, _grid.GetEz(index) + _grid.GetCex(index) * (_grid.GetHy(index) - _grid.GetHy(index - _grid.GetOneXStep())) + _grid.GetCey(index) * (_grid.GetHx(index - _grid.GetOneYStep()) - _grid.GetHx(index)));
			}
		}
	}
}
