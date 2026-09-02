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
	PlotBuilder plotBuilderCurrent(extender, Application::instance());
	PlotBuilder plotBuilderVoltage(extender, Application::instance());

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
	baseParameter.parameterName = solverNameShort + "_" + (normalizedSimType == "DC" ? "sweep" : 
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

	ot::PainterRainbowIterator rainbowPainterIt;
	auto& nameMap = Application::instance()->getNGSpice().netlistNameToCustomNameMap;

	OT_LOG_D("Starting to create curves. Total result vectors: " + std::to_string(resultVectors.size()));

	std::shared_ptr<ParameterDescription> sharedParameterDescr(new ParameterDescription(baseParameter, false));

	for (auto& resultPair : resultVectors) {
		if (resultPair.second.empty()) {
			OT_LOG_W("Skipping curve with empty y-values: " + resultPair.first);
			continue;
		}

		OT_LOG_D("Processing curve: " + resultPair.first + " with " + std::to_string(resultPair.second.size()) + " y-values");

		std::string curveName;
		std::string yUnit = "V";

		// Extract base name without branch suffix
		std::string delimiter = "#branch";
		std::size_t pos = resultPair.first.find(delimiter);
		std::string key = (pos != std::string::npos) ? resultPair.first.substr(0, pos) : resultPair.first;

		std::string displayName = (nameMap.find(key) != nameMap.end()) ? nameMap[key] : resultPair.first;

		// Set curve name based on simulation type
		switch (normalizedSimType[0]) {
		case 'D':
			curveName = displayName + "-DC";
			break;
		case 'T':
			curveName = displayName + "-TRAN";
			break;
		case 'A':
			curveName = displayName + "-AC";
			break;
		default:
			OT_LOG_E("Unexpected simulation type");
			continue;
		}

		// Create and populate quantity with y-values
		std::unique_ptr<QuantityDescriptionCurve> quantity(new QuantityDescriptionCurve());
		const std::vector<double>& yValues = resultPair.second;
		for (const double& value : yValues) {
			quantity->addDatapoint(ot::Variable(value));
		}

		// Create dataset and add parameter
		DatasetDescription dataset;
		dataset.addParameterDescription(sharedParameterDescr);

		// Configure curve appearance
		ot::Plot1DCurveCfg curveCfg;
		curveCfg.setTitle(curveName);
		auto stylePainter = rainbowPainterIt.getNextPainter();
		curveCfg.setLinePenPainter(stylePainter.release());

		std::string seriesName = solverNameShort + "_" + curveName;

		// Determine if voltage or current curve and configure accordingly
		bool isVoltage = (resultPair.first.find("V(") != std::string::npos ||
			resultPair.first.find("vd_") != std::string::npos);

		if (isVoltage) {
			curveCfg.setEntityName(fullPlotNameVoltage + "/" + curveName);
			quantity->setName("Voltage");
			quantity->defineQuantityAsSingle(ot::TypeNames::getDoubleTypeName(), "V");

			QuantityDescriptionCurve* quantityPtr = quantity.release();
			dataset.setQuantityDescription(quantityPtr);
			OT_LOG_D("  Adding voltage curve: " + curveName + " with " + std::to_string(yValues.size()) + " datapoints");
			plotBuilderVoltage.addCurve(std::move(dataset), curveCfg, seriesName, quantityPtr->getMetadataQuantity());
		}
		else {
			curveCfg.setEntityName(fullPlotNameCurrent + "/" + curveName);
			quantity->setName("Current");
			quantity->defineQuantityAsSingle(ot::TypeNames::getDoubleTypeName(), "A");

			QuantityDescriptionCurve* quantityPtr = quantity.release();
			dataset.setQuantityDescription(quantityPtr);
			OT_LOG_D("  Adding current curve: " + curveName + " with " + std::to_string(yValues.size()) + " datapoints");
			plotBuilderCurrent.addCurve(std::move(dataset), curveCfg, seriesName, quantityPtr->getMetadataQuantity());
		}
	}

	// Create voltage plot if curves exist
	if (plotBuilderVoltage.getNumberOfCurves() > 0) {
		OT_LOG_D("Creating voltage plot with " + std::to_string(plotBuilderVoltage.getNumberOfCurves()) + " curves at " + fullPlotNameVoltage);
		ot::Plot1DCfg plotCfg;
		plotCfg.setEntityName(fullPlotNameVoltage);
		plotCfg.setXAxisParameter(sharedParameterDescr->getMetadataParameter().parameterLabel);
		plotBuilderVoltage.buildPlot(plotCfg,false);
	}

	// Create current plot if curves exist
	if (plotBuilderCurrent.getNumberOfCurves() > 0) {
		OT_LOG_D("Creating current plot with " + std::to_string(plotBuilderCurrent.getNumberOfCurves()) + " curves at " + fullPlotNameCurrent);
		ot::Plot1DCfg plotCfg;
		plotCfg.setEntityName(fullPlotNameCurrent);
		plotCfg.setXAxisParameter(sharedParameterDescr->getMetadataParameter().parameterLabel);
		plotBuilderCurrent.buildPlot(plotCfg,false);
	}
	else {
		OT_LOG_W("No current curves to plot");
	}

	OT_LOG_D("=== createResultCurves END ===");
}

const std::string BlockEntityHandler::getInitialCircuitName() const {
	return this->m_initialCircuitName;
}
