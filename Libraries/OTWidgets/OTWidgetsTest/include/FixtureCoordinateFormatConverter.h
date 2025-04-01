#pragma once
#include "OTWidgets/PlotDatasetData.h"
#include <vector>
#include "OTCore/ComplexNumberContainer.h"

#include <numbers>
#include <gtest/gtest.h>

class FixtureCoordinateFormatConverter : public testing::Test
{
public:
	ot::PlotDatasetData getDatasetRealOnly()
	{
		std::vector<double> xData{ 1,2,3,4,5 };
		ot::ComplexNumberContainerCartesian* yData = new ot::ComplexNumberContainerCartesian();
		yData->m_real = { 11,12,13,14,15 };
		ot::PlotDatasetData dataset(std::move(xData), yData);
		return dataset;
	}
	ot::PlotDatasetData getDatasetImagOnly()
	{
		std::vector<double> xData{ 1,2,3,4,5 };
		ot::ComplexNumberContainerCartesian* yData = new ot::ComplexNumberContainerCartesian();
		yData->m_imag = { 11,12,13,14,15 };
		ot::PlotDatasetData dataset(std::move(xData), yData);
		return dataset;
	}
	ot::PlotDatasetData getDatasetMagOnly()
	{
		std::vector<double> xData{ 1,2,3,4,5 };
		ot::ComplexNumberContainerPolar* yData = new ot::ComplexNumberContainerPolar();
		yData->m_magnitudes= { 11,12,13,14,15 };
		ot::PlotDatasetData dataset(std::move(xData), yData);
		return dataset;
	}
	ot::PlotDatasetData getDatasetPhaseOnly()
	{
		std::vector<double> xData{ 1,2,3,4,5 };
		ot::ComplexNumberContainerPolar* yData = new ot::ComplexNumberContainerPolar();
		yData->m_phases = { 11,12,13,14,15 };
		ot::PlotDatasetData dataset(std::move(xData), yData);
		return dataset;
	}
	ot::PlotDatasetData getDatasetComplexCartesian()
	{
		std::vector<double> xData{ 1,2,3,4,5 };
		ot::ComplexNumberContainerCartesian* yData = new ot::ComplexNumberContainerCartesian();
		yData->m_real = getExpectedComplexRe();
		yData->m_imag = getExpectedComplexIm();
		ot::PlotDatasetData dataset(std::move(xData), yData);
		return dataset;
	}
	
	ot::PlotDatasetData getDatasetComplexPolar()
	{
		std::vector<double> xData{ 1,2,3,4,5 };
		ot::ComplexNumberContainerPolar* yData = new ot::ComplexNumberContainerPolar();
		yData->m_magnitudes = getExpectedComplexMag();
		yData->m_phases = getExpectedComplexPhase();
		ot::PlotDatasetData dataset(std::move(xData), yData);
		return dataset;
	}

	std::vector<double> getExpectedComplexRe()
	{
		return {1,0,-1,0,2};
	}

	std::vector<double> getExpectedComplexIm()
	{
		return { 0,1,0,-1,2};
	}

	std::vector<double> getExpectedComplexMag()
	{
		return { 1,1,1,1,2 * sqrt(2) };
	}

	std::vector<double> getExpectedComplexPhase()
	{
		return { 0,90,180,-90,45 };
	}
};
