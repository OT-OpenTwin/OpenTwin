// @otlicense
// File: BlockEntityHandler.cpp
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

// Service header
#include "BlockEntityHandler.h"

// Open twin header
#include "Connection.h"
#include "Application.h"
#include "CircuitElement.h"
#include "SimulationResults.h"

#include "OTCore/EntityName.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
#include "OTGui/Painter/PainterRainbowIterator.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include "OTResultDataAccess/PlotBuilder.h"
#include "OTResultDataAccess/ResultCollection/ResultCollectionExtender.h"
#include "OTResultDataAccess/SerialisationInterfaces/QuantityDescriptionCurve.h"
#include "OTCore/MetadataHandle/MetadataQuantity.h"
#include "OTResultDataAccess/CurveFactory.h"
#include "OTModelEntities/EntityResult1DPlot.h"
#include "OTModelEntities/EntityResult1DCurve.h"
#include "OTCore/FolderNames.h"
#include "OTBlockEntities/EntityBlockConnection.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitVoltageSource.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitResistor.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitDiode.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitVoltageMeter.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitCurrentMeter.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitCapacitor.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitInductor.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitElement.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitGND.h"
#include "OTBlockEntities/Circuit/EntityBlockCircuitTransmissionLine.h"


// Third Party Header

//C++
#include <algorithm>
#include <queue>

void BlockEntityHandler::createBlockPicker() {
	ot::JsonDocument doc;
	
	auto pckg = buildUpBlockPicker();
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_FillItemPicker, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, ot::JsonObject(pckg, doc.GetAllocator()), doc.GetAllocator());
	
	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	std::string tmp;
	_uiComponent->sendMessage(true, doc, tmp);
}

std::map<ot::UID, std::shared_ptr<ot::EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID(const std::string& _folderName) {
	std::list<std::string> blockItemNames = ot::ModelServiceAPI::getListOfFolderItems(_folderName, true);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	
	std::map<ot::UID, std::shared_ptr<ot::EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() != "EntityBlockConnection") { //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others 
			std::shared_ptr<ot::EntityBlock> blockEntity(dynamic_cast<ot::EntityBlock*>(baseEntity));
			if (blockEntity != nullptr) {
				blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
			}
		}

	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections(const std::string& _folderName) {
	const std::string fullFolderName = _folderName + "/" + m_connectionsFolder;
	std::list<std::string> connectionItemNames = ot::ModelServiceAPI::getListOfFolderItems(fullFolderName);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	std::map<ot::UID, std::shared_ptr<ot::EntityBlockConnection>> entityBlockConnectionsByBlockID;
	ot::EntityBlockConnection temp;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() == temp.getClassName()) {
			std::shared_ptr<ot::EntityBlockConnection> blockEntityConnection(dynamic_cast<ot::EntityBlockConnection*>(baseEntity));
			if (blockEntityConnection != nullptr) {
				entityBlockConnectionsByBlockID[blockEntityConnection->getEntityID()] = blockEntityConnection;
			}
		}
	}

	return entityBlockConnectionsByBlockID;	
}

std::shared_ptr<EntityFileText> BlockEntityHandler::getCircuitModel(const std::string& _folderName,std::string _modelName) {
	ot::EntityInformation circuitModelInfo;
	ot::ModelServiceAPI::getEntityInformation("Circuit Models/" + _folderName + "/" + _modelName, circuitModelInfo);

	auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(circuitModelInfo.getEntityID(), circuitModelInfo.getEntityVersion());
	if (baseEntity != nullptr) {
		std::shared_ptr<EntityFileText> circuitModelEntity(dynamic_cast<EntityFileText*>(baseEntity));
		assert(circuitModelEntity != nullptr);
		return circuitModelEntity;
	}
	else {
		OT_LOG_E("No CircuitModelEntity found: " + _modelName);
		return nullptr;
	}

}

ot::GraphicsPickerCollectionPackage BlockEntityHandler::buildUpBlockPicker() {
	ot::GraphicsPickerCollectionPackage graphicsPicker;
	ot::GraphicsPickerCollectionCfg a("CircuitElements", "Circuit Elements");
	ot::GraphicsPickerCollectionCfg a1("PassiveElements", "Passive Elements");
	ot::GraphicsPickerCollectionCfg a2("Meter Elements", "Meter Elements");
	ot::GraphicsPickerCollectionCfg a3("Sources", "Sources");
	ot::GraphicsPickerCollectionCfg a4("ActiveElements", "Active Elements");
	
	a1.addItem(EntityBlockCircuitResistor::className(), "Resistor", "CircuitElementImages/ResistorBG.png");
	a1.addItem(EntityBlockCircuitCapacitor::className(),"Capacitor", "CircuitElementImages/Capacitor.png");
	a1.addItem(EntityBlockCircuitInductor::className(), "Inductor", "CircuitElementImages/Inductor.png");
	a1.addItem(EntityBlockCircuitGND::className(), "GND", "CircuitElementImages/GND.png");
	a1.addItem(EntityBlockCircuitTransmissionLine::className(), "Transmission Line", "CircuitElementImages/TranLine.png");

	a2.addItem(EntityBlockCircuitVoltageMeter::className(), "Voltage Meter", "CircuitElementImages/VoltMeter.png");
	a2.addItem(EntityBlockCircuitCurrentMeter::className(), "Current Meter", "CircuitElementImages/CurrentMeter.png");

	a3.addItem(EntityBlockCircuitVoltageSource::className(), "Voltage Source", "CircuitElementImages/VoltageSource.png");
	

	a4.addItem(EntityBlockCircuitDiode::className(), "Diode", "CircuitElementImages/Diod2.png");

	a.addChildCollection(std::move(a1));
	a.addChildCollection(std::move(a2));
	a.addChildCollection(std::move(a3));
	a.addChildCollection(std::move(a4));

	graphicsPicker.addCollection(std::move(a));
	graphicsPicker.setPickerKey(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
	return graphicsPicker;
}

void BlockEntityHandler::createResultCurves(std::string solverName, std::string simulationType, std::string circuitName)
{
	OT_LOG_D("=== createResultCurves START === solverName=" + solverName + ", simulationType=" + simulationType);

	std::map<std::string, std::vector<double>> resultVectors = SimulationResults::getInstance()->getResultMap();

	if (resultVectors.empty()) {
		OT_LOG_E("No result vectors available");
		return;
	}

	OT_LOG_D("ResultVectors size: " + std::to_string(resultVectors.size()));

	ResultCollectionExtender extender(Application::instance());

	// Normalize simulation type to uppercase for comparison
	std::transform(simulationType.begin(), simulationType.end(), simulationType.begin(), ::tolower);

	std::vector<double> xValues;
	std::string normalizedSimType;

	if (simulationType == ".dc") {
		auto it = resultVectors.find("v-sweep");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("v-sweep");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No v-sweep vector found");
			return;
		}
		normalizedSimType = "DC";
	}
	else if (simulationType == ".tran") {
		auto it = resultVectors.find("time");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("time");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No time vector found");
			return;
		}
		normalizedSimType = "TRAN";
	}
	else if (simulationType == ".ac") {
		auto it = resultVectors.find("frequency");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("frequency");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No frequency vector found");
			return;
		}
		normalizedSimType = "AC";
	}
	else {
		OT_LOG_E("Unknown simulation type: " + simulationType);
		return;
	}

	if (xValues.empty()) {
		OT_LOG_E("X-values are empty for simulation type: " + normalizedSimType);
		return;
	}

	std::string solverNameShort = solverName;
	size_t posSlash = solverName.find_last_of('/');
	if (posSlash != std::string::npos) {
		solverNameShort = solverName.substr(posSlash + 1);
	}

	MetadataParameter baseParameter;
	baseParameter.typeName = ot::TypeNames::getDoubleTypeName();
	baseParameter.parameterName = (normalizedSimType == "DC" ? "sweep" : 
									normalizedSimType == "TRAN" ? "time" : "frequency");
	baseParameter.parameterLabel = baseParameter.parameterName; 
	baseParameter.unit = normalizedSimType == "DC" ? "V" : 
						  normalizedSimType == "TRAN" ? "ms" : "hz";
	for (const double& value : xValues) {
		baseParameter.values.push_back(ot::Variable(value));
	}
	xValues.clear();

	if (baseParameter.values.empty()) {
		OT_LOG_E("Failed to populate parameter values for curves");
		return;
	}

	OT_LOG_D("BaseParameter created: name=" + baseParameter.parameterName + ", label=" + baseParameter.parameterLabel 
		+ ", unit=" + baseParameter.unit + ", #values=" + std::to_string(baseParameter.values.size()));

	// Remove specific unwanted vectors
	auto branchIt = resultVectors.find("v1#branch");
	if (branchIt != resultVectors.end()) {
		resultVectors.erase(branchIt);
	}

	// Remove all vectors containing "ediff"
	for (auto it = resultVectors.begin(); it != resultVectors.end();) {
		if (it->first.find("ediff") != std::string::npos) {
			it = resultVectors.erase(it);
		}
		else {
			++it;
		}
	}

	if (resultVectors.empty()) {
		OT_LOG_E("No result vectors remaining after filtering");
		return;
	}

	const std::string plotFolderPath = solverName + "/" + "Results";
	const std::string plotNameVoltage = "/" + normalizedSimType + "-Voltage";
	const std::string plotNameCurrent = "/" + normalizedSimType + "-Current";
	const std::string fullPlotNameVoltage = plotFolderPath + plotNameVoltage;
	const std::string fullPlotNameCurrent = plotFolderPath + plotNameCurrent;

	ot::ModelServiceAPI::deleteEntitiesFromModel({ fullPlotNameVoltage, fullPlotNameCurrent }, false);

	ot::PainterRainbowIterator rainbowPainterIt;
	auto& nameMap = Application::instance()->getNGSpice().netlistNameToCustomNameMap;

	OT_LOG_D("Starting to create curves. Total result vectors: " + std::to_string(resultVectors.size()));

	std::shared_ptr<ParameterDescription> sharedParameterDescr(new ParameterDescription(baseParameter, false));
	
	// Collect all dataset descriptions
	std::list<DatasetDescription> allCurveDescriptions;
	
	// Process Voltage Curves
	for (auto& resultPair : resultVectors) {
		if (resultPair.second.empty()) continue;

		bool isVoltage = (resultPair.first.find("V(") != std::string::npos || resultPair.first.find("vd_") != std::string::npos);
		if (!isVoltage) continue;

		std::unique_ptr<QuantityDescriptionCurve> quantity(new QuantityDescriptionCurve());
		for (const double& value : resultPair.second) {
			quantity->addDatapoint(ot::Variable(value));
		}

		DatasetDescription dataset;
		dataset.addParameterDescription(sharedParameterDescr);

		std::string delimiter = "#branch";
		std::size_t pos = resultPair.first.find(delimiter);
		std::string key = (pos != std::string::npos) ? resultPair.first.substr(0, pos) : resultPair.first;
		std::string displayName = (nameMap.find(key) != nameMap.end()) ? nameMap[key] : resultPair.first;
		std::string curveName = displayName + "-" + normalizedSimType;

		quantity->setName(curveName);
		quantity->defineQuantityAsSingle(ot::TypeNames::getDoubleTypeName(), "V");

		// Set the quantity label for Y-axis display (curve name with unit)
		quantity->getMetadataQuantity().quantityLabel = curveName + " [V]";

		QuantityDescriptionCurve* quantityPtr = quantity.release();
		dataset.setQuantityDescription(quantityPtr);

		allCurveDescriptions.push_back(std::move(dataset));
	}
	
	// Process Current Curves
	for (auto& resultPair : resultVectors) {
		if (resultPair.second.empty()) continue;

		bool isVoltage = (resultPair.first.find("V(") != std::string::npos || resultPair.first.find("vd_") != std::string::npos);
		if (isVoltage) continue;

		std::unique_ptr<QuantityDescriptionCurve> quantity(new QuantityDescriptionCurve());
		for (const double& value : resultPair.second) {
			quantity->addDatapoint(ot::Variable(value));
		}

		DatasetDescription dataset;
		dataset.addParameterDescription(sharedParameterDescr);

		std::string delimiter = "#branch";
		std::size_t pos = resultPair.first.find(delimiter);
		std::string key = (pos != std::string::npos) ? resultPair.first.substr(0, pos) : resultPair.first;
		std::string displayName = (nameMap.find(key) != nameMap.end()) ? nameMap[key] : resultPair.first;
		std::string curveName = displayName + "-" + normalizedSimType;

		quantity->setName(curveName);
		quantity->defineQuantityAsSingle(ot::TypeNames::getDoubleTypeName(), "A");

		// Set the quantity label for Y-axis display (curve name with unit)
		quantity->getMetadataQuantity().quantityLabel = curveName + " [I]";

		QuantityDescriptionCurve* quantityPtr = quantity.release();
		dataset.setQuantityDescription(quantityPtr);

		allCurveDescriptions.push_back(std::move(dataset));
	}

	if (allCurveDescriptions.empty()) {
		OT_LOG_E("No curves generated.");
		return;
	}

	//extender.registerCallbacks(
	//	ot::EntityCallbackBase::Callback::Properties |
	//	ot::EntityCallbackBase::Callback::Selection |
	//	ot::EntityCallbackBase::Callback::DataNotify,
	//	Application::instance()->getServiceName()
	//);
	extender.setSaveModel(false);

	std::string seriesName = ot::FolderNames::DatasetFolder + "/" + solverNameShort;
	uint64_t seriesMetadataIndex = extender.buildSeriesMetadata(allCurveDescriptions, seriesName);
	extender.storeCampaignChanges();

	const MetadataSeries* series = extender.findMetadataSeries(seriesMetadataIndex);

	bool hasVoltages = false;
	bool hasCurrents = false;
	for (auto& ds : allCurveDescriptions) {
		const std::string& qName = ds.getQuantityDescription()->getName();
		if (qName.find("V(") != std::string::npos || qName.find("vd_") != std::string::npos) hasVoltages = true;
		else hasCurrents = true;
	}

	auto modelComponent = Application::instance()->getModelComponent();

	if (hasVoltages) {
		EntityResult1DPlot newPlotVoltages(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
		newPlotVoltages.setName(fullPlotNameVoltage);
		newPlotVoltages.createProperties();

		ot::Plot1DCfg plotCfg;
		plotCfg.setTitle("Voltages");
		plotCfg.setXAxisParameter(sharedParameterDescr->getMetadataParameter().parameterLabel);
		plotCfg.setYAxisLabel("Voltage [V]");
		plotCfg.setYAxisLabelAutoDetermine(false);
		newPlotVoltages.setStaticCurveQueryOptions(plotCfg);
		newPlotVoltages.storeToDataBase();
		modelComponent->addNewTopologyEntity(newPlotVoltages.getEntityID(), newPlotVoltages.getEntityStorageVersion(), false);
	}
	
	if (hasCurrents) {
		EntityResult1DPlot newPlotCurrents(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
		newPlotCurrents.setName(fullPlotNameCurrent);
		newPlotCurrents.createProperties();

		ot::Plot1DCfg plotCfg;
		plotCfg.setTitle("Currents");
		plotCfg.setXAxisParameter(sharedParameterDescr->getMetadataParameter().parameterLabel);
		plotCfg.setYAxisLabel("Current [A]");
		plotCfg.setYAxisLabelAutoDetermine(false);
		newPlotCurrents.setStaticCurveQueryOptions(plotCfg);
		newPlotCurrents.storeToDataBase();
		modelComponent->addNewTopologyEntity(newPlotCurrents.getEntityID(), newPlotCurrents.getEntityStorageVersion(), false);
	}

	// Create and push curves
	for (DatasetDescription& dataDescription : allCurveDescriptions)
	{
		extender.processDataPoints(&dataDescription, seriesMetadataIndex);

		std::string fullName = dataDescription.getQuantityDescription()->getName();
		bool isVoltageCurve = (fullName.find("V(") != std::string::npos || fullName.find("vd_") != std::string::npos);

		ot::Plot1DCurveCfg curveConfig;
		curveConfig.setTitle(fullName);
		curveConfig.setLinePenPainter(rainbowPainterIt.getNextPainter().release());
		curveConfig.setEntityName((isVoltageCurve ? fullPlotNameVoltage : fullPlotNameCurrent) + "/" + fullName);

		CurveFactory::addToConfig(*series, dataDescription.getQuantityDescription()->getMetadataQuantity(), curveConfig, Application::instance(), &extender);

		EntityResult1DCurve newCurve(modelComponent->createEntityUID(), nullptr, nullptr, nullptr);
		newCurve.setName(curveConfig.getEntityName());
		newCurve.createProperties();
		newCurve.setStaticCurveQueryOptions(curveConfig);
		newCurve.storeToDataBase();

		modelComponent->addNewTopologyEntity(newCurve.getEntityID(), newCurve.getEntityStorageVersion(), false);
	}

	modelComponent->storeNewEntities("Created new plots", false);

	OT_LOG_D("=== createResultCurves END ===");
}

const std::string BlockEntityHandler::getInitialCircuitName() const {
	return this->m_initialCircuitName;
}
