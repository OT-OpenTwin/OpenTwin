#include "MicroServiceInterfaceFITTDSolver.h"

#include "SystemDependencies/SystemProperties.h"
#include "SystemDependencies/Allignments.h"
#include "SystemDependencies/SystemDependentDefines.h"

#include "CurlCoefficients/CurlCoefficients3D.h"
#include "Simulation/FITTD3DBuilder.h"

#include "Ports/Port.h"
#include "Ports/PortFactory.h"
#include "Ports/PortGaussian.h"
#include "Ports/PortSinusGaussian.h"
#include "Ports/SourceTarget.h"

#include "Grid/Grid.h"
#include "Grid/GridIteratorSubvolume.h"
#include "Grid/GridIteratorVolume.h"

#include "ResultPipeline/ResultPipeline.h"
#include "ResultFilter/ResultSourceScalar.h"
#include "ResultFilter/ResultSourceScalarComplex.h"
#include "ResultFilter/ResultSourceVector3DComplex.h"
#include "ResultFilter/ResultPipelineProgressUpdater.h"
#include "ResultFilter/ResultFilterDFT.h"
#include "ResultFilter/ResultFilterEdgeToNode.h"
#include "ResultPostProcessors/ResultPostProcessingComplexNormalization.h"
#include "ResultHandling/ExecutionBarrier.h"
#include "ResultHandling/ExecutionBarrierFixedTimesteps.h"
#include "ResultHandling/ExecutionBarrierFrequencyAndLast.h"
#include "ResultHandling/ResultSinkFilePrinter.h"

#include "OTModelAPI/ModelServiceAPI.h"

#include "FolderNames.h"

#include <chrono>
#include <thread>
#include <sstream>
#include <chrono>

MicroServiceInterfaceFITTDSolver::~MicroServiceInterfaceFITTDSolver()
{
	if (_resultPipelineSettings != nullptr)
	{
		delete _resultPipelineSettings;
		_resultPipelineSettings = nullptr;
	}
	if (_solver != nullptr)
	{
		delete _solver;
		_solver = nullptr;
	}
	delete _mesh;
	_mesh = nullptr;
}

void MicroServiceInterfaceFITTDSolver::CreateSolver()
{
	assert(solverName != "");
	try
	{
		LoadEntityInformation(solverName);
		_performanceTracker.StartTimer();
		InitializeSolver();
		double setupTime = _performanceTracker.StopTimer();
		_performanceTracker.SetSetupTime(setupTime);
	}
	catch (std::exception& e)
	{
		std::string errorMessage = "Solver creation failed. " + std::string(e.what());
		throw std::exception(errorMessage.c_str());
	}
}

void MicroServiceInterfaceFITTDSolver::LoadEntityInformation(std::string solverName)
{

	_solverSettings = LoadSolverSettings(solverName);
	_mesh = LoadMeshData(_solverSettings.GetMeshName());
	uiComponent->displayMessage("Mesh loaded successfully.\n");

	_signals.clear();
	std::list<std::string> ports = ot::ModelServiceAPI::getListOfFolderItems(solverName + "/" + FolderNames::GetFolderNamePorts());
	for (std::string port : ports)
	{
		PortSettings newPort = LoadPortSetting(port, solverName);
		std::list<Signal>::iterator it;
		for ( it = _signals.begin(); it != _signals.end(); it++)
		{
			if (it->GetSignalName() == newPort.GetExcitationSignalName())
			{
				 newPort.SetExcitationSignal(*it);
			}
		}
		if (it == _signals.end())
		{
			Signal newSignal = LoadSignal(newPort.GetExcitationSignalName());
			_signals.push_back(newSignal);
			newPort.SetExcitationSignal(newSignal);
		}
		_portSettings.push_back(newPort);
	}
	uiComponent->displayMessage(std::to_string(_portSettings.size()) + " port(s) loaded.\n");
	
	if (_portSettings.size() == 0 || _signals.size() == 0)
	{
		throw std::logic_error("At least one signal and port have to be defined.");
	}
	if (_signals.size() != 1)
	{
		throw std::invalid_argument("Only a single signal description can be used by all ports.");
	}

	//Load all monitor settings
	std::list<std::string> monitors = ot::ModelServiceAPI::getListOfFolderItems(solverName + "/" + FolderNames::GetFolderNameMonitors());
	for (std::string monitor : monitors)
	{
		_monitorSettings.push_back(LoadMonitorSetting(monitor));
	}
	uiComponent->displayMessage(std::to_string(_monitorSettings.size()) + " monitor(s) loaded.\n");
}

void MicroServiceInterfaceFITTDSolver::InitializeSolver()
{
	using type = float;
	Grid<type> * grid = nullptr;
	CurlCoefficients3D<type> * coefficients = nullptr;
	CurlCoefficients3D<type> * coefficientsDual = nullptr;
	FITTD3DLoop<type> * simulation = nullptr;

	try
	{
		//Grid creation
		index_t dofX = static_cast<index_t>(_mesh->getNumberLinesX());
		index_t dofY = static_cast<index_t>(_mesh->getNumberLinesY());
		index_t dofZ = static_cast<index_t>(_mesh->getNumberLinesZ());

		//grid = new Grid<type>(dofX, dofY, dofZ, _solverSettings.GetAlignment());
		grid = new Grid<type>(dofX, dofY, dofZ, CacheLine64);
		_performanceTracker.SetNumberOfNodes(grid->GetDegreesOfFreedomNumberTotal());

		index_t centerX((dofX - 1) / 2);
		index_t centerY((dofY - 1) / 2);
		index_t centerZ((dofZ - 1) / 2);

		//Creation of CurlCoefficients
		std::vector<double> permittivity;
		bool matrixLoadSuccess = _mesh->applyDepsMatrix(permittivity, false);
		std::vector<double> permeability;
		matrixLoadSuccess = _mesh->applyDmuMatrix(permeability, false);

		if (permeability.size() != permittivity.size())
		{
			throw	std::invalid_argument("Materialproperty vectors have not the same size.");
		}

		//For testing if something is wrong with the mesh data:
		//index_t size = dofX * dofY * dofZ;
		//
		//std::vector<double> dS;
		//_mesh->applyDsMatrix(dS, false);
		//std::vector<double> dA;
		//_mesh->applyDaMatrix(dA, false);
		//std::vector<double> testEdge(size * 3, dS[2]);
		//std::vector<double> testMaterial (size * 3,1.);
		//std::vector<double> testSurface(size * 3,dA[2]);
		//
		//MaterialProperties materialProperties(testMaterial.data(), testMaterial.data(), size);
		//EdgeDiscretization edgeDiscretization;
		//edgeDiscretization.ExtractComponentsFromSingleVector(testEdge);
		//SurfaceDiscretization surfaceDiscretization;
		//surfaceDiscretization.ExtractComponentsFromSingleVector(testSurface);
		//coefficients = new CurlCoefficients3D<type>(*grid);
		//coefficients->SummarizeDescretizationAsCoefficients(surfaceDiscretization, edgeDiscretization, materialProperties, &MaterialProperties::GetPermeabilityInX, &MaterialProperties::GetPermeabilityInY, &MaterialProperties::GetPermeabilityInZ);
		//coefficientsDual = new CurlCoefficients3D<type>(*grid);
		//coefficientsDual->SummarizeDescretizationAsCoefficients(surfaceDiscretization, edgeDiscretization, materialProperties, &MaterialProperties::GetPermittivityInX, &MaterialProperties::GetPermittivityInY, &MaterialProperties::GetPermittivityInZ);

		MaterialProperties materialProperties(permeability.data(), permittivity.data(), permeability.size() / 3);
		{
			std::vector<double> dS;
			_mesh->applyDsMatrix(dS, false);
			EdgeDiscretization edgeDiscretization;
			edgeDiscretization.ExtractComponentsFromSingleVector(dS);

			std::vector<double> dA;
			_mesh->applyDaMatrix(dA, false);
			SurfaceDiscretization surfaceDiscretization;
			surfaceDiscretization.ExtractComponentsFromSingleVector(dA);
			coefficients = new CurlCoefficients3D<type>(*grid);
			coefficients->SummarizeDescretizationAsCoefficients(surfaceDiscretization, edgeDiscretization, materialProperties, &MaterialProperties::GetPermeabilityInX, &MaterialProperties::GetPermeabilityInY, &MaterialProperties::GetPermeabilityInZ);
		}
		{
			std::vector<double> dSDual;
			_mesh->applyDualDsMatrix(dSDual, false);
			EdgeDiscretization edgeDiscretization;
			edgeDiscretization.ExtractComponentsFromSingleVector(dSDual);

			std::vector<double> dADual;
			_mesh->applyDualDaMatrix(dADual, false);
			SurfaceDiscretization surfaceDiscretization;
			surfaceDiscretization.ExtractComponentsFromSingleVector(dADual);
			coefficientsDual = new CurlCoefficients3D<type>(*grid);
			coefficientsDual->SummarizeDescretizationAsCoefficients(surfaceDiscretization, edgeDiscretization, materialProperties, &MaterialProperties::GetPermittivityInX, &MaterialProperties::GetPermittivityInY, &MaterialProperties::GetPermittivityInZ);
		}

		//coefficients->TestValues(0.00150232145f);
		//coefficientsDual->TestValues(213.092438f);

		//Create FITTD handler
		fittd3DBuilder.AddCurlCoefficientsSP(coefficients);
		fittd3DBuilder.AddCurlCoefficientsDualSP(coefficientsDual);
		fittd3DBuilder.AddGridSP(grid);
		simulation = fittd3DBuilder.CreateSimulationSP();

		//Setup ports
		std::vector<Port<type>*> ports;
		ports.reserve(_portSettings.size());
		PortFactory factory;

		for (auto portSetting : _portSettings)
		{
			index_t sourceIndex;
			index_t x, y, z;
			if (portSetting.LocationIsInCentre())
			{
				x = centerX;
				y = centerY;
				z = centerZ;
			}
			else
			{
				x = portSetting.GetCoorX();
				y = portSetting.GetCoorY();
				z = portSetting.GetCoorZ();
			}
			sourceIndex = grid->GetIndexToCoordinate(x, y, z);
			Point3D point(x, y, z);
			std::pair<Point3D, index_t> portLocation(point, sourceIndex);
			ports.push_back(factory.CreatePort(portSetting, portLocation));
		}

		//Setup resultpipelines
		std::vector<ResultPipeline*> resultPipelines;
		int totalSimSteps = _solverSettings.GetSimulationSteps();
		auto doF = simulation->GetDegreesOfFreedom();
		GridIteratorVolume * volumeIterator = grid->GetGridVolumeIterator();

		//Add Signal plots (only a single signal for all ports is currently supported)
		Point3D portLocation = ports[0]->GetPoint();
		auto subVolumeIterator = new GridIteratorSubvolume(*volumeIterator);

		subVolumeIterator->SetXSubspace(portLocation.GetCoordinateX(), portLocation.GetCoordinateX());
		subVolumeIterator->SetYSubspace(portLocation.GetCoordinateY(), portLocation.GetCoordinateY());
		subVolumeIterator->SetZSubspace(portLocation.GetCoordinateZ(), portLocation.GetCoordinateZ());
		int plotFrequency = 1;
		ExecutionBarrier* executionBarrier = new ExecutionBarrierFrequencyAndLast(plotFrequency, totalSimSteps - 1);
		//ResultSource * source = new ResultSourceScalar<type>(plotFrequency, totalSimSteps, subVolumeIterator,*doF,&DegreesOfFreedom3DLoopCPU<type>::GetElectricVoltageXComponent);
		ResultSource * source = new ResultSourceScalarComplex<type>(executionBarrier, subVolumeIterator, *ports[0], &Port<type>::GetCurrentValue);
		ResultSink * sink = new ResultSinkScalarAccumalating(plotFrequency, totalSimSteps, static_cast<double>(coefficients->GetTimeDiscretization()));
		auto newPipeline = new ResultPipeline(source, sink);
		newPipeline->SetResultName(ports[0]->GetSignalName());
		newPipeline->SetLabelXAxis("Time");
		newPipeline->SetUnitXAxis("s");

		newPipeline->SetLabelYAxis("Magnitude");
		newPipeline->SetUnitYAxis("-");
		std::string legendName = ports[0]->GetSignalName() + "_CoC";
		newPipeline->SetResultLegendLabel("Default Signal");
		resultPipelines.push_back(newPipeline);

		//Progressbar
		int numberOfProgressupdates = 4;
		int executionFrequency = totalSimSteps / numberOfProgressupdates;
		resultPipelines[0]->AddResultFilter(new ResultFilterProgressUpdater(executionFrequency, totalSimSteps, uiComponent));

		//Add monitors
		for (auto setting : _monitorSettings)
		{
			ResultSink * newSink = nullptr;
			ResultSource * newSource = nullptr;

			int samplingFrequency;
			setting.GetSamplingFrequency() <= 0 ? samplingFrequency = 1 : samplingFrequency = setting.GetSamplingFrequency();

			sourceType monitoredField = setting.GetMonitoredField();
			std::string labelXAxis, unitXAxis, labelStarter, labelYAxis, unitYAxis, label, point;

			IGridIteratorVolume * volume = nullptr;

			ExecutionBarrier* executionBarrier = nullptr;
			if (setting.GetDomain() == timeDomain && setting.GetMonitorVolume() == full)
			{
				executionBarrier = new ExecutionBarrierFixedTimesteps({ setting.GetObservedTimeStep() });
			}
			else
			{
				executionBarrier = new ExecutionBarrierFrequencyAndLast(samplingFrequency, totalSimSteps - 1);
			}

			if (setting.GetMonitorVolume() == full)
			{
				volume = volumeIterator;
			}
			else
			{
				index_t x, y, z;
				x = centerX;
				y = centerY;
				z = centerZ;
				Point3D monitorPoint(x, y, z);
				point = monitorPoint.Print();
				auto subVolumeIterator = new GridIteratorSubvolume(*volumeIterator);
				subVolumeIterator->SetXSubspace(x, x);
				subVolumeIterator->SetYSubspace(y, y);
				subVolumeIterator->SetZSubspace(z, z);
				volume = subVolumeIterator;
			}

			if (setting.GetMonitorQuantity() == vector)
			{
				newSource = new ResultSourceVector3DComplex<type>(executionBarrier, volume, *doF, monitoredField);
				if (setting.GetMonitorVolume() == full)
				{
					newSink = new ResultSinkVector3DComplexSum(samplingFrequency, totalSimSteps);
				}
				else
				{
					newSink = new ResultSinkVector3DAccumalating(samplingFrequency, totalSimSteps, static_cast<double>(coefficients->GetTimeDiscretization()));
					if (setting.GetMonitoredField() == e)
					{
						labelStarter = "E";
						labelYAxis = "Electric Voltage";
						unitYAxis = "V";
					}
					else
					{
						labelStarter = "H";
						labelYAxis = "Magnetic Voltage";
						unitYAxis = "A";
					}
				}
			}
			else
			{
				type *(DegreesOfFreedom3DLoopCPU<type>::* GetterDoF)(index_t index) const;
				if (setting.GetMonitorQuantity() == x_component)
				{
					if (setting.GetMonitoredField() == e)
					{
						labelStarter = "Ex";
						labelYAxis = "Electric Voltage";
						unitYAxis = "V";
						GetterDoF = &DegreesOfFreedom3DLoopCPU<type>::GetElectricVoltageXComponent;
					}
					else
					{
						labelStarter = "Hx";
						labelYAxis = "Magnetic Voltage";
						unitYAxis = "A";
						GetterDoF = &DegreesOfFreedom3DLoopCPU<type>::GetMagneticVoltageXComponent;
					}
				}
				else if (setting.GetMonitorQuantity() == y_component)
				{
					if (setting.GetMonitoredField() == e)
					{
						labelStarter = "Ey";
						labelYAxis = "Electric Voltage";
						unitYAxis = "V";
						GetterDoF = &DegreesOfFreedom3DLoopCPU<type>::GetElectricVoltageYComponent;
					}
					else
					{
						labelStarter = "Hy";
						labelYAxis = "Magnetic Voltage";
						unitYAxis = "A";
						GetterDoF = &DegreesOfFreedom3DLoopCPU<type>::GetMagneticVoltageYComponent;
					}
				}
				else
				{
					if (setting.GetMonitoredField() == e)
					{
						labelStarter = "Ez";
						labelYAxis = "Electric Voltage";
						unitYAxis = "V";
						GetterDoF = &DegreesOfFreedom3DLoopCPU<type>::GetElectricVoltageZComponent;
					}
					else
					{
						labelStarter = "Hz";
						labelYAxis = "Magnetic Voltage";
						unitYAxis = "A";
						GetterDoF = &DegreesOfFreedom3DLoopCPU<type>::GetMagneticVoltageZComponent;
					}
				}


				newSource = new ResultSourceScalar<type>(executionBarrier, volume, *doF, GetterDoF);
				if (setting.GetMonitorVolume() == full)
				{
					//Visualization not supported yet!
					newSink = new ResultSinkScalarComplexSum(samplingFrequency, totalSimSteps);
				}
				else
				{
					newSink = new ResultSinkScalarAccumalating(samplingFrequency, totalSimSteps, static_cast<double>(coefficients->GetTimeDiscretization()));
				}
			}

			size_t index = setting.GetMonitorName().find_last_of("/");
			label = labelStarter + point + "_" + setting.GetMonitorName().substr(index + 1);
			ResultPipeline * newPipeline = new ResultPipeline(newSource, newSink);
			newPipeline->SetLabelXAxis("Time");
			newPipeline->SetUnitXAxis("s");
			newPipeline->SetLabelYAxis(labelYAxis);
			newPipeline->SetUnitYAxis(unitYAxis);
			newPipeline->SetResultLegendLabel(label);

			if (setting.GetDomain() == frequencyDomain)
			{
				if (ports.size() > 1)
				{
					throw std::logic_error("Capturing the result of the frequency domain is currently only supported with a single port.");
				}
				double signalFrequency = setting.GetObservedFrequency();
				newPipeline->AddResultFilter(new ResultFilterDFT(samplingFrequency, totalSimSteps, signalFrequency));

				//Add normalization by FD transformed prot signal
				Point3D portLocation = ports[0]->GetPoint();
				auto subVolumeIterator = new GridIteratorSubvolume(*volumeIterator);

				subVolumeIterator->SetXSubspace(portLocation.GetCoordinateX(), portLocation.GetCoordinateX());
				subVolumeIterator->SetYSubspace(portLocation.GetCoordinateY(), portLocation.GetCoordinateY());
				subVolumeIterator->SetZSubspace(portLocation.GetCoordinateZ(), portLocation.GetCoordinateZ());

				executionBarrier = new ExecutionBarrierFrequencyAndLast(samplingFrequency, totalSimSteps - 1);
				auto normalizationSource = new ResultSourceScalarComplex<type>(executionBarrier, subVolumeIterator, *ports[0], &Port<type>::GetCurrentValue);
				auto normalizationSink = new ResultSinkScalarComplexSum(samplingFrequency, totalSimSteps);
				auto dftNormalization = new ResultPipeline(normalizationSource, normalizationSink);
				dftNormalization->AddResultFilter(new ResultFilterDFT(samplingFrequency, totalSimSteps, signalFrequency));
				resultPipelines.push_back(dftNormalization);
				newSink->AddPostProcessor(new ResultPostProcessingComplexNormalization(normalizationSink));
			}

			if (setting.GetMonitoredGeometry() == nodes)
			{
				//This filter shall only be executed at the end. Thus execution frequency > totalSimSteps.
				newPipeline->AddResultFilter(new ResultFilterEdgeToNode(totalSimSteps + 1, totalSimSteps));
			}

			newPipeline->SetResultName(setting.GetMonitorName());

			resultPipelines.push_back(newPipeline);
		}

		//Debug:
		//auto portPoint = ports[0]->GetPoint();
		//std::string path = "C:\\Users\\Barbarossa\\Desktop\\";
		//std::string headline =
		//	"DoFX: " + std::to_string(grid->GetDegreesOfFreedomNumberInX()) + ", DoFY: " + std::to_string(grid->GetDegreesOfFreedomNumberInY()) + ", DoFZ: " + std::to_string(grid->GetDegreesOfFreedomNumberInZ()) +
		//	"\n Excitation at: " + portPoint.Print() + " with " + ports[0]->GetSignalName();

		//std::vector<int> snapShots{ 0,1,2,3,4 };

		//for (int i = 0; i < snapShots.size(); i++)
		//{
		//	std::string fullPath = path + "Snapshot"+ solverName[solverName.length()-1] +"_t" + std::to_string(snapShots[i])+".txt";

		//	ExecutionBarrier* executionBarrier =  new ExecutionBarrierFixedTimesteps({snapShots[i]});
		//	auto source = new ResultSourceVector3DComplex<float>(executionBarrier, volumeIterator, *doF, e);
		//	auto sink = new ResulSinkFilePrinter(1, totalSimSteps, fullPath, headline);
		//	resultPipelines.push_back(new ResultPipeline(source, sink));
		//}

		_solver = new Solver(_solverSettings.GetSimulationSteps(), ports, resultPipelines, simulation, _solverSettings);
		uiComponent->displayMessage("Solver setup successful.\n");

	}
	catch (const std::exception&)
	{
		if (_solver != nullptr)
		{
			delete _solver;
			_solver = nullptr;
		}
		else if(simulation != nullptr)
		{
			delete simulation;
			simulation = nullptr;
		}
		else
		{
			if (grid != nullptr)
			{
				delete grid;
			}
			if (coefficients != nullptr)
			{
				delete coefficients;
				coefficients = nullptr;
			}
			if (coefficientsDual != nullptr)
			{
				delete coefficientsDual;
				coefficientsDual = nullptr;
			}
		}
		throw;
	}
}



void MicroServiceInterfaceFITTDSolver::Run(void)
{
	try
	{
		_performanceTracker.StartTimer();
		_solver->SimulateMultipleTimesteps();
		double simulationDuration = _performanceTracker.StopTimer();
		_performanceTracker.SetSimulationTime(simulationDuration);
		HandleResultPipelines();
		StoreLogDataInResultItem(CreateSolverRunDescription());
		UpdateModel();
	}
	catch (std::string e)
	{
		displayMessage("Solver run failed due to: " + e);
	}
}

void MicroServiceInterfaceFITTDSolver::HandleResultPipelines()
{
	auto pipelines = _solver->GetResultPipelines();
	for (auto pipeLine : pipelines)
	{
		auto resultSink = pipeLine->GetSink();
		resultSink->PerformPostProcessing();

		auto result2D = dynamic_cast<const ResultSinkScalarAccumalating*>(resultSink);
		if (result2D != nullptr)
		{
			HandleTimelinePlots(result2D,pipeLine);
			continue;
		}
		auto multiple2DPlots = dynamic_cast<const ResultSinkVector3DAccumalating*>(resultSink);
		if(multiple2DPlots != nullptr)
		{
			HandleTimelinePlots(multiple2DPlots, pipeLine);
			continue;
		}
		auto result3DVector = dynamic_cast<const ResultSinkVector3DComplexSum*>(resultSink);
		if (result3DVector != nullptr)
		{
			SaveVectorFieldResult(result3DVector,pipeLine);
			continue;
		}
	}
}

void MicroServiceInterfaceFITTDSolver::HandleTimelinePlots(const ResultSinkScalarAccumalating * resultSink, ResultPipeline * pipeline)
{
	std::list<std::pair<ot::UID, std::string>> curvesList;
	const double * timeVector = resultSink->GetTimesteps();

	int timeVectorSize = resultSink->GetNbOfExectutions();
	std::vector<double> xdata(timeVector, timeVector + timeVectorSize);

	const double* resultVector = resultSink->GetResult();
	index_t collectionSize = resultSink->GetResultContainerSize();
	std::vector<double> ydataRe(resultVector, resultVector + collectionSize);

	curvesList.push_back(CreateCurve(xdata, ydataRe, pipeline->GetResultLegendLabel(), pipeline));
	addPlot1D(pipeline->GetResultName() + "/Plot", pipeline->GetResultTitle(), curvesList);
}

void MicroServiceInterfaceFITTDSolver::HandleTimelinePlots(const ResultSinkVector3DAccumalating * resultSink, ResultPipeline * pipeline)
{
	std::list<std::pair<ot::UID, std::string>> curvesList;
	const double * timeVector = resultSink->GetTimesteps();

	int timeVectorSize = resultSink->GetNbOfExectutions();
	std::vector<double> xdata(timeVector, timeVector + timeVectorSize);
	
	for (int i = 0; i < 3; i++)
	{
		const double* resultVector;

		std::string curveName;
		if (i == 0)
		{
			resultVector = resultSink->GetResultX();
			curveName = pipeline->GetResultLegendLabel();
			curveName.insert(1, "x");
		}
		else if (i == 1)
		{
			resultVector = resultSink->GetResultY();
			curveName = pipeline->GetResultLegendLabel();
			curveName.insert(1, "y");
		}
		else
		{
			resultVector = resultSink->GetResultZ();
			curveName = pipeline->GetResultLegendLabel();
			curveName.insert(1, "z");
		}

		index_t collectionSize = resultSink->GetResultContainerSize();
		std::vector<double> ydataRe(resultVector, resultVector + collectionSize);
		curvesList.push_back(CreateCurve(xdata, ydataRe, curveName, pipeline));
	}
	
	addPlot1D(pipeline->GetResultName() + "/Plot", pipeline->GetResultTitle(), curvesList);
}

std::pair<ot::UID, std::string> MicroServiceInterfaceFITTDSolver::CreateCurve(std::vector<double> & timeLine, std::vector<double> & measuredValues, std::string curveName, ResultPipeline * pipeline)
{
	std::vector<double>  ydataIm;
	std::string physicalQuantity = pipeline->GetLabelYAxis();
	std::string unit = pipeline->GetUnitYAxis();
	std::string temporalQuantity = pipeline->GetLabelXAxis();
	std::string temporalUnit = pipeline->GetUnitXAxis();

	return addResultCurve(curveName, timeLine, measuredValues, ydataIm, temporalQuantity, temporalUnit, physicalQuantity, unit, 0, _solverSettings.GetDebug());
}

void MicroServiceInterfaceFITTDSolver::SaveVectorFieldResult(const ResultSinkVector3DComplexSum *resultSink, ResultPipeline * pipeline)
{
	std::string title = "Title";
	
	auto result = resultSink->GetResultContainer();
	double * xResult = result->GetRealComponentX();
	double * yResult = result->GetRealComponentY();
	double * zResult = result->GetRealComponentZ();

	index_t size = resultSink->GetVolumeSize();

	addResultFD(pipeline->GetResultName(), title, xResult,yResult,zResult,size);
}

std::string MicroServiceInterfaceFITTDSolver::CreateSolverRunDescription()
{
	std::string solverrunLog = solverName + " ended successfully\n" +
		"Settings: \n" + _solverSettings.Print() +
		"Performance: \n" +
		_performanceTracker.Print();
	
	solverrunLog += "\nPorts:\n";
	for (auto port : _portSettings)
	{
		solverrunLog += port.Print();
	}

	solverrunLog += "\nMonitors:\n";
	for (auto monitor : _monitorSettings)
	{
		solverrunLog += monitor.Print();
	}

	solverrunLog += "\nSignals:\n";
	for (auto signal : _signals)
	{
		solverrunLog += signal.Print();
	}

	return solverrunLog;
}
