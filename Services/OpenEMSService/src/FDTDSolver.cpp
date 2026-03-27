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


#include "OTModelEntities/EntitySolver.h"
#include "OTModelEntities/EntityMeshCartesian.h"

FDTDSolver::FDTDSolver(EntityBase* _solverEntity, EntityMeshCartesian* _meshEntity, const std::string& _openEMSPath, const std::string& _tempDirPath)
	: solverEntity(_solverEntity), meshEntity(_meshEntity), openEMSPath(_openEMSPath), tempDirPath(_tempDirPath)
{
	assert(solverEntity != nullptr);
	assert(meshEntity != nullptr);
	assert(!openEMSPath.empty());
	assert(!_tempDirPath.empty());
}


std::string FDTDSolver::generateRunCommand()
{
	// addPreparationData
	// addUnits
	// addSolverSetup
	// addMesh
	// addGeometry
	// addPorts
	// addBoundaries
	// addMonitors
	// addSolverRun
	// assPostprocessing

	std::string text =
		"import os, tempfile\n"
		"import numpy as np\n"
		"os.environ[\"OPENEMS_INSTALL_PATH\"] = \"C:\\OT\\OpenTwin\\Deployment\\openEMSSolver\"\n"
		"\n"
		"from CSXCAD  import ContinuousStructure\n"
		"from openEMS import openEMS\n"
		"from openEMS.physical_constants import *\n"
		"\n"
		"print(\"Start Simulation\")\n"
		"### Setup the simulation\n"
		"Sim_Path = os.path.join(tempfile.gettempdir(), 'Rect_WG')\n"
		"print(Sim_Path)\n"
		"\n"
		"post_proc_only = False\n"
		"unit = 1e-6; #drawing unit in um\n"
		"\n"
		"# waveguide dimensions\n"
		"# WR42\n"
		"a = 10700;   #waveguide width\n"
		"b = 4300;    #waveguide height\n"
		"length = 50000;\n"
		"\n"
		"# frequency range of interest\n"
		"f_start = 20e9;\n"
		"f_0     = 24e9;\n"
		"f_stop  = 26e9;\n"
		"lambda0 = C0/f_0/unit;\n"
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
		"mesh.SetDeltaUnit(unit)\n"
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
		"ports.append(FDTD.AddRectWaveGuidePort( 0, start, stop, 'z', a*unit, b*unit, TE_mode, 1))\n"
		"\n"
		"start=[0, 0, length-10*mesh_res];\n"
		"stop =[a, b, length-15*mesh_res];\n"
		"mesh.AddLine('z', [start[2], stop[2]])\n"
		"ports.append(FDTD.AddRectWaveGuidePort( 1, start, stop, 'z', a*unit, b*unit, TE_mode))\n"
		"\n"
		"mesh.SmoothMeshLines('all', mesh_res, ratio=1.4)\n"
		"\n"
		"### Define dump box...\n"
		"#Et = CSX.AddDump('Et', dump_type=10, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop], sub_sampling=[2,2,2])\n"
		"Et = CSX.AddDump('Et', dump_type=10, file_type=0, frequency=[f_start, 0.5*(f_start+f_stop), f_stop],opt_resolution=[0,90,2])\n"
		"start = [0, 0, 0];\n"
		"stop  = [a, b, length];\n"
		"Et.AddBox(start, stop);\n"
		"\n"
		"### Run the simulation\n"
		"if 1:  # debugging only\n"
		"    CSX_file = os.path.join(Sim_Path, 'rect_wg.xml')\n"
		"    if not os.path.exists(Sim_Path):\n"
		"        os.mkdir(Sim_Path)\n"
		"    print(CSX_file)\n"
		"    CSX.Write2XML(CSX_file)\n"
		"    #from CSXCAD import AppCSXCAD_BIN\n"
		"    #os.system(AppCSXCAD_BIN + ' \"{}\"'.format(CSX_file))\n"
		"\n"
		"if not post_proc_only:\n"
		"    FDTD.Run(Sim_Path, cleanup=False)\n"
		"\n"
		"### Postprocessing & plotting\n"
		"freq = np.linspace(f_start,f_stop,201)\n"
		"for port in ports:\n"
		"    port.CalcPort(Sim_Path, freq)\n"
		"\n"
		"s11 = ports[0].uf_ref / ports[0].uf_inc\n"
		"s21 = ports[1].uf_ref / ports[0].uf_inc\n"
		"ZL  = ports[0].uf_tot / ports[0].if_tot\n"
		"ZL_a = ports[0].ZL # analytic waveguide impedance\n"
		"\n";

	return text;
}

/*
void FDTDSolver::addVectorResult(const std::string& resultName, char* fileData, int data_length, EntityBase* solverEntity)
{
	EntityBinaryData* vtkData = new EntityBinaryData(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkData->setData(fileData, data_length + 1);
	vtkData->storeToDataBase();

	ot::UID vtkDataEntityID = vtkData->getEntityID();
	ot::UID vtkDataEntityVersion = vtkData->getEntityStorageVersion();

	EntityResultUnstructuredMeshVtk* vtkResult = new EntityResultUnstructuredMeshVtk(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	vtkResult->setData(resultName, EntityResultUnstructuredMeshVtk::VECTOR, vtkData);
	vtkResult->storeToDataBase();

	EntityVisUnstructuredVectorSurface* visualizationEntity = new EntityVisUnstructuredVectorSurface(getModelComponent()->createEntityUID(), nullptr, nullptr, nullptr);
	visualizationEntity->setName(solverEntity->getName() + "/Results/" + resultName);
	visualizationEntity->setResultType(EntityResultBase::UNSTRUCTURED_VECTOR);
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

	visualizationEntity->storeToDataBase();

	getModelComponent()->addNewTopologyEntity(visualizationEntity->getEntityID(), visualizationEntity->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(vtkDataEntityID, vtkDataEntityVersion, vtkResult->getEntityID());
	getModelComponent()->addNewDataEntity(vtkResult->getEntityID(), vtkResult->getEntityStorageVersion(), visualizationEntity->getEntityID());

	delete visualizationEntity;
	visualizationEntity = nullptr;

	delete vtkResult;
	vtkResult = nullptr;
	
}
*/

void FDTDSolver::convertAndStoreResults()
{
	/*
	EntityResultText* text = this->getModelComponent()->addResultTextEntity(solver.getEntityName() + "/Output", logFileText);

	getModelComponent()->addNewTopologyEntity(text->getEntityID(), text->getEntityStorageVersion(), false);
	getModelComponent()->addNewDataEntity(text->getTextDataStorageId(), text->getTextDataStorageVersion(), text->getEntityID());

	// TEMPORARY: Read the result data file and create a new result entity
	std::ifstream file("result.vtu", std::ios::binary | std::ios::ate);
	int data_length = (int)file.tellg();

	if (data_length != -1)
	{
		file.seekg(0, std::ios::beg);

		char* fileData = new char[data_length + 1];
		file.read(fileData, data_length);
		fileData[data_length] = 0;

		addScalarResult("energy_density", fileData, data_length, solverEntity);
		addScalarResult("region IDs", fileData, data_length, solverEntity);
		addScalarResult("reluctivity", fileData, data_length, solverEntity);
		addScalarResult("vector_potential", fileData, data_length, solverEntity);

		addVectorResult("flux_density", fileData, data_length, solverEntity);
		addVectorResult("magnetic_field", fileData, data_length, solverEntity);

		delete[] fileData;
		fileData = nullptr;
	}
	*/
}