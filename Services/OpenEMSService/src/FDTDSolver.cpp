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
#include "ResultManager.h"

#include "OTServiceFoundation/ModelComponent.h"

#include "OTModelEntities/EntitySolver.h"
#include "OTModelEntities/EntityMeshCartesian.h"
#include "OTModelEntities/EntityMeshCartesianData.h"
#include "OTModelEntities/EntityUnits.h"
#include "OTModelEntities/EntityMaterial.h"
#include "OTModelEntities/EntityBinaryData.h"
#include "OTModelEntities/EntityInformation.h"
#include "OTModelEntities/EntityResultVtkComplex.h"
#include "OTModelEntities/EntityResultVtkTime.h"
#include "OTModelEntities/EntityVisVtkVectorVolumeComplex.h"
#include "OTModelEntities/EntityVisVtkVectorVolumeTime.h"
#include "OTModelEntities/DataBase.h"

#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityAPI.h"

#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/EntityWaveguidePort.h"

#include <fstream>
#include <filesystem>
#include <algorithm>
#include <list>
#include <vector>
#include <charconv>
#include <regex>
#include <cctype>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>

FDTDSolver::FDTDSolver(Application* _application, EntityBase* _solverEntity, EntityMeshCartesian* _meshEntity, const std::string& _openEMSPath, const std::string& _tempDirPath)
	: application(_application), solverEntity(_solverEntity), meshEntity(_meshEntity), openEMSPath(_openEMSPath), tempDirPath(_tempDirPath), entityUnits(nullptr), timeStepWidth(0.0)
{
	assert(application != nullptr);
	assert(solverEntity != nullptr);
	assert(meshEntity != nullptr);
	assert(!openEMSPath.empty());
	assert(!_tempDirPath.empty());
}

FDTDSolver::~FDTDSolver()
{
	for (auto port : waveguidePortList)
	{
		delete port;
	}
}

std::string FDTDSolver::generateRunCommand()
{
	std::stringstream runCommand;
	runCommand << std::defaultfloat << std::setprecision(12);

	readPorts();
	readExcitation();

	checkCartesianMesh(runCommand);

	addPreparationData(runCommand);
	addUnits(runCommand);
	addSolverSetup(runCommand);
	addMesh(runCommand);
	addGeometry(runCommand);
	// addPorts
	// addMonitors

	runCommand << "## Apply the waveguide port\n"
				  "# waveguide dimensions\n"
				  "# WR42\n"
				  "a = 10.7;   #waveguide width\n"
				  "b = 4.3;    #waveguide height\n"
				  "length = 50.0;\n"
				  "#waveguide TE-mode definition\n"
				  "TE_mode = 'TE10';\n"
				  "ports = []\n"
				  "start=[0, 0, " << zLines[10] << "];\n"
				  "stop =[a, b, " << zLines[15] << "];\n"
				  "ports.append(FDTD.AddRectWaveGuidePort( 0, start, stop, 'z', a*gunit, b*gunit, TE_mode, 1))\n"
				  "\n"
				  "start=[0, 0, " << zLines[zLines.size() - 12] << "];\n"
				  "stop =[a, b, " << zLines[zLines.size() - 17] << "];\n"
				  "ports.append(FDTD.AddRectWaveGuidePort( 1, start, stop, 'z', a*gunit, b*gunit, TE_mode))\n";

	std::string text =
		"### Define dump box...\n"
		"#Et = CSX.AddDump('E-Field', dump_type=10, dump_mode=3, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop], sub_sampling=[2,2,2])\n"
		"Ef = CSX.AddDump('E-Field Complex', dump_type=10, dump_mode=3, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop])\n"
		"Et = CSX.AddDump('E-Field Time', dump_type=0, dump_mode=3, file_type=0, sub_sampling=[2,2,2])\n"
		"start = [0, 0, 0];\n"
		"stop  = [a, b, length];\n"
		"Ef.AddBox(start, stop);\n"
		"Et.AddBox(start, stop);\n"
		"\n";

	runCommand << text;

	addSolverRun(runCommand);
	addPostprocessing(runCommand);

	return runCommand.str();
}

void FDTDSolver::readPorts()
{
	std::list<std::string> portEntityNames = ot::ModelServiceAPI::getListOfFolderItems(solverEntity->getName() + "/Ports", false);
	if (portEntityNames.empty()) throw(std::string("No ports defined"));

	std::list<ot::EntityInformation> portEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(portEntityNames, portEntitiesInfo);

	// Read all geometry entities
	DataBase::instance().prefetchDocumentsFromStorage(portEntitiesInfo);

	for (auto port : portEntitiesInfo)
	{
		EntityBase* portEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(port.getEntityID(), port.getEntityVersion());

		EntityWaveguidePort* waveguidePort = dynamic_cast<EntityWaveguidePort*>(portEntity);
		if (waveguidePort != nullptr)
		{
			waveguidePortList.push_back(waveguidePort);
			portEntity = nullptr;

			int portNumber = 0;

			if (parsePortNumber(waveguidePort->getName(), portNumber))
			{
				if (portList.count(portNumber) != 0)
				{
					throw std::string("Port number " + std::to_string(portNumber) + " has been multiply defined");
				}

				portList.emplace(portNumber);
			}
			else
			{
				throw std::string("Invalid port name " + waveguidePort->getName());
			}
		}

		if (portEntity != nullptr)
		{
			std::string error = "The port named " + portEntity->getName() + " is not a supported type";
			delete portEntity;

			throw error;
		}
	}
}

void FDTDSolver::readExcitation()
{
	// Parse the excitation string
	excitationList.clear();

	EntityPropertiesString* excitationTypeProperty = dynamic_cast<EntityPropertiesString*>(solverEntity->getProperties().getProperty("Ports"));
	if (excitationTypeProperty == nullptr)
	{
		throw std::string("No port excitation specified");
	}

	std::string excitationString = excitationTypeProperty->getValue();

	excitationList = FDTDSolver::parseExcitations(excitationString);

	// Check, whether all ports in the excitation string exist
	for (auto& excitation : excitationList)
	{
		for (auto& port : excitation)
		{
			if (portList.count(port.first) == 0)
			{
				throw std::string("The excitation port " + std::to_string(port.first) + " does not exist");
			}
		}
	}
}

void FDTDSolver::checkCartesianMesh(std::stringstream& runCommand)
{
	if (!meshEntity->getMeshValid())
	{
		// The mesh is invalid
		throw std::string("The specified cartesian mesh is invalid");
	}

	EntityPropertiesSelection* problemTypeProperty = dynamic_cast<EntityPropertiesSelection*>(meshEntity->getProperties().getProperty("Problem type"));
	if (problemTypeProperty == nullptr)
	{
		runCommand << "print(\"WARNING: The mesh problem type has not been set. The mesh should be of type Electromagnetics (HF).\\n\")\n";
	}
	else
	{
		if (problemTypeProperty->getValue() != "Electromagnetics (HF)")
		{
			runCommand << "print(\"WARNING: The mesh problem type should be set to Electromagnetics (HF) to ensure a proper discretization.\\n\")\n";
		}
		else
		{
			EntityPropertiesDouble* maximumFrequencyProperty = dynamic_cast<EntityPropertiesDouble*>(meshEntity->getProperties().getProperty("Maximum frequency"));
			if (maximumFrequencyProperty == nullptr)
			{
				runCommand << "print(\"WARNING: The maximum frequency has not been defined for the mesh.\\n\")\n";
			}
			else
			{
				EntityPropertiesDouble* fMaxProperty = dynamic_cast<EntityPropertiesDouble*>(solverEntity->getProperties().getProperty("Fmax"));
				double fMax = (fMaxProperty != nullptr) ? fMaxProperty->getValue() : 0.0;

				double tolerance = 1e-5 * std::max(maximumFrequencyProperty->getValue(), fMax);
				if (tolerance == 0.0) tolerance = 1e-5;

				if (fMax - maximumFrequencyProperty->getValue() > tolerance)
				{
					runCommand << "print(\"WARNING: The maximum frequency for the mesh is lower than the maximum solver frequency. The mesh might be too coarse.\\n\")\n";
				}
			}
		}
	}

}

void FDTDSolver::readMeshLineInformation()
{
	// Here we load the mesh data object and get the mesh line distribution from there
	ot::EntityInformation meshDataInfo;
	ot::ModelServiceAPI::getEntityInformation(meshEntity->getMeshDataStorageId(), meshDataInfo);

	EntityMeshCartesianData *meshDataEntity = dynamic_cast<EntityMeshCartesianData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(meshDataInfo.getEntityID(), meshDataInfo.getEntityVersion()));
	if (meshDataEntity == nullptr)
	{
		throw(std::string("Unable to read mesh data"));
	}

	xLines = meshDataEntity->getMeshLinesX();
	yLines = meshDataEntity->getMeshLinesY();
	zLines = meshDataEntity->getMeshLinesZ();

	delete meshDataEntity;
	meshDataEntity = nullptr;
}

void FDTDSolver::writeLinesArray(const std::string& direction, const std::vector<double>& linesArray, std::stringstream& runCommand)
{
	runCommand << "mesh.SetLines('" << direction << "', [";

	for (size_t index = 0; index < linesArray.size() - 1; index++)
	{
		runCommand << linesArray[index] << ", ";
	}

	runCommand << linesArray.back() << "])\n";
}

void FDTDSolver::addMesh(std::stringstream& runCommand)
{
	readMeshLineInformation();

	runCommand << "#=================================================================================\n";
	runCommand << "# Define mesh\n";
	runCommand << "#=================================================================================\n";

	runCommand << "mesh = CSX.GetGrid()\n";
	runCommand << "mesh.SetDeltaUnit(gunit)\n";
	writeLinesArray("x", xLines, runCommand);
	writeLinesArray("y", yLines, runCommand);
	writeLinesArray("z", zLines, runCommand);
}

void FDTDSolver::addPreparationData(std::stringstream &runCommand)
{
	runCommand << "#=================================================================================\n";
	runCommand << "# Import python packages and set path variable\n";
	runCommand << "#=================================================================================\n";

	runCommand << "import os, tempfile\n";
	runCommand << "import numpy as np\n";
	runCommand << "os.environ[\"OPENEMS_INSTALL_PATH\"] = \"" << escapeBackslashes(openEMSPath) << "\"\n";
	runCommand << "from CSXCAD  import ContinuousStructure\n";
	runCommand << "from openEMS import openEMS\n";
	runCommand << "from openEMS.physical_constants import *\n";
	runCommand << "from pathlib import Path\n";
	runCommand << "Sim_Path = str(Path(\"" << escapeBackslashes(tempDirPath) << "\").resolve())\n";
}

void FDTDSolver::addUnits(std::stringstream& runCommand)
{
	ot::EntityInformation unitsInfo;
	ot::ModelServiceAPI::getEntityInformation("Units", unitsInfo);

	entityUnits = dynamic_cast<EntityUnits*>(ot::EntityAPI::readEntityFromEntityIDandVersion(unitsInfo.getEntityID(), unitsInfo.getEntityVersion()));
	if (entityUnits == nullptr)
	{
		throw(std::string("Unable to read units"));
	}
	
	runCommand << "#=================================================================================\n";
	runCommand << "# Set units\n";
	runCommand << "#=================================================================================\n";

	runCommand << "gunit = " << entityUnits->getScaleToSIDimension() << "\n";
	runCommand << "funit = " << entityUnits->getScaleToSIFrequency() << "\n";
	runCommand << "tunit = " << entityUnits->getScaleToSITime() << "\n";
}

void FDTDSolver::addGeometry(std::stringstream& runCommand)
{
	// Loop through all entities below the mesh entities geometry folder. If this folder does not exist, raise an exception
	ot::UIDList geometryEntitiesID = ot::ModelServiceAPI::getIDsOfFolderItemsOfType(meshEntity->getName() + "/Geometry", "EntityGeometry", true);
	if (geometryEntitiesID.empty()) throw(std::string("Unable to read geometry from mesh"));

	std::list<ot::EntityInformation> geometryEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(geometryEntitiesID, geometryEntitiesInfo);

	// Read all geometry entities
	DataBase::instance().prefetchDocumentsFromStorage(geometryEntitiesInfo);

	std::list<EntityGeometry*> geometryEntities;
	for (auto geomEntity : geometryEntitiesInfo)
	{
		EntityGeometry *geometry = dynamic_cast<EntityGeometry*>(ot::EntityAPI::readEntityFromEntityIDandVersion(geomEntity.getEntityID(), geomEntity.getEntityVersion()));
		if (geometry == nullptr)
		{
			throw(std::string("Unable to read geometry from mesh"));
		}
		if (geometry->getName() != meshEntity->getName() + "/Geometry/Background")  // Ignore the background material, since this will be handled separately later
		{
			geometryEntities.push_back(geometry);
		}
	}

	// Sort the geometry entities by material
	ot::UIDList facetIdList;
	std::list<std::string> materialNames;
	std::map<std::string, std::list<EntityGeometry*>> materialToGeometryMap;

	for (auto geom : geometryEntities)
	{
		facetIdList.push_back(geom->getFacetsPrefetchID());

		EntityPropertiesEntityList* material = dynamic_cast<EntityPropertiesEntityList*>(geom->getProperties().getProperty("Material"));
		if (material == nullptr)
		{
			throw(std::string("No material defined for shape: " + geom->getName()));
		}

		std::string materialName = material->getValueName();
		materialNames.push_back(materialName);

		materialToGeometryMap[materialName].push_back(geom);
	}

	std::list<ot::EntityInformation> facetEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(facetIdList, facetEntitiesInfo);

	// Build a map for the facet entities id -> version information
	std::map<ot::UID, ot::UID> facetIDtoVersionMap;
	for (auto facetInfo : facetEntitiesInfo)
	{
		facetIDtoVersionMap[facetInfo.getEntityID()] = facetInfo.getEntityVersion();
	}

	std::list<ot::EntityInformation> materialEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(materialNames, materialEntitiesInfo);

	// Read all facet entities
	DataBase::instance().prefetchDocumentsFromStorage(facetEntitiesInfo);

	// Read all relevant material entities
	DataBase::instance().prefetchDocumentsFromStorage(materialEntitiesInfo);

	// Create the materials one-by-one and add the corresponding objects (write the stl files and add them to the solver script)
	std::list<EntityMaterial*> materialEntities;
	std::list<EntityFacetData*> facetEntities;
	int materialCount = 1;

	// Determine the shape priorities (we store them as doubles combined for material and shape priorities. OpenEMS needs them as an integer)
	std::map<std::string, double> shapeNameToPriorityMap;
	for (auto material : materialEntitiesInfo)
	{
		EntityMaterial* materialEntity = dynamic_cast<EntityMaterial*>(ot::EntityAPI::readEntityFromEntityIDandVersion(material.getEntityID(), material.getEntityVersion()));
		if (materialEntity == nullptr)
		{
			throw(std::string("Unable to load material: ") + material.getEntityName());
		}

		materialEntities.push_back(materialEntity);

		EntityPropertiesDouble* meshPriority = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Mesh priority"));
		double materialPriority = meshPriority != nullptr ? meshPriority->getValue() : 0.0;

		for (auto geomEntity : materialToGeometryMap[materialEntity->getName()])
		{
			EntityPropertiesDouble* shapePriorityProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("Mesh priority"));
			double shapePriority = shapePriorityProperty != nullptr ? shapePriorityProperty->getValue() : 0.0;

			shapeNameToPriorityMap[geomEntity->getName()] = shapePriority + materialPriority;
		}
	}

	std::map<std::string, int> shapeNameToPriorityIntMap = createIntegerPriorities(shapeNameToPriorityMap);

	runCommand << "#=================================================================================\n";
	runCommand << "# Define material and shapes\n";
	runCommand << "#=================================================================================\n";

	// Loop for all materials and store all shapes belonging to this material
	for (auto materialEntity : materialEntities)
	{
		EntityPropertiesSelection* materialType = dynamic_cast<EntityPropertiesSelection*>(materialEntity->getProperties().getProperty("Material type"));

		std::string openEMSMaterialName = "material" + std::to_string(materialCount);

		double minPriority = DBL_MAX;

		if (materialType->getValue() == "PEC")
		{
			runCommand << openEMSMaterialName << " = CSX.AddMetal(\"" << openEMSMaterialName << "\")\n";
		}
		else if (materialType->getValue() == "Volumetric")
		{
			EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Permittivity (relative)"));
			EntityPropertiesDouble* permeability = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Permeability (relative)"));
			EntityPropertiesDouble* conductivity = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Conductivity"));

			double epsilon = permittivity != nullptr ? permittivity->getValue() : 1.0;
			double mue     = permeability != nullptr ? permeability->getValue() : 1.0;
			double kappa   = conductivity != nullptr ? conductivity->getValue() : 0.0;

			runCommand << openEMSMaterialName << " = CSX.AddMaterial(\"" << openEMSMaterialName << "\", epsilon=" << epsilon << ", mue=" << mue << ", kappa=" << kappa << ", sigma=0.0)\n";
		}
		else
		{
			assert(0); // Unknown material type
		}

		// Now add all shapes for this material
		int shapeCount = 1;
		for (auto geomEntity : materialToGeometryMap[materialEntity->getName()])
		{
			std::string stlFileName = tempDirPath + "\\shape" + std::to_string(materialCount) + "_" + std::to_string(shapeCount) + ".stl";

			// Load the facets
			EntityFacetData* facetData = dynamic_cast<EntityFacetData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(geomEntity->getFacetsStorageObjectID(), facetIDtoVersionMap[geomEntity->getFacetsStorageObjectID()]));
			if (facetData == nullptr)
			{
				throw(std::string("Unable to load facets for shape: ") + geomEntity->getName());
			}

			facetEntities.push_back(facetData);

			if (!storeSTLGeometry(facetData, stlFileName))
			{
				throw(std::string("Unable to write facets as STL file: ") + stlFileName);
			}

			runCommand << "stl" << materialCount << "_" << shapeCount << " = " << openEMSMaterialName << ".AddPolyhedronReader(\"" << escapeBackslashes(stlFileName) << "\", priority = " << shapeNameToPriorityIntMap[geomEntity->getName()] << ")\n";
			runCommand << "stl" << materialCount << "_" << shapeCount << ".ReadFile()\n";

			shapeCount++;
		}

		materialCount++;
	}

	// Clean up facet entities
	for (auto facets : facetEntities)
	{
		delete facets;
	}

	// Clean up material entities
	for (auto material : materialEntities)
	{
		delete material;
	}

	// Clean up geometry entities
	for (auto geom : geometryEntities)
	{
		delete geom;
	}

	// Handle the background material
	addBackgroundMaterial(runCommand);
}

void FDTDSolver::addBackgroundMaterial(std::stringstream& runCommand)
{
	EntityPropertiesSelection* backgroundMode = dynamic_cast<EntityPropertiesSelection*>(meshEntity->getProperties().getProperty("Background mode"));
	EntityPropertiesEntityList* backgroundMaterial = dynamic_cast<EntityPropertiesEntityList*>(meshEntity->getProperties().getProperty("Background material"));

	bool isPECBackground = false;
	double epsilon       = 1.0;
	double mue           = 1.0;
	double kappa         = 0.0;

	if (backgroundMode != nullptr)
	{
		if (backgroundMode->getValue() == "Field free")
		{
			isPECBackground = true;
		}
		else
		{
			if (backgroundMaterial != nullptr)
			{
				ot::EntityInformation backgroundInfo;
				ot::ModelServiceAPI::getEntityInformation(backgroundMaterial->getValueName(), backgroundInfo);

				EntityMaterial* materialEntity = dynamic_cast<EntityMaterial*>(ot::EntityAPI::readEntityFromEntityIDandVersion(backgroundInfo.getEntityID(), backgroundInfo.getEntityVersion()));
				if (materialEntity == nullptr)
				{
					throw(std::string("Unable to load background material: ") + backgroundMaterial->getValueName());
				}

				EntityPropertiesSelection* materialType = dynamic_cast<EntityPropertiesSelection*>(materialEntity->getProperties().getProperty("Material type"));

				if (materialType->getValue() == "PEC")
				{
					isPECBackground = true;
				}
				else if (materialType->getValue() == "Volumetric")
				{
					isPECBackground = false;

					EntityPropertiesDouble* permittivity = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Permittivity (relative)"));
					EntityPropertiesDouble* permeability = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Permeability (relative)"));
					EntityPropertiesDouble* conductivity = dynamic_cast<EntityPropertiesDouble*>(materialEntity->getProperties().getProperty("Conductivity"));

					epsilon = permittivity != nullptr ? permittivity->getValue() : 1.0;
					mue     = permeability != nullptr ? permeability->getValue() : 1.0;
					kappa   = conductivity != nullptr ? conductivity->getValue() : 0.0;
				}
			}
			else
			{
				isPECBackground = false;
			}
		}
	}

	if (isPECBackground)
	{
		runCommand << "background = CSX.AddMetal(\"background\")\n";
	}
	else
	{
		runCommand << "background = CSX.AddMaterial(\"background\", epsilon=" << epsilon << ", mue=" << mue << ", kappa=" << kappa << ", sigma=0.0)\n";
	}

	runCommand << "background.AddBox(start = [" << xLines.front() << ", " << yLines.front() << ", " << zLines.front() << "], "
		                            "stop = [" << xLines.back() << ", " << yLines.back() << ", " << zLines.back() << "], priority = 0)\n";
}

std::map<std::string, int> FDTDSolver::createIntegerPriorities(const std::map<std::string, double>& shapeNameToPriorityMap, double tolerance)
{
	std::vector<std::pair<std::string, double>> sortedPriorities(
		shapeNameToPriorityMap.begin(),
		shapeNameToPriorityMap.end()
	);

	// Sort by double priority in ascending order
	std::sort(
		sortedPriorities.begin(),
		sortedPriorities.end(),
		[](const auto& a, const auto& b)
		{
			return a.second < b.second;
		}
	);

	std::map<std::string, int> result;

	if (sortedPriorities.empty()) {
		return result;
	}

	int integerPriority = 1;
	double groupPriority = sortedPriorities.front().second;

	for (const auto& [name, priority] : sortedPriorities) {
		// Start a new group if the priority differs by more than the tolerance
		if (std::abs(priority - groupPriority) > tolerance) {
			++integerPriority;
			groupPriority = priority;
		}

		result[name] = integerPriority;
	}

	return result;
}

bool FDTDSolver::storeSTLGeometry(EntityFacetData* facetData, const std::string &stlFileName)
{
	auto& nodes = facetData->getNodeVector();
	auto& triangles = facetData->getTriangleList();

	std::ofstream file(stlFileName);
	if (!file) {
		return false;
	}

	file << std::scientific << std::setprecision(9);
	file << "solid shape\n";

	for (auto& triangle : triangles) {
		const std::size_t i0 =
			static_cast<std::size_t>(triangle.getNode(0));
		const std::size_t i1 =
			static_cast<std::size_t>(triangle.getNode(1));
		const std::size_t i2 =
			static_cast<std::size_t>(triangle.getNode(2));

		if (i0 >= nodes.size() ||
			i1 >= nodes.size() ||
			i2 >= nodes.size()) {
			return false;
		}

		auto& p0 = nodes[i0];
		auto& p1 = nodes[i1];
		auto& p2 = nodes[i2];

		const double ux = p1.getCoord(0) - p0.getCoord(0);
		const double uy = p1.getCoord(1) - p0.getCoord(1);
		const double uz = p1.getCoord(2) - p0.getCoord(2);

		const double vx = p2.getCoord(0) - p0.getCoord(0);
		const double vy = p2.getCoord(1) - p0.getCoord(1);
		const double vz = p2.getCoord(2) - p0.getCoord(2);

		double nx = uy * vz - uz * vy;
		double ny = uz * vx - ux * vz;
		double nz = ux * vy - uy * vx;

		const double length = std::sqrt(nx * nx + ny * ny + nz * nz);

		if (length > 0.0) {
			nx /= length;
			ny /= length;
			nz /= length;
		}

		file << "  facet normal " << nx << ' ' << ny << ' ' << nz << '\n';
		file << "    outer loop\n";

		file << "      vertex "
			<< p0.getCoord(0) << ' '
			<< p0.getCoord(1) << ' '
			<< p0.getCoord(2) << '\n';

		file << "      vertex "
			<< p1.getCoord(0) << ' '
			<< p1.getCoord(1) << ' '
			<< p1.getCoord(2) << '\n';

		file << "      vertex "
			<< p2.getCoord(0) << ' '
			<< p2.getCoord(1) << ' '
			<< p2.getCoord(2) << '\n';

		file << "    endloop\n";
		file << "  endfacet\n";
	}

	file << "endsolid mesh\n";

	return file.good();
}

void FDTDSolver::addSolverSetup(std::stringstream& runCommand)
{
	EntityPropertiesSelection* excitationTypeProperty  = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Type"));
	EntityPropertiesDouble*    fMinProperty            = dynamic_cast<EntityPropertiesDouble*>(solverEntity->getProperties().getProperty("Fmin"));
	EntityPropertiesDouble*    fMaxProperty            = dynamic_cast<EntityPropertiesDouble*>(solverEntity->getProperties().getProperty("Fmax"));
	EntityPropertiesInteger*   fStepsProperty          = dynamic_cast<EntityPropertiesInteger*>(solverEntity->getProperties().getProperty("Fsamples"));

	EntityPropertiesInteger*   maxTimestepsProperty    = dynamic_cast<EntityPropertiesInteger*>(solverEntity->getProperties().getProperty("Max. timesteps"));
	EntityPropertiesDouble*    energyStopLevelProperty = dynamic_cast<EntityPropertiesDouble*>(solverEntity->getProperties().getProperty("Energy stop level"));

	std::string excitationType = (excitationTypeProperty != nullptr) ? excitationTypeProperty->getValue() : "Gaussian";
	double fMin                = (fMinProperty != nullptr) ? fMinProperty->getValue() : 0.0;
	double fMax                = (fMaxProperty != nullptr) ? fMaxProperty->getValue() : 0.0;
	long long fSteps           = (fStepsProperty != nullptr) ? fStepsProperty->getValue() : 201;
	long long maxTimesteps     = (maxTimestepsProperty != nullptr) ? maxTimestepsProperty->getValue() : 1000000;
	double energyStopLevel     = (energyStopLevelProperty != nullptr) ? energyStopLevelProperty->getValue() : 1e-5;

	double centerFreq    = 0.5 * (fMin + fMax) * entityUnits->getScaleToSIFrequency();
	double halfBandwidth = 0.5 * (fMax - fMin) * entityUnits->getScaleToSIFrequency();

	runCommand << "#=================================================================================\n";
	runCommand << "# Setup solver and define frequency ranges\n";
	runCommand << "#=================================================================================\n";

	runCommand << "CSX = ContinuousStructure()\n";
	runCommand << "FDTD = openEMS(NrTS=" << maxTimesteps << ", EndCriteria=" << energyStopLevel << ")\n";
	runCommand << "FDTD.SetCSX(CSX)\n";

	if (excitationType == "Gaussian")
	{
		if (fMin >= fMax || fMax == 0.0) throw(std::string("Invalid frequency range. Range must not be empty."));
	
		runCommand << "FDTD.SetGaussExcite(" << centerFreq << "," << halfBandwidth << ")\n";
	}
	else
	{
		assert(0); // Unknown excitation type
	}

	EntityPropertiesSelection* xminBoundaryProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Xmin"));
	EntityPropertiesSelection* xmaxBoundaryProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Xmax"));
	EntityPropertiesSelection* yminBoundaryProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Ymin"));
	EntityPropertiesSelection* ymaxBoundaryProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Ymax"));
	EntityPropertiesSelection* zminBoundaryProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Zmin"));
	EntityPropertiesSelection* zmaxBoundaryProperty = dynamic_cast<EntityPropertiesSelection*>(solverEntity->getProperties().getProperty("Zmax"));

	std::string xminBoundary = (xminBoundaryProperty != nullptr) ? xminBoundaryProperty->getValue() : "PEC";
	std::string xmaxBoundary = (xmaxBoundaryProperty != nullptr) ? xmaxBoundaryProperty->getValue() : "PEC";
	std::string yminBoundary = (yminBoundaryProperty != nullptr) ? yminBoundaryProperty->getValue() : "PEC";
	std::string ymaxBoundary = (ymaxBoundaryProperty != nullptr) ? ymaxBoundaryProperty->getValue() : "PEC";
	std::string zminBoundary = (zminBoundaryProperty != nullptr) ? zminBoundaryProperty->getValue() : "PEC";
	std::string zmaxBoundary = (zmaxBoundaryProperty != nullptr) ? zmaxBoundaryProperty->getValue() : "PEC";

	runCommand << "FDTD.SetBoundaryCond(['" << xminBoundary << "', '"
		<< xmaxBoundary << "', '"
		<< yminBoundary << "', '"
		<< ymaxBoundary << "', '"
		<< zminBoundary << "', '"
		<< zmaxBoundary << "'])\n";

	runCommand << "f_start = " << fMin * entityUnits->getScaleToSIFrequency() << "\n";
	runCommand << "f_stop  = " << fMax * entityUnits->getScaleToSIFrequency() << "\n";
	runCommand << "f_samples = " << fSteps << "\n";
}

void FDTDSolver::addSolverRun(std::stringstream& runCommand)
{
	EntityPropertiesBoolean* debug = dynamic_cast<EntityPropertiesBoolean*>(solverEntity->getProperties().getProperty("Debug"));
	assert(debug != nullptr);

	bool debugFlag = false;
	if (debug != nullptr) debugFlag = debug->getValue();

	runCommand << "#=================================================================================\n";
	runCommand << "# Run FDTD solver\n";
	runCommand << "#=================================================================================\n";

	if (debugFlag)
	{
		runCommand << "CSX_file = os.path.join(Sim_Path, 'input.xml')\n";
		runCommand << "CSX.Write2XML(CSX_file)\n";
	}

	runCommand << "FDTD.Run(Sim_Path, cleanup=False)\n";
}

void FDTDSolver::addPostprocessing(std::stringstream& runCommand)
{
	runCommand << "#=================================================================================\n";
	runCommand << "# Define post-processing\n";
	runCommand << "#=================================================================================\n";

	// Define export function
	runCommand << "\n"
	"def save_xy_data(x, y, filename) :\n"
		"		full_path = os.path.join(Sim_Path, filename)\n"
		"		with open(full_path, 'w', encoding = 'utf-8') as f :\n"
		"			for xi, yi in zip(x, y) :\n"
		"				if isinstance(yi, complex) :\n"
		"					f.write(f'{xi}\t{yi.real}\t{yi.imag}\\n')\n"
		"				else :\n"
		"					f.write(f'{xi}\t{yi}\\n')\n\n";

	runCommand <<
		"### Postprocessing & plotting\n"
		"freq = np.linspace(f_start,f_stop,f_samples)\n"
		"for port in ports :\n"
		"	port.CalcPort(Sim_Path, freq)\n"
		"\n"
		"s11 = ports[0].uf_ref / ports[0].uf_inc\n"
		"s21 = ports[1].uf_ref / ports[0].uf_inc\n"
		"ZL  = ports[0].uf_tot / ports[0].if_tot\n"
		"ZL_a = ports[0].ZL # analytic waveguide impedance\n"
		"save_xy_data(freq, s11, 's11')\n"
		"save_xy_data(freq, s21, 's21')\n"
		"save_xy_data(freq, ZL, 'Zl')\n"
		"\n";
}

void FDTDSolver::convertAndStoreResults(const std::string& logFileText)
{
	timeStepWidth = readTimeStepWidthFromLogText(logFileText);
	if (timeStepWidth == 0.0) throw(std::string("Unable to determine time step width."));

	convertAndStoreFrequencyDomainDump("E-Field Complex", "E-Field", "V/m");
	convertAndStoreTimeDomainDump("E-Field Time", "E-Field", "V/m");

	ResultManager result1D(application->getModelComponent(), tempDirPath, solverEntity->getName() + "/Results");

	convert1DTimeSignal("Energy/E-Field", "et", "E-Field Energy", result1D);
	convert1DTimeSignal("Energy/H-Field", "ht", "H-Field Energy", result1D);

	convert1DTimeSignal("Ports/Currents/I1", "port_it_0", "Port 1 Current", result1D, 1);
	convert1DTimeSignal("Ports/Currents/I2", "port_it_1", "Port 2 Current", result1D, 1);
	convert1DTimeSignal("Ports/Voltages/V1", "port_ut_0", "Port 1 Voltage", result1D, 1);
	convert1DTimeSignal("Ports/Voltages/V2", "port_ut_1", "Port 2 Voltage", result1D, 1);

	convert1DFrequencySpectrum("S-Parameter/S1,1", "s11", "S1,1", result1D);
	convert1DFrequencySpectrum("S-Parameter/S2,1", "s21", "S2,1", result1D);

	result1D.storeResults();
}

double FDTDSolver::readTimeStepWidthFromLogText(const std::string& logFileText)
{
	const std::string keyword = "FDTD timestep is: ";

	const std::size_t valueStart = logFileText.find(keyword);
	if (valueStart == std::string::npos) 
	{
		return 0.0;
	}

	const std::size_t numberStart = valueStart + keyword.length();
	const std::size_t semicolonPos = logFileText.find(';', numberStart);

	if (semicolonPos == std::string::npos) 
	{
		return 0.0;
	}

	try 
	{
		const std::string numberText =
			logFileText.substr(numberStart, semicolonPos - numberStart);

		return std::stod(numberText);
	}
	catch (const std::exception&) 
	{
		return 0.0;
	}
}

void FDTDSolver::convert1DTimeSignal(const std::string& resultName, const std::string& fileName, const std::string& quantityName, ResultManager &result1D, int readDataColumnOnly)
{
	result1D.convert1D(resultName, fileName, quantityName, "", "Time", entityUnits->getTimeUnit(), entityUnits->getScaleToSITime(), readDataColumnOnly);
}

void FDTDSolver::convert1DFrequencySpectrum(const std::string& resultName, const std::string& fileName, const std::string& quantityName, ResultManager& result1D)
{
	result1D.convert1D(resultName, fileName, quantityName, "", "Frequency", entityUnits->getFrequencyUnit(), entityUnits->getScaleToSIFrequency());
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
	visualizationEntity->setName(solverEntity->getName() + "/Results/3D Results/" + resultName);
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
	visualizationEntity->setName(solverEntity->getName() + "/Results/3D Results/" + resultName);
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

std::list<std::map<int, double>> FDTDSolver::parseExcitations(std::string_view input)
{
	std::list<std::map<int, double>> excitationList;
	std::size_t pos = 0;

	const auto skipWhitespace = [&]()
		{
			while (pos < input.size() &&
				std::isspace(static_cast<unsigned char>(input[pos])))
			{
				++pos;
			}
		};

	const auto fail = [&](const std::string& message)
		{
			throw std::string("Incorrect excitation port setting; " + message + " at position " + std::to_string(pos));
		};

	skipWhitespace();

	if (pos == input.size())
		fail("Excitation string is empty");

	while (pos < input.size())
	{
		std::map<int, double> excitation;

		while (true)
		{
			skipWhitespace();

			// Parse the port number
			const std::size_t portBegin = pos;

			while (pos < input.size() &&
				std::isdigit(static_cast<unsigned char>(input[pos])))
			{
				++pos;
			}

			if (portBegin == pos)
				fail("Expected a positive port number");

			int portNumber = 0;

			const auto portResult = std::from_chars(
				input.data() + portBegin,
				input.data() + pos,
				portNumber);

			if (portResult.ec != std::errc{} || portNumber < 1)
				fail("Invalid port number");

			skipWhitespace();

			// The default excitation amplitude is 1.0
			double amplitude = 1.0;

			if (pos < input.size() && input[pos] == '(')
			{
				++pos;
				skipWhitespace();

				const std::size_t amplitudeBegin = pos;
				const std::size_t closingBracket = input.find(')', pos);

				if (closingBracket == std::string_view::npos)
					fail("Missing closing bracket");

				std::size_t amplitudeEnd = closingBracket;

				while (amplitudeEnd > amplitudeBegin &&
					std::isspace(static_cast<unsigned char>(
						input[amplitudeEnd - 1])))
				{
					--amplitudeEnd;
				}

				if (amplitudeBegin == amplitudeEnd)
					fail("Excitation amplitude is empty");

				const char* first = input.data() + amplitudeBegin;
				const char* last = input.data() + amplitudeEnd;

				// std::from_chars does not necessarily accept a leading '+'
				if (first != last && *first == '+')
					++first;

				if (first == last)
					fail("Invalid excitation amplitude");

				const auto amplitudeResult = std::from_chars(
					first,
					last,
					amplitude,
					std::chars_format::general);

				if (amplitudeResult.ec != std::errc{} ||
					amplitudeResult.ptr != last)
				{
					fail("Invalid excitation amplitude");
				}

				pos = closingBracket + 1;
			}

			if (!excitation.emplace(portNumber, amplitude).second)
			{
				fail(
					"Port " + std::to_string(portNumber) +
					" occurs more than once in one excitation");
			}

			skipWhitespace();

			if (pos == input.size() || input[pos] == ',')
				break;

			if (input[pos] != '+')
				fail("Expected '+', ',' or end of string");

			++pos;
			skipWhitespace();

			if (pos == input.size() ||
				input[pos] == '+' ||
				input[pos] == ',')
			{
				fail("Expected a port number after '+'");
			}
		}

		excitationList.push_back(std::move(excitation));

		if (pos == input.size())
			break;

		// Skip the comma between solver runs
		++pos;
		skipWhitespace();

		if (pos == input.size())
			fail("Trailing comma");
	}

	return excitationList;
}

bool FDTDSolver::parsePortNumber(const std::string& name, int& portNumber)
{
	const std::size_t slashPos = name.find_last_of('/');

	const std::string portName =
		slashPos == std::string::npos
		? name
		: name.substr(slashPos + 1);

	if (portName.empty())
		return false;

	const char* begin = portName.data();
	const char* end = begin + portName.size();

	const auto result = std::from_chars(begin, end, portNumber);

	return result.ec == std::errc{} &&
		result.ptr == end &&
		portNumber >= 1;
}
