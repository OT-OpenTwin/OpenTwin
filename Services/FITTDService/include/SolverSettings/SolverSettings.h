// @otlicense
// File: SolverSettings.h
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
#include "SystemDependencies/Allignments.h"
#include "SolverSettings/SolverVersion.h"
#include "SystemDependencies/SystemProperties.h"

#include <string>
class SolverSettings
{
public:
	enum Dimension {D3, UNKOWN};
	enum VolumeSelection {Full, UNKNOWN};
	std::map<Alignment, std::string> alignmentName{ {CacheLine64,"None" }, {AVX, "AVX"}, {AVX2, "AVX2"}, {AVX512, "AVX512"} };

	SolverSettings() {};

	SolverSettings(bool debug, std::string dimension, std::string meshName, int timeSteps, std::string volumeSelection)
		: _debug(debug), _dimension(dimension), _meshName(meshName), _timeSteps(timeSteps),_volumeSelection(volumeSelection) 
	{
		auto system = SystemProperties::GetInstance();
		if (system.CPUSupportsAVX512())
		{
			_activeAlignment= AVX512; //ToDo: Property for it
		}
		else if(system.CPUSupportsAVX2())
		{
			_activeAlignment = AVX2;
		}
		else
		{
			_activeAlignment = CacheLine64;
		}
	};

	const bool GetDebug(void) const { return _debug; }
	const std::string GetMeshName(void) const { return _meshName; }
	const Alignment GetAlignment(void) const { return _activeAlignment; }
	const int GetSimulationSteps(void) const { return _timeSteps; }
	void SetActiveNumberOfThreads(int threads) { _activeThreads = threads; }



	std::string Print()
	{
		std::string settings =
			"Running solver version " + SolverVersion + " with solver settings: \n"
			"Calculating " + std::to_string(_timeSteps) + " time steps. \n" +
			"Vectorization: " + alignmentName[_activeAlignment] + "\n" +
			"Active threads: " + std::to_string(_activeThreads) + "\n";

		return settings;
	}

private:
	bool _debug;
	std::string _dimension;
	std::string _meshName;
	int _timeSteps;
	std::string _volumeSelection;

	int _activeThreads;
	Alignment _activeAlignment;
};
 