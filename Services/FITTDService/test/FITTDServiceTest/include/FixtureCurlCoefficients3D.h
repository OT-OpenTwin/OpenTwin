#pragma once
#include "Grid/Grid.h"
#include "CurlCoefficients/CurlCoefficients3D.h"
#include "gtest/gtest.h"

class FixtureCurlCoefficients3D : public testing::Test
{
private:
	CurlCoefficients3D<float> * coefficients = nullptr;
	Grid<float> * grid = nullptr;

public:
	~FixtureCurlCoefficients3D() ;
	CurlCoefficients3D<float> * GetCurlCoefficients3DSp() const { return coefficients; };
	Grid<float> * GetGridSp()const { return grid; };

	void SetCorrectUniformPECCubeGrid();
	void SetCorrectUniformCubeDualGrid();
	void SetCorrectUniformCubeGrid();
};