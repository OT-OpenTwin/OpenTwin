// @otlicense
// File: FDTDSolver.cpp
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

#include "FDTDSolver.h"

#include "Application.h"
#include "OTServiceFoundation/ModelComponent.h"


#include "OTModelEntities/EntitySolver.h"
#include "OTModelEntities/EntityMeshCartesian.h"
#include "OTModelEntities/EntityUnits.h"
#include "OTModelEntities/EntityBinaryData.h"
#include "OTModelEntities/EntityInformation.h"
#include "OTModelEntities/EntityResultVtkComplex.h"
#include "OTModelEntities/EntityResultVtkTime.h"
#include "OTModelEntities/EntityVisVtkVectorVolumeComplex.h"
#include "OTModelEntities/EntityVisVtkVectorVolumeTime.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityAPI.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include <vector>
#include <charconv>
#include <regex>

FDTDSolver::FDTDSolver(Application* _application, EntityBase* _solverEntity, EntityMeshCartesian* _meshEntity, const std::string& _openEMSPath, const std::string& _tempDirPath)
	: application(_application), solverEntity(_solverEntity), meshEntity(_meshEntity), openEMSPath(_openEMSPath), tempDirPath(_tempDirPath), entityUnits(nullptr)
{
	assert(application != nullptr);
	assert(solverEntity != nullptr);
	assert(meshEntity != nullptr);
	assert(!openEMSPath.empty());
	assert(!_tempDirPath.empty());
}


std::string FDTDSolver::generateRunCommand()
{
	std::stringstream runCommand;
	
	addPreparationData(runCommand);
	addUnits(runCommand);

	// addSolverSetup
	// addMesh
	// addGeometry
	// addPorts
	// addBoundaries
	// addMonitors


	std::string text =
		"# waveguide dimensions\n"
		"# WR42\n"
		"a = 10.7;   #waveguide width\n"
		"b = 4.3;    #waveguide height\n"
		"length = 50.0;\n"
		"\n"
		"# frequency range of interest\n"
		"f_start = 20e9;\n"
		"f_0     = 24e9;\n"
		"f_stop  = 26e9;\n"
		"lambda0 = C0/f_0/gunit;\n"
		"\n"
		"#waveguide TE-mode definition\n"
		"TE_mode = 'TE10';\n"
		"\n"
		"#targeted mesh resolution\n"
		"mesh_res = lambda0/30\n"
		"\n"
		"### Setup FDTD parameter & excitation function\n"
		"FDTD = openEMS(NrTS=1e4);\n"
		"FDTD.SetGaussExcite(0.5*(f_start+f_stop),0.5*(f_stop-f_start));\n"
		"\n"
		"# boundary conditions\n"
		"FDTD.SetBoundaryCond([0, 0, 0, 0, 3, 3]);\n"
		"\n"
		"### Setup geometry & mesh\n"
		"CSX = ContinuousStructure()\n"
		"FDTD.SetCSX(CSX)\n"
		"mesh = CSX.GetGrid()\n"
		"mesh.SetDeltaUnit(gunit)\n"
		"\n"
		"mesh.AddLine('x', [0, a])\n"
		"mesh.AddLine('y', [0, b])\n"
		"mesh.AddLine('z', [0, length])\n"
		"\n"
		"## Apply the waveguide port\n"
		"ports = []\n"
		"start=[0, 0, 10*mesh_res];\n"
		"stop =[a, b, 15*mesh_res];\n"
		"mesh.AddLine('z', [start[2], stop[2]])\n"
		"ports.append(FDTD.AddRectWaveGuidePort( 0, start, stop, 'z', a*gunit, b*gunit, TE_mode, 1))\n"
		"\n"
		"start=[0, 0, length-10*mesh_res];\n"
		"stop =[a, b, length-15*mesh_res];\n"
		"mesh.AddLine('z', [start[2], stop[2]])\n"
		"ports.append(FDTD.AddRectWaveGuidePort( 1, start, stop, 'z', a*gunit, b*gunit, TE_mode))\n"
		"\n"
		"mesh.SmoothMeshLines('all', mesh_res, ratio=1.4)\n"
		"\n"
		"### Define dump box...\n"
		"#Et = CSX.AddDump('E-Field', dump_type=10, dump_mode=3, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop], sub_sampling=[2,2,2])\n"
		"Ef = CSX.AddDump('E-Field Complex', dump_type=10, dump_mode=3, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop])\n"
		"Et = CSX.AddDump('E-Field Time', dump_type=0, dump_mode=3, file_type=0, sub_sampling=[2,2,2])\n"
		"start = [0, 0, 0];\n"
		"stop  = [a, b, length];\n"
		"Ef.AddBox(start, stop);\n"
		"Et.AddBox(start, stop);\n"
		"\n";
		//"### Postprocessing & plotting\n"
		//"freq = np.linspace(f_start,f_stop,201)\n"
		//"port.CalcPort(Sim_Path, freq)\n"
		//"\n"
		//"s11 = ports[0].uf_ref / ports[0].uf_inc\n"
		//"s21 = ports[1].uf_ref / ports[0].uf_inc\n"
		//"ZL  = ports[0].uf_tot / ports[0].if_tot\n"
		//"ZL_a = ports[0].ZL # analytic waveguide impedance\n"
		//"\n";

	runCommand << text;

	addSolverRun(runCommand);
	// addPostprocessing

	return runCommand.str();
}

void FDTDSolver::addPreparationData(std::stringstream &runCommand)
{
	runCommand << "import os, tempfile\n";
	runCommand << "import numpy as np\n";
	runCommand << "os.environ[\"OPENEMS_INSTALL_PATH\"] = \"" << escapeBackslashes(openEMSPath) << "\"\n";
	runCommand << "from CSXCAD  import ContinuousStructure\n";
	runCommand << "from openEMS import openEMS\n";
	runCommand << "from openEMS.physical_constants import *\n";
	runCommand << "Sim_Path = \"" << escapeBackslashes(tempDirPath) << "\"\n";
}

void FDTDSolver::addUnits(std::stringstream& runCommand)
{
	ot::EntityInformation unitsInfo;
	ot::ModelServiceAPI::getEntityInformation("Units", unitsInfo);

	entityUnits = dynamic_cast<EntityUnits*>(ot::EntityAPI::readEntityFromEntityIDandVersion(unitsInfo.getEntityID(), unitsInfo.getEntityVersion()));
	if (entityUnits == nullptr)
	{
		throw("ERROR: Unable to read units\n");
	}
	
	runCommand << "gunit = " << entityUnits->getScaleToSIDimension() << "\n";
	runCommand << "funit = " << entityUnits->getScaleToSIFrequency() << "\n";
	runCommand << "tunit = " << entityUnits->getScaleToSITime() << "\n";
}

void FDTDSolver::addSolverRun(std::stringstream& runCommand)
{
	EntityPropertiesBoolean* debug = dynamic_cast<EntityPropertiesBoolean*>(solverEntity->getProperties().getProperty("Debug"));
	assert(debug != nullptr);

	bool debugFlag = false;
	if (debug != nullptr) debugFlag = debug->getValue();

	if (debugFlag)
	{
		runCommand << "CSX_file = os.path.join(Sim_Path, 'input.xml')\n";
		runCommand << "CSX.Write2XML(CSX_file)\n";
	}

	runCommand << "FDTD.Run(Sim_Path, cleanup=False)\n";
}

void FDTDSolver::convertAndStoreResults()
{
	timeStepWidth = 5.18459e-13;

	convertAndStoreFrequencyDomainDump("E-Field Complex", "E-Field", "V/m");
	convertAndStoreTimeDomainDump("E-Field Time", "E-Field", "V/m");
}

void FDTDSolver::convertAndStoreTimeDomainDump(const std::string& resultName, const std::string& fieldType, const std::string& unit)
{
	// Search for files of the form resultName_00010.vtr

	auto escape_regex = [](const std::string& s) {
		return std::regex_replace(s, std::regex(R"([.^$|()\\[*+?{\]])"), R"(\$&)");
	};

	std::list<std::string> resultFileList;

	for (const auto& entry : std::filesystem::directory_iterator(tempDirPath)) {
		if (entry.is_regular_file()) {
			std::string name = toLower(entry.path().filename().string());

			std::string pattern_str = "^" + escape_regex(toLower(resultName)) + R"(_\d+\.vtr$)";
			std::regex pattern(pattern_str);

			if (std::regex_match(name, pattern))
			{
				std::string fullPath = entry.path().string();

				resultFileList.push_back(fullPath);
			}
		}
	}

	if (!resultFileList.empty())
	{
		convertAndStoreSingleTimeDomainDump(resultFileList, resultName, fieldType, unit);
	}
}

void FDTDSolver::convertAndStoreFrequencyDomainDump(const std::string &resultName, const std::string& fieldType, const std::string& unit)
{
	// Search for files of the form resultName_f=20000.000_abs.vtr

	auto escape_regex = [](const std::string& s) {
		return std::regex_replace(s, std::regex(R"([.^$|()\\[*+?{\]])"), R"(\$&)");
	};

	for (const auto& entry : std::filesystem::directory_iterator(tempDirPath)) {
		if (entry.is_regular_file()) {
			std::string name = toLower(entry.path().filename().string());

			std::string pattern_str = "^" + escape_regex(toLower(resultName)) + R"(_[^_]+_abs\.vtr$)";
			std::regex pattern(pattern_str);

			if (std::regex_match(name, pattern))
			{
				std::string fullPath = entry.path().string();

				// remove "_abs.vtr" (8 characters)
				std::string base = fullPath.substr(0, fullPath.size() - 8);

				std::string absFile = base + "_abs.vtr";
				std::string argFile = base + "_arg.vtr";

				if (std::filesystem::exists(absFile) && std::filesystem::exists(argFile))
				{
					convertAndStoreSingleFrequencyDomainDump(absFile, argFile, fieldType, unit);
				}
			}
		}
	}
}

void FDTDSolver::convertAndStoreSingleFrequencyDomainDump(const std::string& absFileName, const std::string& argFileName, const std::string &fieldType, const std::string &unit)
{
	// Extract result folder name from result file name
	std::string resultName = parseComplexResultFileName(absFileName);
	if (resultName.empty())
	{
		assert(0);
		return;
	}

	// Load the abs file data and store it as a binary data object
	std::vector<char> absFileData = readFile(absFileName);

	EntityBinaryData* vtkAbsData = new EntityBinaryData(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkAbsData->setData(absFileData.data(), absFileData.size());
	vtkAbsData->storeToDataBase();

	ot::UID vtkAbsDataEntityID = vtkAbsData->getEntityID();
	ot::UID vtkAbsDataEntityVersion = vtkAbsData->getEntityStorageVersion();
	absFileData.clear();

	// Load the arg file data and store it as a binary data object
	std::vector<char> argFileData = readFile(argFileName);

	EntityBinaryData* vtkArgData = new EntityBinaryData(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkArgData->setData(argFileData.data(), argFileData.size());
	vtkArgData->storeToDataBase();

	ot::UID vtkArgDataEntityID = vtkArgData->getEntityID();
	ot::UID vtkArgDataEntityVersion = vtkArgData->getEntityStorageVersion();

	argFileData.clear();

	EntityResultVtkComplex* vtkResult = new EntityResultVtkComplex(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkResult->setComplexData(fieldType, EntityResultVtkComplex::VECTOR_COMPLEX_MAG_PHASE, vtkAbsData, vtkArgData);
	vtkResult->setScaleFactor(1.0 / entityUnits->getScaleToSIDimension());
	vtkResult->storeToDataBase();

	EntityVisVtkVectorVolumeComplex* visualizationEntity = new EntityVisVtkVectorVolumeComplex(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	visualizationEntity->setName(solverEntity->getName() + "/Results/" + resultName);
	visualizationEntity->setResultType(EntityResultBase::CARTESIAN_NODE);
	visualizationEntity->setTreeItemEditable(true);
	visualizationEntity->setInitiallyHidden(true);
	visualizationEntity->registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		OT_INFO_SERVICE_TYPE_VisualizationService
	);

	visualizationEntity->createProperties();

	visualizationEntity->setSource(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion());
	visualizationEntity->setUnit(unit);

	visualizationEntity->storeToDataBase();

	application->getModelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);
	application->getModelComponent()->addNewDataEntity(vtkAbsDataEntityID, vtkAbsDataEntityVersion, vtkResult->getEntityID());
	application->getModelComponent()->addNewDataEntity(vtkArgDataEntityID, vtkArgDataEntityVersion, vtkResult->getEntityID());
	application->getModelComponent()->addNewDataEntity(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion(), visualizationEntity->getEntityID());

	delete visualizationEntity;
	visualizationEntity = nullptr;

	delete vtkResult;
	vtkResult = nullptr;
}

void FDTDSolver::convertAndStoreSingleTimeDomainDump(std::list<std::string>& resultFileList, const std::string& resultName, const std::string& fieldType, const std::string& unit)
{
	// First, write binary data items for all result data files
	std::list<std::pair<ot::UID, ot::UID>> dataEntityList;
	std::list<double>  dataEntityTimeList;

	double rangeMax = 0.0;

	for (auto resultFile : resultFileList)
	{
		// Determine the time for this result
		double time = 0.0;

		size_t underscore_pos = resultFile.rfind('_');
		if (underscore_pos != std::string::npos)
		{
			size_t dot_pos = resultFile.rfind(".vtr");

			if (dot_pos != std::string::npos && dot_pos > underscore_pos)
			{
				std::string number_str = resultFile.substr(underscore_pos + 1, dot_pos - underscore_pos - 1);
				time = timeStepWidth * std::stoi(number_str) / entityUnits->getScaleToSITime();
			}
		}

		// Read the data
		std::vector<char> resultFileData = readFile(resultFile);

		// Determine the max value
		double dataRangeMax = fabs(extractRangeMax(resultFileData));
		rangeMax = std::max(rangeMax, dataRangeMax);

		EntityBinaryData* vtkData = new EntityBinaryData(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
		vtkData->setData(resultFileData.data(), resultFileData.size());
		vtkData->storeToDataBase();

		dataEntityList.push_back(std::pair<ot::UID, ot::UID>(vtkData->getEntityID(), vtkData->getEntityStorageVersion()));
		dataEntityTimeList.push_back(time);

		// Clear the storage (we need to load file by file, since the storage may be too large otherwise
		delete vtkData;
		vtkData = nullptr;
	}

	// Now we create a vtk result entity as a container for the binary data and the topology entity
	EntityResultVtkTime* vtkResult = new EntityResultVtkTime(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkResult->setTimeData(fieldType, dataEntityList, dataEntityTimeList);
	vtkResult->setScaleFactor(1.0 / entityUnits->getScaleToSIDimension());
	vtkResult->storeToDataBase();

	EntityVisVtkVectorVolumeTime* visualizationEntity = new EntityVisVtkVectorVolumeTime(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	visualizationEntity->setName(solverEntity->getName() + "/Results/" + resultName);
	visualizationEntity->setResultType(EntityResultBase::CARTESIAN_NODE);
	visualizationEntity->setTreeItemEditable(true);
	visualizationEntity->setInitiallyHidden(true);
	visualizationEntity->registerCallbacks(
		ot::EntityCallbackBase::Callback::Properties |
		ot::EntityCallbackBase::Callback::Selection |
		ot::EntityCallbackBase::Callback::DataNotify,
		OT_INFO_SERVICE_TYPE_VisualizationService
	);

	visualizationEntity->createProperties();

	visualizationEntity->setSource(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion());
	visualizationEntity->setUnit(unit);
	visualizationEntity->setTimeList(dataEntityTimeList);
	visualizationEntity->setGlobalRange(-rangeMax, rangeMax);

	visualizationEntity->storeToDataBase();

	application->getModelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);

	for (auto dataEntity : dataEntityList)
	{
		application->getModelComponent()->addNewDataEntity(dataEntity.first, dataEntity.second, vtkResult->getEntityID());
	}

	application->getModelComponent()->addNewDataEntity(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion(), visualizationEntity->getEntityID());

	delete visualizationEntity;
	visualizationEntity = nullptr;

	delete vtkResult;
	vtkResult = nullptr;
}

double FDTDSolver::extractRangeMax(const std::vector<char>& data) 
{
	if (data.empty()) {
		return 0.0;
	}

	const char* begin = data.data();
	const char* end = begin + data.size();

	const char key[] = "RangeMax=\"";
	const std::size_t keyLen = sizeof(key) - 1;

	const char* pos = begin;

	while (pos + keyLen < end) {
		const void* found = std::memchr(pos, 'R', static_cast<std::size_t>(end - pos));
		if (!found) {
			return 0.0;
		}

		pos = static_cast<const char*>(found);

		if (pos + keyLen >= end) {
			return 0.0;
		}

		if (std::memcmp(pos, key, keyLen) == 0) {
			const char* valueBegin = pos + keyLen;
			const char* valueEnd = valueBegin;

			while (valueEnd < end && *valueEnd != '"') {
				++valueEnd;
			}

			if (valueEnd == end) {
				return 0.0;
			}

			double value = 0.0;
			auto result = std::from_chars(valueBegin, valueEnd, value);

			if (result.ec != std::errc() || result.ptr != valueEnd) {
				return 0.0;
			}

			return value;
		}

		++pos;
	}

	return 0.0;
}

std::string FDTDSolver::escapeBackslashes(const std::string& input) 
{
	std::string result;
	result.reserve(input.size() * 2);

	for (char c : input) {
		if (c == '\\') {
			result += "\\\\";
		}
		else {
			result += c;
		}
	}

	return result;
}

std::string FDTDSolver::toLower(std::string s) 
{
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
	return s;
}

std::vector<char> FDTDSolver::readFile(const std::string& filename) 
{
	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	if (!file) return {};

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<char> buffer(size+1);

	if (!file.read(buffer.data(), size)) {
		return {};
	}

	buffer[size] = '\0';

	return buffer;
}

std::string FDTDSolver::parseComplexResultFileName(const std::string& input)
{
	// The input name has the format: <name>_f=<frequency>_abs.vtr

	if (entityUnits == nullptr)
	{
		assert(0);
		return std::string();
	}

	double frequency = 0.0;

	const std::string marker1 = "_f=";
	const std::string marker2 = "_abs.vtr";

	std::string lowerCaseInput = toLower(input);
	size_t pos1 = lowerCaseInput.find(marker1);
	size_t pos2 = lowerCaseInput.rfind(marker2);

	if (pos1 == std::string::npos || pos2 == std::string::npos || pos2 <= pos1)
		return std::string();

	// Extract name
	std::string name = input.substr(0, pos1);

	// Extract frequency
	std::string freqStr = input.substr(
		pos1 + marker1.size(),
		pos2 - (pos1 + marker1.size())
	);

	if (!toDouble(freqStr, frequency))
	{
		return std::string();
	}

	// Remove path
	size_t pos = name.rfind('\\');

	if (pos != std::string::npos) 
	{
		name = name.substr(pos + 1);
	}

	// Build new name with scaled frequency
	name = name + "(f=" + doubleToString(frequency / entityUnits->getScaleToSIFrequency()) + ")";

	return name;
}

bool FDTDSolver::toDouble(const std::string& s, double& value) 
{
	auto result = std::from_chars(s.data(), s.data() + s.size(), value);

	return result.ec == std::errc() && result.ptr == s.data() + s.size();
}


std::string FDTDSolver::doubleToString(double value) {
	char buffer[64];

	auto result = std::to_chars(
		buffer, buffer + sizeof(buffer),
		value,
		std::chars_format::general
	);

	if (result.ec != std::errc()) {
		return {};
	}

	return std::string(buffer, result.ptr);
}
