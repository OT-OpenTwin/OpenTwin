#pragma once
#include "Simulation/FITTD3DLoopAVX2.h"
#include <cassert>

inline FITTD3DLoopAVX2::FITTD3DLoopAVX2(Grid<float>* grid, CurlCoefficients3D<float>* coefficients, CurlCoefficients3D<float>* coefficientsDual)
	: FITTD3DLoop<float>(grid, coefficients, coefficientsDual)
{
	assert(_grid->GetAlignment() == AVX2);

		_maskBeginNoBeginningExclude = new int[_grid->GetLengthOfIntrinsicVector()];
	_maskBegin = new int[_grid->GetLengthOfIntrinsicVector()];
	_maskEnd = new int[_grid->GetLengthOfIntrinsicVector()];

	int includeValue = -1;
	int excludeValue = 1;

	for (int i = 0; i < _grid->GetLengthOfIntrinsicVector(); i++)
	{
		_maskBeginNoBeginningExclude[i] = excludeValue;
		_maskBegin[i] = excludeValue;
		_maskEnd[i] = excludeValue;
	}

	const IntrinsicMasks * masks = _grid->GetIntrinsicMasks();
	for (int i = 0; i < _grid->GetLengthOfIntrinsicVector(); i++)
	{
		if (masks->GetMaskBeginBit(i) == 1)
		{
			_maskBegin[i] = includeValue;
		}
		if (masks->GetMaskBeginNoBeginningExcludeBit(i))
		{
			_maskBeginNoBeginningExclude[i] = includeValue;
		}
		if (masks->GetMaskEndBit(i))
		{
			_maskEnd[i] = includeValue;
		}
	}
};

void FITTD3DLoopAVX2::UpdateSimulationValuesE()
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

void FITTD3DLoopAVX2::UpdateSimulationValuesH()
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

inline void FITTD3DLoopAVX2::UpdateEx(index_t index)
{
	__m256 Ex = _mm256_load_ps(_doF.GetElectricVoltageXComponent(index));
	__m256 Hy = _mm256_load_ps(_doF.GetMagneticVoltageYComponent(index));
	__m256 Hz = _mm256_load_ps(_doF.GetMagneticVoltageZComponent(index));
	__m256 Hy_shift = _mm256_load_ps(_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneZStep()));
	__m256 Hz_shift = _mm256_load_ps(_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneYStep()));
	__m256 yCurl = _mm256_load_ps(_coefficientsDual->GetCoefficientComponentXCurlYAtIndex(index));
	__m256 zCurl = _mm256_load_ps(_coefficientsDual->GetCoefficientComponentXCurlZAtIndex(index));
	//_grid.SetEx(tempIndex, _grid.GetEx(tempIndex) + _grid.GetCey(tempIndex) * (_grid.GetHz(tempIndex) - _grid.GetHz(tempIndex - _grid.GetOneYStep())) + _grid.GetCez(tempIndex) * (_grid.GetHy(tempIndex - 1) - _grid.GetHy(tempIndex)));
	__m256 result = _mm256_add_ps(Ex,
		_mm256_fmadd_ps(zCurl,
			_mm256_sub_ps(Hz, Hz_shift),
			_mm256_mul_ps(yCurl,
				_mm256_sub_ps(Hy_shift, Hy))));
	_mm256_store_ps(_doF.GetElectricVoltageXComponent(index), result);
}

inline void FITTD3DLoopAVX2::UpdateEy(index_t index)
{
	__m256 Ey = _mm256_load_ps(_doF.GetElectricVoltageYComponent(index));
	__m256 Hx = _mm256_load_ps(_doF.GetMagneticVoltageXComponent(index));
	__m256 Hz = _mm256_load_ps(_doF.GetMagneticVoltageZComponent(index));
	__m256 Hx_shift = _mm256_load_ps(_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneZStep()));
	__m256 Hz_shift = _mm256_load_ps(_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneXStep()));
	__m256 xCurl = _mm256_load_ps(_coefficientsDual->GetCoefficientComponentYCurlXAtIndex(index));
	__m256 zCurl = _mm256_load_ps(_coefficientsDual->GetCoefficientComponentYCurlZAtIndex(index));
	//_grid.SetEy(tempIndex,_grid.GetEy(tempIndex) + _grid.GetCez(tempIndex) * (_grid.GetHx(tempIndex) - _grid.GetHx(tempIndex -1)) + _grid.GetCex(tempIndex) * (_grid.GetHz(tempIndex - _grid.GetOneXStep()) - _grid.GetHz(tempIndex)));
	__m256 result = _mm256_add_ps(Ey,
		_mm256_fmadd_ps(xCurl,
			_mm256_sub_ps(Hx, Hx_shift),
			_mm256_mul_ps(zCurl,
				_mm256_sub_ps(Hz_shift, Hz))));
	_mm256_store_ps(_doF.GetElectricVoltageYComponent(index), result);
}

inline void FITTD3DLoopAVX2::UpdateEz(index_t index)
{
	__m256 Ez = _mm256_load_ps(_doF.GetElectricVoltageZComponent(index));
	__m256 Hx = _mm256_load_ps(_doF.GetMagneticVoltageXComponent(index));
	__m256 Hy = _mm256_load_ps(_doF.GetMagneticVoltageYComponent(index));
	__m256 Hx_shift = _mm256_load_ps(_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneYStep()));
	__m256 Hy_shift = _mm256_load_ps(_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneXStep()));
	__m256 xCurl = _mm256_load_ps(_coefficientsDual->GetCoefficientComponentZCurlXAtIndex(index));
	__m256 yCurl = _mm256_load_ps(_coefficientsDual->GetCoefficientComponentZCurlYAtIndex(index));
	//_grid.SetEz(tempIndex, _grid.GetEz(tempIndex) + _grid.GetCex(tempIndex) * (_grid.GetHy(tempIndex) - _grid.GetHy(tempIndex - _grid.GetOneXStep())) + _grid.GetCey(tempIndex) * (_grid.GetHx(tempIndex - _grid.GetOneYStep()) - _grid.GetHx(tempIndex)));
	__m256 result = _mm256_add_ps(Ez,
		_mm256_fmadd_ps(yCurl,
			_mm256_sub_ps(Hy, Hy_shift),
			_mm256_mul_ps(xCurl,
				_mm256_sub_ps(Hx_shift, Hx))));
	_mm256_store_ps(_doF.GetElectricVoltageZComponent(index), result);
}

inline void FITTD3DLoopAVX2::UpdateHx(index_t index)
{
	__m256 Hx = _mm256_load_ps(_doF.GetMagneticVoltageXComponent(index));
	__m256 Ey = _mm256_load_ps(_doF.GetElectricVoltageYComponent(index));
	__m256 Ez = _mm256_load_ps(_doF.GetElectricVoltageZComponent(index));
	__m256 Ey_shift = _mm256_load_ps(_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneZStep()));
	__m256 Ez_shift = _mm256_load_ps(_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneYStep()));
	__m256 yCurl = _mm256_load_ps(_coefficients->GetCoefficientComponentXCurlYAtIndex(index));
	__m256 zCurl = _mm256_load_ps(_coefficients->GetCoefficientComponentXCurlZAtIndex(index));
	//_grid.SetHx(index,_grid.GetHx(tempIndex) - _grid.GetChy(tempIndex) * (_grid.GetEz(tempIndex + _grid.GetOneYStep()) - _grid.GetEz(tempIndex))- _grid.GetChz(tempIndex) * ( _grid.GetEy(tempIndex) - _grid.GetEy(tempIndex +1)));
	__m256 result = _mm256_sub_ps(Hx,
		_mm256_fmadd_ps(yCurl,
			_mm256_sub_ps(Ey, Ey_shift),
			_mm256_mul_ps(zCurl,
				_mm256_sub_ps(Ez_shift, Ez))));

	_mm256_store_ps(_doF.GetMagneticVoltageXComponent(index), result);
}

inline void FITTD3DLoopAVX2::UpdateHy(index_t index)
{
	__m256 Hy = _mm256_load_ps(_doF.GetMagneticVoltageYComponent(index));
	__m256 Ex = _mm256_load_ps(_doF.GetElectricVoltageXComponent(index));
	__m256 Ez = _mm256_load_ps(_doF.GetElectricVoltageZComponent(index));
	__m256 Ex_shift = _mm256_load_ps(_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneZStep()));
	__m256 Ez_shift = _mm256_load_ps(_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneXStep()));
	__m256 xCurl = _mm256_load_ps(_coefficients->GetCoefficientComponentYCurlXAtIndex(index));
	__m256 zCurl = _mm256_load_ps(_coefficients->GetCoefficientComponentYCurlZAtIndex(index));
	//_grid.SetHy(tempIndex, _grid.GetHy(tempIndex) - _grid.GetChz(tempIndex) * (_grid.GetEx(tempIndex +1) - _grid.GetEx(tempIndex)) - _grid.GetChx(tempIndex) * (_grid.GetEz(tempIndex) - _grid.GetEz(tempIndex + _grid.GetOneXStep())) );
	__m256 result = _mm256_sub_ps(Hy,
		_mm256_fmadd_ps(xCurl,
			_mm256_sub_ps(Ex_shift, Ex),
			_mm256_mul_ps(zCurl,
				_mm256_sub_ps(Ez, Ez_shift))));

	_mm256_store_ps(_doF.GetMagneticVoltageYComponent(index), result);
}

inline void FITTD3DLoopAVX2::UpdateHz(index_t index)
{
	__m256 Hz = _mm256_load_ps(_doF.GetMagneticVoltageZComponent(index));
	__m256 Ex = _mm256_load_ps(_doF.GetElectricVoltageXComponent(index));
	__m256 Ey = _mm256_load_ps(_doF.GetElectricVoltageYComponent(index));
	__m256 Ex_shift = _mm256_load_ps(_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneYStep()));
	__m256 Ey_shift = _mm256_load_ps(_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneXStep()));
	__m256 xCurl = _mm256_load_ps(_coefficients->GetCoefficientComponentZCurlXAtIndex(index));
	__m256 yCurl = _mm256_load_ps(_coefficients->GetCoefficientComponentZCurlYAtIndex(index));
	//_grid.SetHz(tempIndex,_grid.GetHz(tempIndex) - _grid.GetChx(tempIndex) * (_grid.GetEy(tempIndex + _grid.GetOneXStep()) -_grid.GetEy(tempIndex)) - _grid.GetChy(tempIndex) * (_grid.GetEx(tempIndex) - _grid.GetEx(tempIndex + _grid.GetOneYStep())));
	//_grid.SetHz(tempIndex,_grid.GetHz(tempIndex) - (_grid.GetChx(tempIndex) * (_grid.GetEy(tempIndex + _grid.GetOneXStep()) + _grid.GetEy(tempIndex)) - _grid.GetChy(tempIndex) * (_grid.GetEx(tempIndex) - _grid.GetEx(tempIndex + _grid.GetOneYStep()))));
	__m256 result = _mm256_sub_ps(Hz,
		_mm256_fmadd_ps(xCurl,
			_mm256_sub_ps(Ex, Ex_shift),
			_mm256_mul_ps(yCurl,
				_mm256_sub_ps(Ey_shift, Ey))));
	
	_mm256_store_ps(_doF.GetMagneticVoltageZComponent(index), result);
}

inline void FITTD3DLoopAVX2::MaskedUpdateEx(index_t index, int*  maskArr)
{
	__m256i mask = _mm256_load_si256((__m256i *)maskArr);
	__m256 Ex = _mm256_maskload_ps(_doF.GetElectricVoltageXComponent(index), mask);
	__m256 Hy = _mm256_maskload_ps(_doF.GetMagneticVoltageYComponent(index), mask);
	__m256 Hz = _mm256_maskload_ps(_doF.GetMagneticVoltageZComponent(index), mask);
	__m256 Hy_shift = _mm256_maskload_ps(_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneZStep()), mask);
	__m256 Hz_shift = _mm256_maskload_ps(_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneYStep()), mask);
	__m256 yCurl = _mm256_maskload_ps(_coefficientsDual->GetCoefficientComponentXCurlYAtIndex(index), mask);
	__m256 zCurl = _mm256_maskload_ps(_coefficientsDual->GetCoefficientComponentXCurlZAtIndex(index), mask);

	__m256 result = _mm256_add_ps(Ex,
		_mm256_fmadd_ps(zCurl,
			_mm256_sub_ps(Hz, Hz_shift),
			_mm256_mul_ps(yCurl,
				_mm256_sub_ps(Hy_shift, Hy))));
	_mm256_maskstore_ps(_doF.GetElectricVoltageXComponent(index),mask, result);
}

inline void FITTD3DLoopAVX2::MaskedUpdateEy(index_t index, int*  maskArr)
{
	__m256i mask = _mm256_load_si256((__m256i *)maskArr);
	__m256 Ey = _mm256_maskload_ps(_doF.GetElectricVoltageYComponent(index), mask);
	__m256 Hx = _mm256_maskload_ps(_doF.GetMagneticVoltageXComponent(index), mask);
	__m256 Hz = _mm256_maskload_ps(_doF.GetMagneticVoltageZComponent(index), mask);
	__m256 Hx_shift = _mm256_maskload_ps(_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneZStep()), mask);
	__m256 Hz_shift = _mm256_maskload_ps(_doF.GetMagneticVoltageZComponent(index - _grid->GetPlusOneXStep()), mask);
	__m256 xCurl = _mm256_maskload_ps( _coefficientsDual->GetCoefficientComponentYCurlXAtIndex(index), mask);
	__m256 zCurl = _mm256_maskload_ps(_coefficientsDual->GetCoefficientComponentYCurlZAtIndex(index), mask);

	__m256 result = _mm256_add_ps(Ey,
		_mm256_fmadd_ps(xCurl,
			_mm256_sub_ps(Hx, Hx_shift),
			_mm256_mul_ps(zCurl,
				_mm256_sub_ps(Hz_shift, Hz))));
	_mm256_maskstore_ps(_doF.GetElectricVoltageYComponent(index), mask, result);
}

inline void FITTD3DLoopAVX2::MaskedUpdateEz(index_t index, int*  maskArr)
{
	__m256i mask = _mm256_load_si256((__m256i *)maskArr);
	__m256 Ez = _mm256_maskload_ps(_doF.GetElectricVoltageZComponent(index), mask);
	__m256 Hx = _mm256_maskload_ps(_doF.GetMagneticVoltageXComponent(index), mask);
	__m256 Hy = _mm256_maskload_ps(_doF.GetMagneticVoltageYComponent(index), mask);
	__m256 Hx_shift = _mm256_maskload_ps(_doF.GetMagneticVoltageXComponent(index - _grid->GetPlusOneYStep()), mask);
	__m256 Hy_shift = _mm256_maskload_ps(_doF.GetMagneticVoltageYComponent(index - _grid->GetPlusOneXStep()), mask);
	__m256 xCurl = _mm256_maskload_ps(_coefficientsDual->GetCoefficientComponentZCurlXAtIndex(index), mask);
	__m256 yCurl = _mm256_maskload_ps(_coefficientsDual->GetCoefficientComponentZCurlYAtIndex(index), mask);

	__m256 result = _mm256_add_ps(Ez,
		_mm256_fmadd_ps(yCurl,
			_mm256_sub_ps(Hy, Hy_shift),
			_mm256_mul_ps(xCurl,
				_mm256_sub_ps(Hx_shift, Hx))));
	_mm256_maskstore_ps(_doF.GetElectricVoltageZComponent(index), mask, result);
}

inline void FITTD3DLoopAVX2::MaskedUpdateHx(index_t index, int*  maskArr)
{
	__m256i mask = _mm256_load_si256((__m256i *)maskArr);
	__m256 Hx = _mm256_maskload_ps(_doF.GetMagneticVoltageXComponent(index), mask);
	__m256 Ey = _mm256_maskload_ps(_doF.GetElectricVoltageYComponent(index), mask);
	__m256 Ez = _mm256_maskload_ps(_doF.GetElectricVoltageZComponent(index), mask);
	__m256 Ey_shift = _mm256_maskload_ps(_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneZStep()), mask);
	__m256 Ez_shift = _mm256_maskload_ps(_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneYStep()), mask);
	__m256 yCurl = _mm256_maskload_ps(_coefficients->GetCoefficientComponentXCurlYAtIndex(index), mask);
	__m256 zCurl = _mm256_maskload_ps(_coefficients->GetCoefficientComponentXCurlZAtIndex(index), mask);

	__m256 result = _mm256_sub_ps(Hx,
		_mm256_fmadd_ps(yCurl,
			_mm256_sub_ps(Ey, Ey_shift),
			_mm256_mul_ps(zCurl,
				_mm256_sub_ps(Ez_shift, Ez))));

	_mm256_maskstore_ps(_doF.GetMagneticVoltageXComponent(index),mask, result);
}

inline void FITTD3DLoopAVX2::MaskedUpdateHy(index_t index, int*  maskArr)
{
	__m256i mask = _mm256_load_si256((__m256i *)maskArr);
	__m256 Hy = _mm256_maskload_ps(_doF.GetMagneticVoltageYComponent(index), mask);
	__m256 Ex = _mm256_maskload_ps(_doF.GetElectricVoltageXComponent(index), mask);
	__m256 Ez = _mm256_maskload_ps(_doF.GetElectricVoltageZComponent(index), mask);
	__m256 Ex_shift = _mm256_maskload_ps(_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneZStep()), mask);
	__m256 Ez_shift = _mm256_maskload_ps(_doF.GetElectricVoltageZComponent(index + _grid->GetPlusOneXStep()), mask);
	__m256 xCurl = _mm256_maskload_ps(_coefficients->GetCoefficientComponentYCurlXAtIndex(index), mask);
	__m256 zCurl = _mm256_maskload_ps(_coefficients->GetCoefficientComponentYCurlZAtIndex(index), mask);

	__m256 result = _mm256_sub_ps(Hy,
		_mm256_fmadd_ps(xCurl,
			_mm256_sub_ps(Ex_shift, Ex),
			_mm256_mul_ps(zCurl,
				_mm256_sub_ps(Ez, Ez_shift))));

	_mm256_maskstore_ps(_doF.GetMagneticVoltageYComponent(index),mask, result);
}

inline void FITTD3DLoopAVX2::MaskedUpdateHz(index_t index, int*  maskArr)
{
	__m256i mask = _mm256_load_si256((__m256i *)maskArr);
	__m256 Hz = _mm256_maskload_ps(_doF.GetMagneticVoltageZComponent(index), mask);
	__m256 Ex = _mm256_maskload_ps(_doF.GetElectricVoltageXComponent(index), mask);
	__m256 Ey = _mm256_maskload_ps(_doF.GetElectricVoltageYComponent(index), mask);
	__m256 Ex_shift = _mm256_maskload_ps(_doF.GetElectricVoltageXComponent(index + _grid->GetPlusOneYStep()), mask);
	__m256 Ey_shift = _mm256_maskload_ps(_doF.GetElectricVoltageYComponent(index + _grid->GetPlusOneXStep()), mask);
	__m256 xCurl = _mm256_maskload_ps(_coefficients->GetCoefficientComponentZCurlXAtIndex(index), mask);
	__m256 yCurl = _mm256_maskload_ps(_coefficients->GetCoefficientComponentZCurlYAtIndex(index), mask);

	__m256 result = _mm256_sub_ps(Hz,
		_mm256_fmadd_ps(xCurl,
			_mm256_sub_ps(Ex, Ex_shift),
			_mm256_mul_ps(yCurl,
				_mm256_sub_ps(Ey_shift, Ey))));

	_mm256_maskstore_ps(_doF.GetMagneticVoltageZComponent(index),mask, result);
}

