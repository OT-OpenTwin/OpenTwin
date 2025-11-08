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
#include "OTCommunication/ActionTypes.h"
#include "CircuitElement.h"
#include "Connection.h"
#include "EntityAPI.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTGui/StyleRefPainter2D.h"

//#include "ExternalDependencies.h"
#include "Application.h"
#include "EntityBlockCircuitVoltageSource.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockConnection.h"
#include "SimulationResults.h"
#include "EntityBlockCircuitDiode.h"
#include "EntityBlockCircuitVoltageMeter.h"
#include "EntityBlockCircuitCurrentMeter.h"
#include "EntityBlockCircuitCapacitor.h"
#include "EntityBlockCircuitInductor.h"
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitGND.h"
#include "EntityBlockCircuitTransmissionLine.h"

#include "PlotBuilder.h"
#include "ResultCollectionExtender.h"
#include "QuantityDescriptionCurve.h"
#include "OTGui/PainterRainbowIterator.h"
#include "OTCore/EntityName.h"

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

std::map<ot::UID, std::shared_ptr<EntityBlock>> BlockEntityHandler::findAllBlockEntitiesByBlockID(const std::string& _folderName) {
	std::list<std::string> blockItemNames = ot::ModelServiceAPI::getListOfFolderItems(_folderName, true);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(blockItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);
	
	std::map<ot::UID, std::shared_ptr<EntityBlock>> blockEntitiesByBlockID;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() != "EntityBlockConnection") { //Otherwise not a BlockEntity, since ClassFactoryBlock does not handle others 
			std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));
			if (blockEntity != nullptr) {
				blockEntitiesByBlockID[blockEntity->getEntityID()] = blockEntity;
			}
		}

	}
	return blockEntitiesByBlockID;
}

std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> BlockEntityHandler::findAllEntityBlockConnections(const std::string& _folderName) {
	const std::string fullFolderName = _folderName + "/" + m_connectionsFolder;
	std::list<std::string> connectionItemNames = ot::ModelServiceAPI::getListOfFolderItems(fullFolderName);
	std::list<ot::EntityInformation> entityInfos;
	ot::ModelServiceAPI::getEntityInformation(connectionItemNames, entityInfos);
	Application::instance()->prefetchDocumentsFromStorage(entityInfos);

	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> entityBlockConnectionsByBlockID;
	EntityBlockConnection temp;
	for (auto& entityInfo : entityInfos) {
		auto baseEntity = ot::EntityAPI::readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion());
		if (baseEntity != nullptr && baseEntity->getClassName() == temp.getClassName()) {
			std::shared_ptr<EntityBlockConnection> blockEntityConnection(dynamic_cast<EntityBlockConnection*>(baseEntity));
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

void BlockEntityHandler::createResultCurves(std::string solverName,std::string simulationType,std::string circuitName) 
{
		
	std::map<std::string, std::vector<double>> resultVectors = SimulationResults::getInstance()->getResultMap();
	ResultCollectionExtender extender(Application::instance());

	PlotBuilder plotBuilderCurrent(extender);
	PlotBuilder plotBuilderVoltage(extender);

	//First we parse the result values, depending on the simulation type 
	std::vector<double> xValues;	
	if (simulationType == ".dc") {
		auto it = resultVectors.find("v-sweep");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("v-sweep");
			resultVectors.erase(it);
		}
		else
		{
			OT_LOG_E("No v-sweep vector found");
			//ngSpice_Command(const_cast<char*>("quit"));
			return;
		}

		simulationType = "DC";
	}
	else if (simulationType == ".TRAN") {
		auto it = resultVectors.find("time");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("time");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No time vector found");
			//ngSpice_Command(const_cast<char*>("quit"));
			return;
		}
			
		simulationType = "TRAN";
	}
	else {
		auto it = resultVectors.find("frequency");
		if (it != resultVectors.end()) {
			xValues = resultVectors.at("frequency");
			resultVectors.erase(it);
		}
		else {
			OT_LOG_E("No frequency vector found");
			//ngSpice_Command(const_cast<char*>("quit"));
			return;
		}

		simulationType = "AC";
	}
	
	MetadataParameter parameter;
	for (double& value : xValues)
	{
		parameter.values.push_back(ot::Variable(value));
	}
	xValues.clear();

	// Now i try to find the branch and erase it too
	auto it = resultVectors.find("v1#branch");
	if (it != resultVectors.end()) {
		resultVectors.erase(it);
	}
	else {
		OT_LOG_E("No v1#branch vector found");
	}


	// Here i want to delete the ediff vector from my Result beacuse i dont need it
	while (true) {
			
		auto it2 = std::find_if(resultVectors.begin(), resultVectors.end(),
			[&](const std::pair<const std::string, std::vector<double>>& element) {
				return element.first.find("ediff") != std::string::npos;
			});
		if (it2 != resultVectors.end()) {
				
			resultVectors.erase(it2);
		}
		else {
				
			break;
		}
	}

	std::string _curveFolderPath = solverName + "/" + "Results" + "/" + "1D/Curves";

	// Here i create the plot for all the curves of voltage
	const std::string _plotNameVoltage = "/" + simulationType + "-Voltage";
	const std::string plotFolderVoltage = solverName + "/" + "Results";
	const std::string fullPlotNameVoltage = plotFolderVoltage + _plotNameVoltage;

	// Here i create the plit for all the curves of current
	const std::string _plotNameCurrent = "/" + simulationType + "-Current";
	const std::string plotFolderCurrent = solverName + "/" + "Results";
	const std::string fullPlotNameCurrent = plotFolderCurrent + _plotNameCurrent;

	ot::PainterRainbowIterator rainbowPainterIt;
	// No i want to get the node vectors of voltage and for each of them i create a curve
	for (auto& it : resultVectors) {
		std::string curveName;
		std::string xLabel;
		std::string xUnit;
		std::string yUnit;
		auto& map = Application::instance()->getNGSpice().netlistNameToCustomNameMap;

		std::string name;
		std::string delimiter = "#branch";
		std::size_t pos = it.first.find(delimiter);

		std::string key = (pos != std::string::npos) ? it.first.substr(0, pos) : it.first;
		if (map.find(key) != map.end()) {
			name = map[key];
		}
		else {
			name = it.first; 
		}

		if (simulationType == "DC") {
			curveName = name + "-DC";
			xLabel = "sweep";
			xUnit = "V";
			yUnit = "V";

				
		}
		else if (simulationType == "TRAN") {
			curveName = name + "-TRAN";
			xLabel = "time";
			xUnit = "ms";
			yUnit = "V";
		}
		else {
			curveName = name + "-AC";
			xLabel = "frequency";
			xUnit = "hz";
			yUnit = "V";
		}

		parameter.unit = xUnit;
		parameter.parameterName = xLabel;
		parameter.typeName = ot::TypeNames::getDoubleTypeName();
		std::shared_ptr<ParameterDescription> parameterDescr(new ParameterDescription(parameter, false));
		

		std::unique_ptr<QuantityDescriptionCurve> quantity(new QuantityDescriptionCurve());
		const std::vector<double>& yValues = it.second;
		for (const double& value : yValues)
		{
			quantity->addDatapoint(ot::Variable(value));
		}
		it.second.clear();

		DatasetDescription dataset;
		dataset.addParameterDescription(parameterDescr);

		ot::Plot1DCurveCfg curveCfg;
		curveCfg.setTitle(curveName);

		
			
		auto stylePainter = rainbowPainterIt.getNextPainter();
		curveCfg.setLinePenPainter(stylePainter.release());

		std::string yLabel = it.first;
		if (yLabel.find("V(") != std::string::npos || yLabel.find("vd_") != std::string::npos)
		{
			curveCfg.setEntityName(fullPlotNameVoltage + "/" + curveName);
			yLabel = "Voltage";
			quantity->setName(yLabel);
			quantity->addValueDescription("", ot::TypeNames::getDoubleTypeName(), yUnit);
			dataset.setQuantityDescription(quantity.release());
			plotBuilderVoltage.addCurve(std::move(dataset), curveCfg, curveName);				
		}
		else
		{
			curveCfg.setEntityName(fullPlotNameCurrent + "/" + curveName);
			yLabel = "Current";
			yUnit = "I";
			quantity->setName(yLabel);
			quantity->addValueDescription("", ot::TypeNames::getDoubleTypeName(), yUnit);
			dataset.setQuantityDescription(quantity.release());

			plotBuilderCurrent.addCurve(std::move(dataset), curveCfg, curveName);
		}
	}
	
	

	// Creating the Plot Entity for Voltage
	if (plotBuilderVoltage.getNumberOfCurves() != 0)
	{
		ot::Plot1DCfg plotCfg;
		plotCfg.setEntityName(fullPlotNameVoltage);
		plotBuilderVoltage.buildPlot(plotCfg, false);			
	}
	// Creating the Plot Entity for Current
	if (plotBuilderCurrent.getNumberOfCurves() != 0)
	{
		ot::Plot1DCfg plotCfg;
		plotCfg.setEntityName(fullPlotNameCurrent);
		plotBuilderCurrent.buildPlot(plotCfg, false);
	}
}

const std::string BlockEntityHandler::getInitialCircuitName() const {
	return this->m_initialCircuitName;
}
