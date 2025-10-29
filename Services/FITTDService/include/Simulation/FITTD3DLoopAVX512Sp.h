// @otlicense

#pragma once
#include "SystemDependencies/SystemDependentDefines.h"
#include "Simulation/FITTD3DLoop.h"
#include "Grid/Grid.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include "Grid/IntrinsicMasks.h"

class FITTD3DLoopAVX512Sp : public FITTD3DLoop<float>
{
private:
	using FITTD3DLoop<float>::_grid;
	using FITTD3DLoop<float>::_coefficients;
	using FITTD3DLoop<float>::_coefficientsDual;
	using FITTD3DLoop<float>::_doF;

	unsigned short _maskBegin = 0i16;
	unsigned short _maskBeginNoBeginningExclude = 0i16;
	unsigned short _maskEnd = 0i16;

	void SetBitInMask(unsigned short& mask, int bit); 

	inline void UpdateEx(index_t index);
	inline void UpdateEy(index_t index);
	inline void UpdateEz(index_t index);
	inline void UpdateHx(index_t index);
	inline void UpdateHy(index_t index);
	inline void UpdateHz(index_t index);

	inline void MaskedUpdateEx(index_t index, const unsigned short mask);
	inline void MaskedUpdateEy(index_t index, const unsigned short mask);
	inline void MaskedUpdateEz(index_t index, const unsigned short mask);
	inline void MaskedUpdateHx(index_t index, const unsigned short mask);
	inline void MaskedUpdateHy(index_t index, const unsigned short mask);
	inline void MaskedUpdateHz(index_t index, const unsigned short mask);

public:
	FITTD3DLoopAVX512Sp(Grid<float> *grid, CurlCoefficients3D<float> *coefficients, CurlCoefficients3D<float> *coefficientsDual);
	FITTD3DLoopAVX512Sp(FITTD3DLoopAVX512Sp &other) = delete;
	FITTD3DLoopAVX512Sp & operator=(FITTD3DLoopAVX512Sp &other) = delete;

	virtual void UpdateSimulationValuesE() override;
	virtual void UpdateSimulationValuesH() override;
};

#include "Simulation/FITTD3DLoopAVX512Sp.hpp"
