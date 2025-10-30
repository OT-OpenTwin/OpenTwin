// @otlicense
// File: FITTD3DLoopAVX512Sp.hpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#include "Simulation/FITTD3DLoopAVX512Sp.h"
#include <immintrin.h>
#include <cassert>

inline FITTD3DLoopAVX512Sp::FITTD3DLoopAVX512Sp(Grid<float>* grid, CurlCoefficients3D<float>* coefficients, CurlCoefficients3D<float>* coefficientsDual)
	: FITTD3DLoop<float>(grid, coefficients, coefficientsDual)
{
	assert(_grid->GetAlignment() == AVX512);

	const IntrinsicMasks * masks = _grid->GetIntrinsicMasks();
	for (int i = 0; i < _grid->GetLengthOfIntrinsicVector(); i++)
	{
		if (masks->GetMaskBeginBit(i) == 1)
		{
			SetBitInMask(_maskBegin, i);
		}
		if (masks->GetMaskBeginNoBeginningExcludeBit(i))
		{
			SetBitInMask(_maskBeginNoBeginningExclude, i);
		}
		if (masks->GetMaskEndBit(i))
		{
			SetBitInMask(_maskEnd, i);
		}
	}
};

inline void FITTD3DLoopAVX512Sp::SetBitInMask(unsigned short &mask, int bitIndex)
{
	mask |= 1i16 << bitIndex;
}

void FITTD3DLoopAVX512Sp::UpdateSimulationValuesE()
{
index_t index;
#pragma omp for nowait
	for (index_t z = 1; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 1; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			MaskedUpdateEx(index, _maskBeginNoBeginningExclude);
			index += _grid->GetLengthOfIntrinsicVector();

			for (index_t x = 1; x < _grid->GetNumberOfIntrinsicVectors() - 1; x++)
			{
				UpdateEx(index);
				index += _grid->GetLengthOfIntrinsicVector();
			}
			MaskedUpdateEx(index, _maskEnd);
		}
	}

#pragma omp for nowait
	for (index_t z = 1; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			MaskedUpdateEy(index, _maskBegin);
			index += _grid->GetLengthOfIntrinsicVector();

			for (index_t x = 1; x < _grid->GetNumberOfIntrinsicVectors() - 1; x++)
			{
				UpdateEy(index);
				index += _grid->GetLengthOfIntrinsicVector();

			}
			MaskedUpdateEy(index, _maskEnd);
		}
	}
#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 1; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			MaskedUpdateEz(index, _maskBegin);
			index += _grid->GetLengthOfIntrinsicVector();

			for (index_t x = 1; x < _grid->GetNumberOfIntrinsicVectors() - 1; x++)
			{
				UpdateEz(index);
				index += _grid->GetLengthOfIntrinsicVector();

			}
			MaskedUpdateEz(index, _maskEnd);
		}
	}
}

void FITTD3DLoopAVX512Sp::UpdateSimulationValuesH()
{
index_t index;
#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			MaskedUpdateHx(index, _maskBeginNoBeginningExclude);
			index += _grid->GetLengthOfIntrinsicVector();

			for (int x = 1; x < _grid->GetNumberOfIntrinsicVectors() - 1; x++)
			{
				UpdateHx(index);
				index += _grid->GetLengthOfIntrinsicVector();
			}
			MaskedUpdateHx(index, _maskEnd);
		}
	}
#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ() - 1; z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY(); y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			MaskedUpdateHy(index, _maskBeginNoBeginningExclude);
			index += _grid->GetLengthOfIntrinsicVector();

			for (int x = 1; x < _grid->GetNumberOfIntrinsicVectors() - 1; x++)
			{
				UpdateHy(index);
				index += _grid->GetLengthOfIntrinsicVector();

			}
			MaskedUpdateHy(index, _maskEnd);
		}
	}
#pragma omp for nowait
	for (index_t z = 0; z < _grid->GetDegreesOfFreedomNumberInZ(); z++)
	{
		for (index_t y = 0; y < _grid->GetDegreesOfFreedomNumberInY() - 1; y++)
		{
			index = _grid->GetIndexToCoordinate(0, y, z);
			MaskedUpdateHz(index, _maskBeginNoBeginningExclude);
			index += _grid->GetLengthOfIntrinsicVector();

			for (int x = 1; x < _grid->GetNumberOfIntrinsicVectors() - 1; x++)
			{
				UpdateHz(index);
				index += _grid->GetLengthOfIntrinsicVector();
			}
			MaskedUpdateHz(index, _maskEnd);
		}
	}
}

inline void FITTD3DLoopAVX512Sp::UpdateEx(index_t index)
{
	__m512 Ex = _mm512_load_ps(_doF.GetElectricVoltageXComponent(index));
	__m512 Hy = _mm512_load_ps(_doF.GetMagneticVoltageYComponent(index));
	__m512 Hz = _mm512_load_ps(_doF.GetMagneticVoltageZComponent(index));
	__m512 Hy_shift = _mm512_load_ps(_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneZStep()));
	__m512 Hz_shift = _mm512_load_ps(_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneYStep()));
	__m512 yCurl = _mm512_load_ps(_coefficientsDual->GetCoefficientComponentXCurlYAtIndex(index));
	__m512 zCurl = _mm512_load_ps(_coefficientsDual->GetCoefficientComponentXCurlZAtIndex(index));

	////_grid.SetEx(index, _grid.GetEx(index) + _grid.GetCey(index) * (_grid.GetHz(index) - _grid.GetHz(index - _grid.GetOneYStep())) + _grid.GetCez(index) * (_grid.GetHy(index - 1) - _grid.GetHy(index)));
	__m512 result = _mm512_add_ps(Ex,
		_mm512_fmadd_ps(zCurl,
			_mm512_sub_ps(Hz, Hz_shift),
			_mm512_mul_ps(yCurl,
				_mm512_sub_ps(Hy_shift, Hy))));
	_mm512_store_ps(_doF.GetElectricVoltageXComponent(index), result);
}

inline void FITTD3DLoopAVX512Sp::UpdateEy(index_t index)
{
	__m512 Ey = _mm512_load_ps(_doF.GetElectricVoltageYComponent(index));
	__m512 Hx = _mm512_load_ps(_doF.GetMagneticVoltageXComponent(index));
	__m512 Hz = _mm512_load_ps(_doF.GetMagneticVoltageZComponent(index));
	__m512 Hx_shift = _mm512_load_ps(_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneZStep()));
	__m512 Hz_shift = _mm512_load_ps(_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_load_ps(_coefficientsDual->GetCoefficientComponentYCurlXAtIndex(index));
	__m512 zCurl = _mm512_load_ps(_coefficientsDual->GetCoefficientComponentYCurlZAtIndex(index));
	//_grid.SetEy(tempIndex,_grid.GetEy(tempIndex) + _grid.GetCez(tempIndex) * (_grid.GetHx(tempIndex) - _grid.GetHx(tempIndex -1)) + _grid.GetCex(tempIndex) * (_grid.GetHz(tempIndex - _grid.GetOneXStep()) - _grid.GetHz(tempIndex)));
	__m512 result = _mm512_add_ps(Ey,
		_mm512_fmadd_ps(xCurl,
			_mm512_sub_ps(Hx, Hx_shift),
			_mm512_mul_ps(zCurl,
				_mm512_sub_ps(Hz_shift, Hz))));
	_mm512_store_ps(_doF.GetElectricVoltageYComponent(index), result);
}

inline void FITTD3DLoopAVX512Sp::UpdateEz(index_t index)
{
	__m512 Ez = _mm512_load_ps(_doF.GetElectricVoltageZComponent(index));
	__m512 Hx = _mm512_load_ps(_doF.GetMagneticVoltageXComponent(index));
	__m512 Hy = _mm512_load_ps(_doF.GetMagneticVoltageYComponent(index));
	__m512 Hx_shift = _mm512_load_ps(_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneYStep()));
	__m512 Hy_shift = _mm512_load_ps(_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_load_ps(_coefficientsDual->GetCoefficientComponentZCurlXAtIndex(index));
	__m512 yCurl = _mm512_load_ps(_coefficientsDual->GetCoefficientComponentZCurlYAtIndex(index));
	//_grid.SetEz(tempIndex, _grid.GetEz(tempIndex) + _grid.GetCex(tempIndex) * (_grid.GetHy(tempIndex) - _grid.GetHy(tempIndex - _grid.GetOneXStep())) + _grid.GetCey(tempIndex) * (_grid.GetHx(tempIndex - _grid.GetOneYStep()) - _grid.GetHx(tempIndex)));
	__m512 result = _mm512_add_ps(Ez,
		_mm512_fmadd_ps(yCurl,
			_mm512_sub_ps(Hy, Hy_shift),
			_mm512_mul_ps(xCurl,
				_mm512_sub_ps(Hx_shift, Hx))));
	_mm512_store_ps(_doF.GetElectricVoltageZComponent(index), result);
}

inline void FITTD3DLoopAVX512Sp::UpdateHx(index_t index)
{
	__m512 Hx = _mm512_load_ps(_doF.GetMagneticVoltageXComponent(index));
	__m512 Ey = _mm512_load_ps(_doF.GetElectricVoltageYComponent(index));
	__m512 Ez = _mm512_load_ps(_doF.GetElectricVoltageZComponent(index));
	__m512 Ey_shift = _mm512_load_ps(_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneZStep()));
	__m512 Ez_shift = _mm512_load_ps(_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneYStep()));
	__m512 yCurl = _mm512_load_ps(_coefficients->GetCoefficientComponentXCurlYAtIndex(index));
	__m512 zCurl = _mm512_load_ps(_coefficients->GetCoefficientComponentXCurlZAtIndex(index));
	//_grid.SetHx(index,_grid.GetHx(tempIndex) - _grid.GetChy(tempIndex) * (_grid.GetEz(tempIndex + _grid.GetOneYStep()) - _grid.GetEz(tempIndex))- _grid.GetChz(tempIndex) * ( _grid.GetEy(tempIndex) - _grid.GetEy(tempIndex +1)));
	__m512 result = _mm512_sub_ps(Hx,
		_mm512_fmadd_ps(yCurl,
			_mm512_sub_ps(Ey, Ey_shift),
			_mm512_mul_ps(zCurl,
				_mm512_sub_ps(Ez_shift, Ez))));
	_mm512_store_ps(_doF.GetMagneticVoltageXComponent(index), result);
}

inline void FITTD3DLoopAVX512Sp::UpdateHy(index_t index)
{
	__m512 Hy = _mm512_load_ps(_doF.GetMagneticVoltageYComponent(index));
	__m512 Ex = _mm512_load_ps(_doF.GetElectricVoltageXComponent(index));
	__m512 Ez = _mm512_load_ps(_doF.GetElectricVoltageZComponent(index));
	__m512 Ex_shift = _mm512_load_ps(_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneZStep()));
	__m512 Ez_shift = _mm512_load_ps(_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_load_ps(_coefficients->GetCoefficientComponentYCurlXAtIndex(index));
	__m512 zCurl = _mm512_load_ps(_coefficients->GetCoefficientComponentYCurlZAtIndex(index));
	//_grid.SetHy(tempIndex, _grid.GetHy(tempIndex) - _grid.GetChz(tempIndex) * (_grid.GetEx(tempIndex +1) - _grid.GetEx(tempIndex)) - _grid.GetChx(tempIndex) * (_grid.GetEz(tempIndex) - _grid.GetEz(tempIndex + _grid.GetOneXStep())) );
	__m512 result = _mm512_sub_ps(Hy,
		_mm512_fmadd_ps(xCurl,
			_mm512_sub_ps(Ex_shift, Ex),
			_mm512_mul_ps(zCurl,
				_mm512_sub_ps(Ez, Ez_shift))));
	_mm512_store_ps(_doF.GetMagneticVoltageYComponent(index), result);
}

inline void FITTD3DLoopAVX512Sp::UpdateHz(index_t index)
{
	__m512 Hz = _mm512_load_ps(_doF.GetMagneticVoltageZComponent(index));
	__m512 Ex = _mm512_load_ps(_doF.GetElectricVoltageXComponent(index));
	__m512 Ey = _mm512_load_ps(_doF.GetElectricVoltageYComponent(index));
	__m512 Ex_shift = _mm512_load_ps(_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneYStep()));
	__m512 Ey_shift = _mm512_load_ps(_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_load_ps(_coefficients->GetCoefficientComponentZCurlXAtIndex(index));
	__m512 yCurl = _mm512_load_ps(_coefficients->GetCoefficientComponentZCurlYAtIndex(index));
	//_grid.SetHz(tempIndex,_grid.GetHz(tempIndex) - _grid.GetChx(tempIndex) * (_grid.GetEy(tempIndex + _grid.GetOneXStep()) -_grid.GetEy(tempIndex)) - _grid.GetChy(tempIndex) * (_grid.GetEx(tempIndex) - _grid.GetEx(tempIndex + _grid.GetOneYStep())));
	//_grid.SetHz(tempIndex,_grid.GetHz(tempIndex) - (_grid.GetChx(tempIndex) * (_grid.GetEy(tempIndex + _grid.GetOneXStep()) + _grid.GetEy(tempIndex)) - _grid.GetChy(tempIndex) * (_grid.GetEx(tempIndex) - _grid.GetEx(tempIndex + _grid.GetOneYStep()))));
	__m512 result = _mm512_sub_ps(Hz,
		_mm512_fmadd_ps(xCurl,
			_mm512_sub_ps(Ex, Ex_shift),
			_mm512_mul_ps(yCurl,
				_mm512_sub_ps(Ey_shift, Ey))));
	_mm512_store_ps(_doF.GetMagneticVoltageZComponent(index), result);
}

inline void FITTD3DLoopAVX512Sp::MaskedUpdateEx(index_t index, const unsigned short mask)
{
	__m512 Ex = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageXComponent(index));
	__m512 Hy = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageYComponent(index));
	__m512 Hz = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageZComponent(index));
	__m512 Hy_shift = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneZStep()));
	__m512 Hz_shift = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneYStep()));
	__m512 yCurl = _mm512_maskz_load_ps(mask, _coefficientsDual->GetCoefficientComponentXCurlYAtIndex(index));
	__m512 zCurl = _mm512_maskz_load_ps(mask, _coefficientsDual->GetCoefficientComponentXCurlZAtIndex(index));

	__m512 result = _mm512_add_ps(Ex,
		_mm512_fmadd_ps(zCurl,
			_mm512_sub_ps(Hz, Hz_shift),
			_mm512_mul_ps(yCurl,
				_mm512_sub_ps(Hy_shift, Hy))));
	_mm512_mask_store_ps(_doF.GetElectricVoltageXComponent(index), mask, result);
}

inline void FITTD3DLoopAVX512Sp::MaskedUpdateEy(index_t index, const unsigned short mask)
{
	__m512 Ey = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageYComponent(index));
	__m512 Hx = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageXComponent(index));
	__m512 Hz = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageZComponent(index));
	
	__m512 Hx_shift = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneZStep()));
	__m512 Hz_shift = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_maskz_load_ps(mask, _coefficientsDual->GetCoefficientComponentYCurlXAtIndex(index));
	__m512 zCurl = _mm512_maskz_load_ps(mask, _coefficientsDual->GetCoefficientComponentYCurlZAtIndex(index));
	//_grid.SetEy(tempIndex,_grid.GetEy(tempIndex) + _grid.GetCez(tempIndex) * (_grid.GetHx(tempIndex) - _grid.GetHx(tempIndex -1)) + _grid.GetCex(tempIndex) * (_grid.GetHz(tempIndex - _grid.GetOneXStep()) - _grid.GetHz(tempIndex)));
	__m512 result = _mm512_add_ps(Ey,
		_mm512_fmadd_ps(xCurl,
			_mm512_sub_ps(Hx, Hx_shift),
			_mm512_mul_ps(zCurl,
				_mm512_sub_ps(Hz_shift, Hz))));
	_mm512_mask_store_ps(_doF.GetElectricVoltageYComponent(index), mask, result);
}

inline void FITTD3DLoopAVX512Sp::MaskedUpdateEz(index_t index, const unsigned short mask)
{
	__m512 Ez = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageZComponent(index));
	__m512 Hx = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageXComponent(index));
	__m512 Hy = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageYComponent(index));
	__m512 Hx_shift = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneYStep()));
	__m512 Hy_shift = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_maskz_load_ps(mask, _coefficientsDual->GetCoefficientComponentZCurlXAtIndex(index));
	__m512 yCurl = _mm512_maskz_load_ps(mask, _coefficientsDual->GetCoefficientComponentZCurlYAtIndex(index));
	//_grid.SetEz(tempIndex, _grid.GetEz(tempIndex) + _grid.GetCex(tempIndex) * (_grid.GetHy(tempIndex) - _grid.GetHy(tempIndex - _grid.GetOneXStep())) + _grid.GetCey(tempIndex) * (_grid.GetHx(tempIndex - _grid.GetOneYStep()) - _grid.GetHx(tempIndex)));
	__m512 result = _mm512_add_ps(Ez,
		_mm512_fmadd_ps(yCurl,
			_mm512_sub_ps(Hy, Hy_shift),
			_mm512_mul_ps(xCurl,
				_mm512_sub_ps(Hx_shift, Hx))));
	_mm512_mask_store_ps(_doF.GetElectricVoltageZComponent(index), mask, result);
}

inline void FITTD3DLoopAVX512Sp::MaskedUpdateHx(index_t index, const unsigned short mask)
{
	__m512 Hx = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageXComponent(index));
	__m512 Ey = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageYComponent(index));
	__m512 Ez = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageZComponent(index));
	__m512 Ey_shift = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneZStep()));
	__m512 Ez_shift = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneYStep()));
	__m512 yCurl = _mm512_maskz_load_ps(mask, _coefficients->GetCoefficientComponentXCurlYAtIndex(index));
	__m512 zCurl = _mm512_maskz_load_ps(mask, _coefficients->GetCoefficientComponentXCurlZAtIndex(index));
	//_grid.SetHx(index,_grid.GetHx(tempIndex) - _grid.GetChy(tempIndex) * (_grid.GetEz(tempIndex + _grid.GetOneYStep()) - _grid.GetEz(tempIndex))- _grid.GetChz(tempIndex) * ( _grid.GetEy(tempIndex) - _grid.GetEy(tempIndex +1)));
	__m512 result = _mm512_sub_ps(Hx,
		_mm512_fmadd_ps(yCurl,
			_mm512_sub_ps(Ey, Ey_shift),
			_mm512_mul_ps(zCurl,
				_mm512_sub_ps(Ez_shift, Ez))));
	_mm512_mask_store_ps(_doF.GetMagneticVoltageXComponent(index), mask, result);
}

inline void FITTD3DLoopAVX512Sp::MaskedUpdateHy(index_t index, const unsigned short mask)
{
	__m512 Hy = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageYComponent(index));
	__m512 Ex = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageXComponent(index));
	__m512 Ez = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageZComponent(index));
	__m512 Ex_shift = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneZStep()));
	__m512 Ez_shift = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_maskz_load_ps(mask, _coefficients->GetCoefficientComponentYCurlXAtIndex(index));
	__m512 zCurl = _mm512_maskz_load_ps(mask, _coefficients->GetCoefficientComponentYCurlZAtIndex(index));
	//_grid.SetHy(tempIndex, _grid.GetHy(tempIndex) - _grid.GetChz(tempIndex) * (_grid.GetEx(tempIndex +1) - _grid.GetEx(tempIndex)) - _grid.GetChx(tempIndex) * (_grid.GetEz(tempIndex) - _grid.GetEz(tempIndex + _grid.GetOneXStep())) );
	__m512 result = _mm512_sub_ps(Hy,
		_mm512_fmadd_ps(xCurl,
			_mm512_sub_ps(Ex_shift, Ex),
			_mm512_mul_ps(zCurl,
				_mm512_sub_ps(Ez, Ez_shift))));
	//result = _mm512_sub_ps(result, t2);
	_mm512_mask_store_ps(_doF.GetMagneticVoltageYComponent(index), mask, result);
}

inline void FITTD3DLoopAVX512Sp::MaskedUpdateHz(index_t index, const unsigned short mask)
{
	__m512 Hz = _mm512_maskz_load_ps(mask, _doF.GetMagneticVoltageZComponent(index));
	__m512 Ex = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageXComponent(index));
	__m512 Ey = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageYComponent(index));
	__m512 Ex_shift = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneYStep()));
	__m512 Ey_shift = _mm512_maskz_load_ps(mask, _doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneXStep()));
	__m512 xCurl = _mm512_maskz_load_ps(mask, _coefficients->GetCoefficientComponentZCurlXAtIndex(index));
	__m512 yCurl = _mm512_maskz_load_ps(mask, _coefficients->GetCoefficientComponentZCurlYAtIndex(index));
	//_grid.SetHz(tempIndex,_grid.GetHz(tempIndex) - _grid.GetChx(tempIndex) * (_grid.GetEy(tempIndex + _grid.GetOneXStep()) -_grid.GetEy(tempIndex)) - _grid.GetChy(tempIndex) * (_grid.GetEx(tempIndex) - _grid.GetEx(tempIndex + _grid.GetOneYStep())));
	//_grid.SetHz(tempIndex,_grid.GetHz(tempIndex) - (_grid.GetChx(tempIndex) * (_grid.GetEy(tempIndex + _grid.GetOneXStep()) + _grid.GetEy(tempIndex)) - _grid.GetChy(tempIndex) * (_grid.GetEx(tempIndex) - _grid.GetEx(tempIndex + _grid.GetOneYStep()))));
	__m512 result = _mm512_sub_ps(Hz,
		_mm512_fmadd_ps(xCurl,
			_mm512_sub_ps(Ex, Ex_shift),
			_mm512_mul_ps(yCurl,
				_mm512_sub_ps(Ey_shift, Ey))));
	_mm512_mask_store_ps(_doF.GetMagneticVoltageZComponent(index), mask, result);
}
