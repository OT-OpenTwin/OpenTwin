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
#include "OTModelEntities/EntityResultCartesianMeshVtk.h"
#include "OTModelEntities/EntityVisCartesianVectorVolume.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityAPI.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include <vector>
#include <charconv>

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
		"Et = CSX.AddDump('E-Field', dump_type=10, dump_mode=3, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop],opt_resolution=[0,90,2])\n"
		"start = [0, 0, 0];\n"
		"stop  = [a, b, length];\n"
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
	convertAndStoreFrequencyDomainDumps();
}

void FDTDSolver::convertAndStoreFrequencyDomainDumps()
{
	// First, we search for all complex dump results
	std::list<std::string> results;

	for (const auto& entry : std::filesystem::directory_iterator(tempDirPath)) {
		if (entry.is_regular_file()) {
			std::string name = toLower(entry.path().filename().string());

			if (name.size() >= 8 && name.substr(name.size() - 8) == "_abs.vtr") 
			{
				std::string fullPath = entry.path().string();

				// remove "_abs.vtr" (8 characters)
				std::string base = fullPath.substr(0, fullPath.size() - 8);

				results.push_back(base);
			}
		}
	}

	// Now we process each result one by one (make sure that both _abs and _arg exist
	for (auto& result : results)
	{
		std::string absFile = result + "_abs.vtr";
		std::string argFile = result + "_arg.vtr";

		if (std::filesystem::exists(absFile) && std::filesystem::exists(argFile))
		{
			convertAndStoreSingleFrequencyDomainDump(absFile, argFile);
		}
	}
}

void FDTDSolver::convertAndStoreSingleFrequencyDomainDump(const std::string& absFileName, const std::string& argFileName)
{
	// Extract result folder name from result file name
	std::string quantityName;
	std::string resultName = parseComplexResultFileName(absFileName, quantityName);
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

	EntityResultCartesianMeshVtk* vtkResult = new EntityResultCartesianMeshVtk(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkResult->setComplexData(quantityName, EntityResultCartesianMeshVtk::VECTOR_COMPLEX_MAG_PHASE, vtkAbsData, vtkArgData);
	vtkResult->setScaleFactor(1.0 / entityUnits->getScaleToSIDimension());
	vtkResult->storeToDataBase();

	EntityVisCartesianVectorVolume* visualizationEntity = new EntityVisCartesianVectorVolume(application->getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
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
	visualizationEntity->setUnit("V/m");

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

std::string FDTDSolver::parseComplexResultFileName(const std::string& input, std::string& quantityName)
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

	quantityName = name;

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
