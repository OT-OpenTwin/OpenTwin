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
#include "OTCADEntities/EntityFieldDump.h"

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
#include <cmath>
#include <array>
#include <cstddef>
#include <cstdint>
#include <numeric>

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

	for (auto fieldDump : fieldDumpList)
	{
		delete fieldDump;
	}
}

std::string FDTDSolver::generateRunCommand()
{
	std::stringstream runCommand;
	runCommand << std::defaultfloat << std::setprecision(12);

	readPorts();
	readExcitation();
	readFieldDumps();

	checkCartesianMesh(runCommand);

	addPreparationData(runCommand);
	addUnits(runCommand);
	addSolverSetup(runCommand);
	addMesh(runCommand);
	addGeometry(runCommand);
	addPorts(runCommand);
	addFieldDumps(runCommand);
	addSolverRun(runCommand);
	addPostprocessing(runCommand);

	return runCommand.str();
}

void FDTDSolver::addFieldDumps(std::stringstream& runCommand)
{
	int count = 1;

	for (auto fieldDump : fieldDumpList)
	{
		int dumpType = 0, dumpMode = 0;
		if (!getFieldDumpTypeAndMode(fieldDump, dumpType, dumpMode))
		{
			throw std::string("Unknown field type for field dump: " + fieldDump->getName());
		}
		else
		{
			std::string frequencyString;
			
			if (isFrequencyDump(fieldDump))
			{
				frequencyString = getFrequencyString(fieldDump);
				if (frequencyString.empty())
				{
					throw std::string("Invalid frequencies list for field dump: " + fieldDump->getName());
				}
			}
			
			std::string subSamplingString = getSubsamplingString(fieldDump);
			std::string resulutionString = getResolutionString(fieldDump);
			std::string fieldDumpName = getFieldDumpName(fieldDump);
			std::string startStopString = getStartStopString(fieldDump);

			runCommand << "dump" << count << " = CSX.AddDump('" << fieldDumpName << "', dump_type=" << dumpType << ", dump_mode=" << dumpMode << ", file_type=0";

			if (!frequencyString.empty())
			{
				runCommand << ", frequency=[" << frequencyString << "]";
			}

			if (!subSamplingString.empty())
			{
				runCommand << ", sub_sampling=[" << subSamplingString << "]";
			}

			if (!subSamplingString.empty())
			{
				runCommand << ", resolution=[" << resulutionString << "]";
			}

			runCommand << ")\n";

			runCommand << startStopString << "dump" << count << ".AddBox(start, stop)\n";

			count++;
		}
	}
}

bool FDTDSolver::getFieldDumpTypeAndMode(EntityFieldDump *fieldDump, int &dumpType, int &dumpMode)
{
	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(fieldDump->getProperties().getProperty("Type"));
	if (typeProperty == nullptr) return false;

	if (typeProperty->getValue() == "Electric Field (Frequency Domain)")
	{
		dumpType = 10;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "Magnetic Field (Frequency Domain)")
	{
		dumpType = 11;
		dumpMode = 1;
	}
	else if (typeProperty->getValue() == "Electric Current Density (Frequency Domain)")
	{
		dumpType = 12;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "Total Current Density (Frequency Domain)")
	{
		dumpType = 13;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "Electric Field (Time Domain)")
	{
		dumpType = 0;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "Magnetic Field (Time Domain)")
	{
		dumpType = 1;
		dumpMode = 1;
	}
	else if (typeProperty->getValue() == "Electric Current Density (Time Domain)")
	{
		dumpType = 2;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "Total Current Density (Time Domain)")
	{
		dumpType = 3;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "Local SAR (Frequency Domain)")
	{
		dumpType = 20;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "1 g Averaged SAR (Frequency Domain)")
	{
		dumpType = 21;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "10 g Averaged SAR (Frequency Domain)")
	{
		dumpType = 22;
		dumpMode = 2;
	}
	else if (typeProperty->getValue() == "SAR Raw Data (Frequency Domain)")
	{
		dumpType = 29;
		dumpMode = 2;
	}
	else
	{
		assert(0); // Unknown field type
		return false;
	}

	return true;
}

bool FDTDSolver::getFieldTypeAndUnit(EntityFieldDump* fieldDump, std::string& fieldType, std::string& unit)
{
	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(fieldDump->getProperties().getProperty("Type"));
	if (typeProperty == nullptr) return false;

	if (typeProperty->getValue() == "Electric Field (Frequency Domain)")
	{
		fieldType = "E-Field";
		unit = "V/m";
	}
	else if (typeProperty->getValue() == "Magnetic Field (Frequency Domain)")
	{
		fieldType = "H-Field";
		unit = "A/m";
	}
	else if (typeProperty->getValue() == "Electric Current Density (Frequency Domain)")
	{
		fieldType = "J-Field";
		unit = "A/m^2";
	}
	else if (typeProperty->getValue() == "Total Current Density (Frequency Domain)")
	{
		fieldType = "Jtot-Field";
		unit = "A/m^2";
	}
	else if (typeProperty->getValue() == "Electric Field (Time Domain)")
	{
		fieldType = "E-Field";
		unit = "V/m";
	}
	else if (typeProperty->getValue() == "Magnetic Field (Time Domain)")
	{
		fieldType = "H-Field";
		unit = "A/m";
	}
	else if (typeProperty->getValue() == "Electric Current Density (Time Domain)")
	{
		fieldType = "J-Field";
		unit = "A/m^2";
	}
	else if (typeProperty->getValue() == "Total Current Density (Time Domain)")
	{
		fieldType = "Jtot-Field";
		unit = "A/m^2";
	}
	else if (typeProperty->getValue() == "Local SAR (Frequency Domain)")
	{
		fieldType = "SAR";
		unit = "W/kg";
	}
	else if (typeProperty->getValue() == "1 g Averaged SAR (Frequency Domain)")
	{
		fieldType = "SAR-1g";
		unit = "W/kg";
	}
	else if (typeProperty->getValue() == "10 g Averaged SAR (Frequency Domain)")
	{
		fieldType = "SAR-10g";
		unit = "W/kg";
	}
	else if (typeProperty->getValue() == "SAR Raw Data (Frequency Domain)")
	{
		fieldType = "SAR-Raw";
		unit = "";
	}
	else
	{
		assert(0); // Unknown field type
		return false;
	}

	return true;
}

bool FDTDSolver::isFrequencyDump(EntityFieldDump* fieldDump)
{
	EntityPropertiesSelection* typeProperty = dynamic_cast<EntityPropertiesSelection*>(fieldDump->getProperties().getProperty("Type"));
	if (typeProperty == nullptr) return false;

	bool isFrequencyDomain = typeProperty->getValue().find("(Frequency Domain)") != std::string::npos;

	return isFrequencyDomain;
}

std::string FDTDSolver::getFrequencyString(EntityFieldDump* fieldDump)
{
	EntityPropertiesString* frequencyProperty = dynamic_cast<EntityPropertiesString*>(fieldDump->getProperties().getProperty("Frequencies"));
	if (frequencyProperty == nullptr) return "";

	std::list<double> values;
	std::istringstream stream(frequencyProperty->getValue());
	std::string item;

	while (std::getline(stream, item, ','))
	{
		if (item.empty()) return ""; // Empty value in frquency list

		std::size_t parsedCharacters = 0;
		const double value = std::stod(item, &parsedCharacters);

		// Allow whitespace after the number, but no other characters.
		if (item.find_first_not_of(
			" \t\r\n",
			parsedCharacters) != std::string::npos)
		{
			return ""; // Invalid double value
		}

		values.push_back(value);
	}

	std::stringstream result;
	result << std::defaultfloat << std::setprecision(6);

	bool isFirst = true;
	for (double value : values)
	{
		if (!isFirst) result << ", ";
		isFirst = false;

		result << value * entityUnits->getScaleToSIFrequency();
	}

	return result.str();
}

std::string FDTDSolver::getSubsamplingString(EntityFieldDump* fieldDump)
{
	EntityPropertiesSelection* subsamplingProperty = dynamic_cast<EntityPropertiesSelection*>(fieldDump->getProperties().getProperty("Mode"));
	if (subsamplingProperty == nullptr) return "";

	if (subsamplingProperty->getValue() != "Index based") return "";

	EntityPropertiesInteger* xProperty = dynamic_cast<EntityPropertiesInteger*>(fieldDump->getProperties().getProperty("Step x"));
	EntityPropertiesInteger* yProperty = dynamic_cast<EntityPropertiesInteger*>(fieldDump->getProperties().getProperty("Step y"));
	EntityPropertiesInteger* zProperty = dynamic_cast<EntityPropertiesInteger*>(fieldDump->getProperties().getProperty("Step z"));

	if (xProperty == nullptr || yProperty == nullptr || zProperty == nullptr)
	{
		assert(0);
		return "";
	}

	std::stringstream result;
	result << xProperty->getValue() << ", " << yProperty->getValue() << ", " << zProperty->getValue();

	return result.str();
}

std::string FDTDSolver::getResolutionString(EntityFieldDump* fieldDump)
{
	EntityPropertiesSelection* subsamplingProperty = dynamic_cast<EntityPropertiesSelection*>(fieldDump->getProperties().getProperty("Mode"));
	if (subsamplingProperty == nullptr) return "";

	if (subsamplingProperty->getValue() != "Resolution based") return "";

	EntityPropertiesDouble* xProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Resolution x"));
	EntityPropertiesDouble* yProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Resolution y"));
	EntityPropertiesDouble* zProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Resolution z"));

	if (xProperty == nullptr || yProperty == nullptr || zProperty == nullptr)
	{
		assert(0);
		return "";
	}

	std::stringstream result;
	result << std::defaultfloat << std::setprecision(6);
	result << xProperty->getValue() * entityUnits->getScaleToSIDimension() << ", " << yProperty->getValue() * entityUnits->getScaleToSIDimension() << ", " << zProperty->getValue() * entityUnits->getScaleToSIDimension();

	return result.str();
}

std::string FDTDSolver::getFieldDumpName(EntityFieldDump* fieldDump)
{
	std::string prefix = solverEntity->getName() + "/Field Dumps/";

	std::string name = fieldDump->getName();

	if (name.starts_with(prefix)) name.erase(0, prefix.size());

	return name;
}

std::string FDTDSolver::getStartStopString(EntityFieldDump* fieldDump)
{
	double xMin = xLines.front();
	double xMax = xLines.back();
	double yMin = yLines.front();
	double yMax = yLines.back();
	double zMin = zLines.front();
	double zMax = zLines.back();

	EntityPropertiesBoolean* subrangeProperty = dynamic_cast<EntityPropertiesBoolean*>(fieldDump->getProperties().getProperty("Subrange"));
	
	if (subrangeProperty != nullptr)
	{
		if (subrangeProperty->getValue())
		{
			EntityPropertiesDouble* xminProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Xmin"));
			EntityPropertiesDouble* xmaxProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Xmax"));
			EntityPropertiesDouble* yminProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Ymin"));
			EntityPropertiesDouble* ymaxProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Ymax"));
			EntityPropertiesDouble* zminProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Zmin"));
			EntityPropertiesDouble* zmaxProperty = dynamic_cast<EntityPropertiesDouble*>(fieldDump->getProperties().getProperty("Zmax"));

			if (xminProperty != nullptr && xmaxProperty != nullptr && yminProperty != nullptr && ymaxProperty != nullptr && zminProperty != nullptr && zmaxProperty != nullptr)
			{
				xMin = xminProperty->getValue();
				xMax = xmaxProperty->getValue();
				yMin = yminProperty->getValue();
				yMax = ymaxProperty->getValue();
				zMin = zminProperty->getValue();
				zMax = zmaxProperty->getValue();
			}
			else
			{
				assert(0); // Property missing
			}
		}
	}

	std::stringstream result;
	result << std::defaultfloat << std::setprecision(6);
	result << "start = [" << xMin << ", " << yMin << ", " << zMin << "]\n";
	result << "stop  = [" << xMax << ", " << yMax << ", " << zMax << "]\n";

	return result.str();
}

void FDTDSolver::addPorts(std::stringstream& runCommand)
{
	if (waveguidePortList.empty()) return;

	// Here we create all ports first with excitation set on (this will be updated to the actual values in the solver run part).
	// This is necessary to create the excitation data structures.
	ot::UIDList facetIDList;
	for (auto port : waveguidePortList)
	{
		facetIDList.push_back(port->getFacetsStorageID());
	}

	std::list<ot::EntityInformation> facetEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(facetIDList, facetEntitiesInfo);

	// Read all facet entities
	DataBase::instance().prefetchDocumentsFromStorage(facetEntitiesInfo);

	runCommand << "ports = {}\n";

	for (auto port : waveguidePortList)
	{
		int portNumber = std::stoi(port->getNameOnly());

		EntityFacetData* facetEntity = dynamic_cast<EntityFacetData*>(ot::EntityAPI::readEntityFromEntityIDandVersion(facetEntitiesInfo.front().getEntityID(), facetEntitiesInfo.front().getEntityVersion()));
		facetEntitiesInfo.pop_front();
		if (facetEntity != nullptr)
		{
			EntityPropertiesSelection* modeProperty = dynamic_cast<EntityPropertiesSelection*>(port->getProperties().getProperty("Mode"));
			if (modeProperty == nullptr)
			{
				throw std::string("Invalid mode setting for port: " + port->getName());
			}

			std::string mode = modeProperty->getValue();

			// Determine the exact port bounding box and the average port normal
			double nx = 0.0, ny = 0.0, nz = 0.0;
			BoundingBox box;

			for (auto& node : facetEntity->getNodeVector())
			{
				box.extend(node.getCoord(0), node.getCoord(1), node.getCoord(2));
				nx += node.getNormal(0);
				ny += node.getNormal(1);
				nz += node.getNormal(2);
			}

			nx /= -1.0 * facetEntity->getNodeVector().size();  // -, since the port face is pointing outward of the waveguide
			ny /= -1.0 * facetEntity->getNodeVector().size();
			nz /= -1.0 * facetEntity->getNodeVector().size();

			double tolerance = 1e-4;

			if (fabs(ny) < tolerance && fabs(nz) < tolerance && fabs(fabs(nx) - 1.0) < tolerance)
			{
				// Port normal +/- x direction (u = y, v = z)
				double from = 0.0, to = 0.0;
				findPortRange(0.5 * (box.getXmin() + box.getXmax()), xLines, xminBoundary, xmaxBoundary, nx, from, to);

				runCommand << "start=[" << from << ", " << box.getYmin() << ", " << box.getZmin() << "];\n";
				runCommand << "stop =[" << to << ", " << box.getYmax() << ", " << box.getZmax() << "];\n";
				runCommand << "ports[" << portNumber << "] = FDTD.AddRectWaveGuidePort( " << portNumber << ", start, stop, 'x', " << box.getYmax() - box.getYmin() << "*gunit, " << box.getZmax() - box.getZmin() << "*gunit, '" << mode << "', 1)\n";

			}
			else if (fabs(nx) < tolerance && fabs(nz) < tolerance && fabs(fabs(ny) - 1.0) < tolerance)
			{
				// Port normal +/- y direction (u = z, v = x)
				double from = 0.0, to = 0.0;
				findPortRange(0.5 * (box.getYmin() + box.getYmax()), yLines, yminBoundary, ymaxBoundary, ny, from, to);

				runCommand << "start=[" << box.getXmin() << ", " << from << ", " << box.getZmin() << "];\n";
				runCommand << "stop =[" << box.getXmax() << ", " << to << ", " << box.getZmax() << "];\n";
				runCommand << "ports[" << portNumber << "] = FDTD.AddRectWaveGuidePort( " << portNumber << ", start, stop, 'y', " << box.getZmax() - box.getZmin() << "*gunit, " << box.getXmax() - box.getXmin() << "*gunit, '" << mode << "', 1)\n";

			}
			else if (fabs(nx) < tolerance && fabs(ny) < tolerance && fabs(fabs(nz) - 1.0) < tolerance)
			{
				// Port normal +/- z direction (u = x, v = y)
				double from = 0.0, to = 0.0;
				findPortRange(0.5 * (box.getZmin() + box.getZmax()), zLines, zminBoundary, zmaxBoundary, nz, from, to);

				runCommand << "start=[" << box.getXmin() << ", " << box.getYmin() << ", " << from << "];\n";
				runCommand << "stop =[" << box.getXmax() << ", " << box.getYmax() << ", " << to << "];\n";
				runCommand << "ports[" << portNumber << "] = FDTD.AddRectWaveGuidePort( " << portNumber << ", start, stop, 'z', " << box.getXmax() - box.getXmin() << "*gunit, " << box.getYmax() - box.getYmin() << "*gunit, '" << mode << "', 1)\n";
			}

			delete facetEntity;
			facetEntity = nullptr;
		}
	}
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

void FDTDSolver::readFieldDumps()
{
	std::list<std::string> fieldDumpEntityNames = ot::ModelServiceAPI::getListOfFolderItems(solverEntity->getName() + "/Field Dumps", false);
	if (fieldDumpEntityNames.empty()) return;

	std::list<ot::EntityInformation> fieldDumpEntitiesInfo;
	ot::ModelServiceAPI::getEntityInformation(fieldDumpEntityNames, fieldDumpEntitiesInfo);

	// Read all geometry entities
	DataBase::instance().prefetchDocumentsFromStorage(fieldDumpEntitiesInfo);

	for (auto dump : fieldDumpEntitiesInfo)
	{
		EntityBase* fieldDumpEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(dump.getEntityID(), dump.getEntityVersion());

		EntityFieldDump* fieldDump = dynamic_cast<EntityFieldDump*>(fieldDumpEntity);
		if (fieldDump != nullptr)
		{
			fieldDumpList.push_back(fieldDump);
		}
		else if (fieldDumpEntity != nullptr)
		{
			std::string error = "The field dump named " + fieldDumpEntity->getName() + " is not a supported type";
			delete fieldDumpEntity;

			throw error;
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

	runCommand << "\n"
		"def save_xy_data(x, y, filename) :\n"
		"		full_path = os.path.join(Sim_Path, filename)\n"
		"		with open(full_path, 'w', encoding = 'utf-8') as f :\n"
		"			for xi, yi in zip(x, y) :\n"
		"				if isinstance(yi, complex) :\n"
		"					f.write(f'{xi}\t{yi.real}\t{yi.imag}\\n')\n"
		"				else :\n"
		"					f.write(f'{xi}\t{yi}\\n')\n\n";
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
	// We copy the node vector, since we want to make sure that all close points are snapped together
	auto nodes = facetData->getNodeVector();
	mergeCloseNodes(nodes, 1e-9);

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
	EntityPropertiesInteger*   oversamplingProperty    = dynamic_cast<EntityPropertiesInteger*>(solverEntity->getProperties().getProperty("Oversampling"));

	std::string excitationType = (excitationTypeProperty != nullptr) ? excitationTypeProperty->getValue() : "Gaussian";
	double fMin                = (fMinProperty != nullptr) ? fMinProperty->getValue() : 0.0;
	double fMax                = (fMaxProperty != nullptr) ? fMaxProperty->getValue() : 0.0;
	long long fSteps           = (fStepsProperty != nullptr) ? fStepsProperty->getValue() : 201;
	long long maxTimesteps     = (maxTimestepsProperty != nullptr) ? maxTimestepsProperty->getValue() : 1000000;
	double energyStopLevel     = (energyStopLevelProperty != nullptr) ? energyStopLevelProperty->getValue() : 1e-5;
	int oversamplingFactor     = (oversamplingProperty != nullptr) ? oversamplingProperty->getValue() : 4;

	double centerFreq    = 0.5 * (fMin + fMax) * entityUnits->getScaleToSIFrequency();
	double halfBandwidth = 0.5 * (fMax - fMin) * entityUnits->getScaleToSIFrequency();

	runCommand << "#=================================================================================\n";
	runCommand << "# Setup solver and define frequency ranges\n";
	runCommand << "#=================================================================================\n";

	runCommand << "CSX = ContinuousStructure()\n";
	runCommand << "FDTD = openEMS(NrTS=" << maxTimesteps << ", EndCriteria=" << energyStopLevel << ", OverSampling=" << oversamplingFactor << ")\n";
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

	xminBoundary = (xminBoundaryProperty != nullptr) ? xminBoundaryProperty->getValue() : "PEC";
	xmaxBoundary = (xmaxBoundaryProperty != nullptr) ? xmaxBoundaryProperty->getValue() : "PEC";
	yminBoundary = (yminBoundaryProperty != nullptr) ? yminBoundaryProperty->getValue() : "PEC";
	ymaxBoundary = (ymaxBoundaryProperty != nullptr) ? ymaxBoundaryProperty->getValue() : "PEC";
	zminBoundary = (zminBoundaryProperty != nullptr) ? zminBoundaryProperty->getValue() : "PEC";
	zmaxBoundary = (zmaxBoundaryProperty != nullptr) ? zmaxBoundaryProperty->getValue() : "PEC";

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

	runCommand <<
		"# Store the excitation properties and their original vectors\n"
		"port_excitations = {}\n"
		"\n"
		"for port_number in ports:\n"
		"    properties = CSX.GetPropertiesByName(\n"
		"        f\"port_excite_{port_number}\"\n"
		"    )\n"
		"\n"
		"    if len(properties) != 1:\n"
		"        raise RuntimeError(\n"
		"            f\"Excitation property for port {port_number} not found\"\n"
		"        )\n"
		"\n"
		"    excitation_property = properties[0]\n"
		"\n"
		"    port_excitations[port_number] = {\n"
		"        \"property\": excitation_property,\n"
		"        \"base_vector\": np.array(\n"
		"            excitation_property.GetExcitation(),\n"
		"            dtype=float\n"
		"        )\n"
		"    }\n"
		"\n";

	runCommand << "excitation_list = [\n";

	for (const auto& excitation : excitationList)
	{
		runCommand << "    {";

		bool firstEntry = true;

		for (const auto& [portNumber, amplitude] : excitation)
		{
			if (!firstEntry)
				runCommand << ", ";

			runCommand << portNumber << ": " << amplitude;
			firstEntry = false;
		}

		runCommand << "},\n";
	}

	runCommand << 
		"]\n"
		"\n"
		"for run_index, excitation_settings in enumerate(\n"
		"    excitation_list,\n"
		"    start=1\n"
		"):\n"
		"    for port_number, data in port_excitations.items():\n"
		"        amplitude = excitation_settings.get(\n"
		"            port_number,\n"
		"            0.0\n"
		"        )\n"
		"\n"
		"        data[\"property\"].SetExcitation(\n"
		"            amplitude * data[\"base_vector\"]\n"
		"        )\n"
		"\n"
		"    run_path = os.path.join(\n"
		"        Sim_Path,\n"
		"        f\"run_{run_index}\"\n"
		"    )\n"
		"\n"
		"    os.makedirs(run_path, exist_ok=True)\n";

	if (debugFlag)
	{
		runCommand <<
			"    CSX.Write2XML(\n"
			"        os.path.join(run_path, \"input.xml\")\n"
			"    )\n"
			"\n";
	}

	runCommand <<
		"    excitation_text = \", \".join(\n"
		"        f\"port {port_number}\"\n"
		"        if np.isclose(amplitude, 1.0)\n"
		"        else f\"port {port_number}: {amplitude}\"\n"
		"        for port_number, amplitude in excitation_settings.items()\n"
		"    )\n"
		"\n"
		"    print(\n"
		"        f\"=================================================================================\\n\"\n"
		"        f\"Starting solver run {run_index} of \"\n"
		"        f\"{len(excitation_list)}: excitation {excitation_text}\\n\"\n"
		"        f\"=================================================================================\\n\"\n"
		"    )\n"
		"\n";

	runCommand <<
		"    FDTD.Run(\n"
		"        run_path,\n"
		"        cleanup=False\n"
		"    )\n"
		"\n";
}

void FDTDSolver::addPostprocessing(std::stringstream& runCommand)
{
	runCommand << "#=================================================================================\n";
	runCommand << "# Define post-processing\n";
	runCommand << "#=================================================================================\n";

	runCommand <<
		"### S-parameter postprocessing\n"
		"freq = np.linspace(f_start, f_stop, f_samples)\n"
		"\n"
		"for run_index, excitation_settings in enumerate(excitation_list, start=1):\n"
		"    active_ports = [\n"
		"        port_number\n"
		"        for port_number, amplitude in excitation_settings.items()\n"
		"        if amplitude != 0.0\n"
		"    ]\n"
		"\n"
		"    # Ignore simultaneous port excitations for S-parameter evaluation\n"
		"    if len(active_ports) != 1:\n"
		"        continue\n"
		"\n"
		"    input_port = active_ports[0]\n"
		"    run_path = os.path.join(Sim_Path, f'run_{run_index}')\n"
		"\n"
		"    for port in ports.values():\n"
		"        port.CalcPort(run_path, freq)\n"
		"\n"
		"    incident_wave = ports[input_port].uf_inc\n"
		"\n"
		"    for output_port in sorted(ports.keys()):\n"
		"        s_parameter = (\n"
		"            ports[output_port].uf_ref / incident_wave\n"
		"        )\n"
		"\n"
		"        save_xy_data(\n"
		"            freq,\n"
		"            s_parameter,\n"
		"            f's{output_port},{input_port}'\n"
		"        )\n"
		"\n"
		"    input_impedance = (\n"
		"        ports[input_port].uf_tot /\n"
		"        ports[input_port].if_tot\n"
		"    )\n"
		"\n"
		"    save_xy_data(\n"
		"        freq,\n"
		"        input_impedance,\n"
		"        f'Zin{input_port}'\n"
		"    )\n"
		"\n"
		"    save_xy_data(\n"
		"        freq,\n"
		"        ports[input_port].ZL,\n"
		"        f'ZL_analytic_{input_port}'\n"
		"    )\n"
		"\n";
}

void FDTDSolver::convertAndStoreResults(const std::string& logFileText)
{
	timeStepWidth = readTimeStepWidthFromLogText(logFileText);
	if (timeStepWidth == 0.0) throw(std::string("Unable to determine time step width."));

	ResultManager result1D(application->getModelComponent(), tempDirPath, solverEntity->getName() + "/Results");

	std::size_t runIndex = 1;

	for (const auto& excitation : excitationList)
	{
		std::ostringstream stream;
		stream.imbue(std::locale::classic());
		stream << std::defaultfloat << std::setprecision(6);

		bool firstPort = true;

		for (const auto& [portNumber, amplitude] : excitation)
		{
			if (!firstPort)
				stream << '+';

			stream << portNumber;

			if (amplitude != 1.0)
				stream << '(' << amplitude << ')';

			firstPort = false;
		}

		const std::string excitationString = " (" + stream.str() + ")";

		// Process the current run here
		std::string resultFolderName = "run_" + std::to_string(runIndex) + "\\";

		// Convert the excitation signals
		convert1DTimeSignal("Excitation/E-Field" + excitationString, resultFolderName + "et", "E-Field Excitation", result1D);
		convert1DTimeSignal("Excitation/H-Field" + excitationString, resultFolderName + "ht", "H-Field Excitation", result1D);

		// Convert the port voltages and currents
		for (int port : portList)
		{
			std::string portName = std::to_string(port);

			convert1DTimeSignal("Ports/Currents/I" + portName + excitationString, resultFolderName + "port_it_" + portName, "Port " + portName + " Current", result1D, 1);
			convert1DTimeSignal("Ports/Voltages/V" + portName + excitationString, resultFolderName + "port_ut_" + portName, "Port " + portName + " Voltage", result1D, 1);
		}

		// Convert the field dumps
		convertAndStoreFieldDumps(resultFolderName, excitationString);

		++runIndex;
	}

	// Convert the S-parameters
	convertAndStoreSParameters(result1D);

	result1D.storeResults();
}

void FDTDSolver::convertAndStoreFieldDumps(const std::string &resultFolderName, const std::string &excitationString)
{
	for (auto fieldDump : fieldDumpList)
	{
		std::string fieldDumpName = getFieldDumpName(fieldDump);

		std::string fieldType, unit;
		getFieldTypeAndUnit(fieldDump, fieldType, unit);

		if (isFrequencyDump(fieldDump))
		{
			convertAndStoreFrequencyDomainDump(resultFolderName, fieldDumpName, fieldType, excitationString, unit);
		}
		else
		{
			convertAndStoreTimeDomainDump(resultFolderName, fieldDumpName, fieldType, excitationString, unit);
		}
	}
}

void FDTDSolver::convertAndStoreSParameters(ResultManager &result1D)
{
	std::set<int> processedInputPorts;

	for (const auto& excitation : excitationList)
	{
		int inputPort = 0;
		std::size_t activePortCount = 0;

		for (const auto& [portNumber, amplitude] : excitation)
		{
			if (amplitude != 0.0)
			{
				inputPort = portNumber;
				++activePortCount;
			}
		}

		// Only process single-port excitations.
		if (activePortCount != 1)
			continue;

		// Avoid converting the same S-parameters more than once.
		if (!processedInputPorts.insert(inputPort).second)
			continue;

		for (const int outputPort : portList)
		{
			const std::string indices =
				std::to_string(outputPort) +
				"," +
				std::to_string(inputPort);

			convert1DFrequencySpectrum(
				"S-Parameter/S" + indices,
				"s" + indices,
				"S" + indices,
				result1D);
		}
	}
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

void FDTDSolver::convertAndStoreTimeDomainDump(const std::string& resultFolder, const std::string& resultName, const std::string& fieldType, const std::string& postfix, const std::string& unit)
{
	// Search for files of the form resultName_00010.vtr

	auto escape_regex = [](const std::string& s) {
		return std::regex_replace(s, std::regex(R"([.^$|()\\[*+?{\]])"), R"(\$&)");
	};

	std::list<std::string> resultFileList;

	for (const auto& entry : std::filesystem::directory_iterator(tempDirPath + "\\" + resultFolder)) {
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
		convertAndStoreSingleTimeDomainDump(resultFileList, resultName, fieldType, postfix, unit);
	}
}

void FDTDSolver::convertAndStoreFrequencyDomainDump(const std::string& resultFolder, const std::string &resultName, const std::string& fieldType, const std::string& postfix, const std::string& unit)
{
	// Search for files of the form resultName_f=20000.000_abs.vtr

	auto escape_regex = [](const std::string& s) {
		return std::regex_replace(s, std::regex(R"([.^$|()\\[*+?{\]])"), R"(\$&)");
	};

	for (const auto& entry : std::filesystem::directory_iterator(tempDirPath + "\\" + resultFolder)) {
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
					convertAndStoreSingleFrequencyDomainDump(absFile, argFile, fieldType, postfix, unit);
				}
			}
		}
	}
}

void FDTDSolver::convertAndStoreSingleFrequencyDomainDump(const std::string& absFileName, const std::string& argFileName, const std::string &fieldType, const std::string& postfix, const std::string &unit)
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
	visualizationEntity->setName(solverEntity->getName() + "/Results/3D Results/" + resultName + postfix);
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

void FDTDSolver::convertAndStoreSingleTimeDomainDump(std::list<std::string>& resultFileList, const std::string& resultName, const std::string& fieldType, const std::string& postfix, const std::string& unit)
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
	visualizationEntity->setName(solverEntity->getName() + "/Results/3D Results/" + resultName + postfix);
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

void FDTDSolver::findPortRange(
	double position,
	const std::vector<double>& gridLines,
	const std::string& minBoundary,
	const std::string& maxBoundary,
	double nx,
	double &from,
	double &to)
{
	constexpr std::size_t portIntervals = 5;
	constexpr std::size_t pmlOffset = 10;

	if (gridLines.size() <= portIntervals)
		throw std::string("Not enough grid lines for the port.");

	if (!std::isfinite(position))
		throw std::string("The port position is not finite.");

	if (!std::isfinite(nx) || nx == 0.0)
		throw std::string("The port normal component must not be zero.");

	// Grid lines must be strictly increasing.
	for (std::size_t i = 1; i < gridLines.size(); ++i)
	{
		if (gridLines[i] <= gridLines[i - 1])
		{
			throw std::string("Grid lines must be strictly increasing.");
		}
	}

	if (position < gridLines.front() || position > gridLines.back())
	{
		throw std::string("The port position is outside the grid.");
	}

	const auto isPML = [](const std::string& boundary)
		{
			return boundary.rfind("PML", 0) == 0;
		};

	// Find the grid line closest to the requested position.
	const auto upper = std::lower_bound(
		gridLines.begin(),
		gridLines.end(),
		position);

	std::size_t fromIndex;

	if (upper == gridLines.begin())
	{
		fromIndex = 0;
	}
	else if (upper == gridLines.end())
	{
		fromIndex = gridLines.size() - 1;
	}
	else
	{
		const std::size_t upperIndex =
			static_cast<std::size_t>(upper - gridLines.begin());

		const std::size_t lowerIndex = upperIndex - 1;

		fromIndex =
			std::abs(gridLines[upperIndex] - position) <
			std::abs(position - gridLines[lowerIndex])
			? upperIndex
			: lowerIndex;
	}

	if (nx > 0.0)
	{
		// The wave propagates from the minimum boundary towards + direction.
		if (isPML(minBoundary))
			fromIndex = std::max(fromIndex, pmlOffset);

		if (fromIndex + portIntervals >= gridLines.size())
		{
			throw std::string("Not enough grid lines behind the port in positive direction.");
		}

		from = gridLines[fromIndex];
		to = gridLines[fromIndex + portIntervals];

		return;
	}

	// The wave propagates from the maximum boundary towards - direction.
	if (isPML(maxBoundary))
	{
		if (gridLines.size() <= pmlOffset)
		{
			throw std::string("Not enough grid lines for the requested PML distance.");
		}

		fromIndex = std::min(
			fromIndex,
			gridLines.size() - 1 - pmlOffset);
	}

	if (fromIndex < portIntervals)
	{
		throw std::string("Not enough grid lines behind the port in negative direction.");
	}

	from = gridLines[fromIndex];
	to = gridLines[fromIndex - portIntervals];
}

// C++17. Include the header defining Geometry::Node BEFORE this header.
// Uses getCoord(int) and setCoords(double, double, double).
// The getters do not need to be const. Normals and UV parameters are preserved.
// Merge connected components of the ORIGINAL point-distance graph:
// each pair at Euclidean distance <= tolerance belongs to the same component.
// All component members receive the original coordinates of its first point.
// Connectivity is transitive: long chains can move a point by more than tolerance.
// No entries are removed or reordered; existing triangle indices remain valid.
// Return the number of non-representative points, including exact duplicates.
// Use on export coordinates before float conversion and STL serialization.
// Afterward check triangles for collapsed edges/zero area and check closure.
std::size_t FDTDSolver::mergeCloseNodes(std::vector<Geometry::Node>& nodes, double tolerance)
{
	if (!std::isfinite(tolerance) || tolerance <= 0.0)
		throw std::invalid_argument("Invalid merge tolerance");

	for (auto& node : nodes)
		for (int axis = 0; axis < 3; ++axis)
			if (!std::isfinite(node.getCoord(axis)))
				throw std::invalid_argument("Non-finite node coordinate");

	const std::size_t count = nodes.size();
	if (count < 2)
		return 0;

	using Cell = std::array<std::int64_t, 3>;
	std::map<Cell, std::vector<std::size_t>> grid;
	std::vector<std::size_t> parent(count);
	std::iota(parent.begin(), parent.end(), std::size_t{ 0 });

	auto findRoot = [&](std::size_t index) {
		while (parent[index] != index) {
			parent[index] = parent[parent[index]];
			index = parent[index];
		}
		return index;
		};

	// Subtract a local origin to avoid large absolute spatial-index coordinates.
	const std::array<long double, 3> origin = {
		nodes[0].getCoord(0), nodes[0].getCoord(1), nodes[0].getCoord(2) };

	for (std::size_t i = 0; i < count; ++i) {
		Cell cell;
		for (std::size_t axis = 0; axis < 3; ++axis) {
			// Cell width 2*tolerance leaves a margin for division rounding.
			const long double scaled =
				((static_cast<long double>(nodes[i].getCoord(static_cast<int>(axis)))
					- origin[axis])
					/ static_cast<long double>(tolerance)) * 0.5L;

			// Also safe when long double has only double precision (MSVC).
			if (!std::isfinite(scaled) || std::abs(scaled) > 1.0e14L)
				throw std::out_of_range("Coordinate span too large for tolerance");
			cell[axis] = static_cast<std::int64_t>(std::floor(scaled));
		}

		// Include neighboring cells, not just points with the same rounded key.
		for (int dx = -1; dx <= 1; ++dx)
			for (int dy = -1; dy <= 1; ++dy)
				for (int dz = -1; dz <= 1; ++dz) {
					const Cell neighbor = { cell[0] + dx, cell[1] + dy, cell[2] + dz };
					const auto bucket = grid.find(neighbor);
					if (bucket == grid.end())
						continue;

					for (std::size_t j : bucket->second) {
						std::size_t a = findRoot(i);
						std::size_t b = findRoot(j);
						if (a == b)
							continue;

						const double distance = std::hypot(
							nodes[i].getCoord(0) - nodes[j].getCoord(0),
							nodes[i].getCoord(1) - nodes[j].getCoord(1),
							nodes[i].getCoord(2) - nodes[j].getCoord(2));

						if (distance <= tolerance) {
							if (a > b)
								std::swap(a, b);
							parent[b] = a; // Keep the first point as representative.
						}
					}
				}
		grid[cell].push_back(i);
	}

	// Only now modify coordinates: all searches above used original positions.
	std::size_t merged = 0;
	for (std::size_t i = 0; i < count; ++i) {
		const std::size_t representative = findRoot(i);
		if (i == representative)
			continue;
		nodes[i].setCoords(nodes[representative].getCoord(0),
			nodes[representative].getCoord(1),
			nodes[representative].getCoord(2));
		++merged;
	}
	return merged;
}
