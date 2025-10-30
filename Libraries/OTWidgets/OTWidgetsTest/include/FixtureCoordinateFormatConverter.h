// @otlicense
// File: FixtureCoordinateFormatConverter.h
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
