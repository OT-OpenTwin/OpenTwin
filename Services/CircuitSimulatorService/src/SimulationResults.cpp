#include "SimulationResults.h"
// Open Twin Header
#include "OTModelAPI/ModelServiceAPI.h"

// C++ Header
#include <regex>

SimulationResults* SimulationResults::instance = nullptr;

SimulationResults* SimulationResults::getInstance()
{
    
    if (!instance) {
        instance = new SimulationResults();
    }
    return instance;
    
}

void SimulationResults::addToResultMap(const std::string& key, double value)

{
    std::lock_guard<std::mutex> lock(m_mutex);
    resultMap[key].push_back(value);
}

void SimulationResults::addResults(const QJsonValue& _result) {
    
    

    QJsonArray resultsArray = _result.toArray();

    for (const QJsonValue& value : resultsArray) {
        if (value.isObject()) {
            QJsonObject obj = value.toObject();


            QString key = obj["key"].toString();
            QJsonArray valuesArray = obj["values"].toArray();


            std::vector<double> values;
            for (const QJsonValue& val : valuesArray) {
                values.push_back(val.toDouble());
            }


            resultMap[key.toStdString()] = values;
        }
    }
    
    m_blockEntityHandler.setModelComponent(this->getInstance()->_modelComponent);
    m_blockEntityHandler.createResultCurves(solverName, simulationType, circuitName);
    this->resultMap.clear();
}

void SimulationResults::setSolverInformation(std::string _solverName, std::string _simulationType, std::string _circuitName) {
    std::lock_guard<std::mutex> lock(m_mutex);


    this->solverName = _solverName;
    this->simulationType = _simulationType;
    this->circuitName = _circuitName;
    
}

void SimulationResults::displayMessage(std::string _message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<int> percentages = findPercentage(_message);

    if (!percentages.empty()) {
        for (auto value : percentages) {
            _uiComponent->setProgress(value);
        }
    }
    else {
        this->logData.append(_message + "\n");
        this->getInstance()->_uiComponent->displayMessage(_message);
        
    }
 
    
}

void SimulationResults::displayError(std::string _message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    ot::StyledTextBuilder errorMessage;
    errorMessage << "[" << ot::StyledText::Bold << ot::StyledText::Error << "Error" << ot::StyledText::ClearStyle << "] " << _message;
    this->logData.append(_message + "\n");
    this->getInstance()->_uiComponent->displayStyledMessage(errorMessage);
    
}

void SimulationResults::handleResults(const QJsonValue& _result) {
    std::lock_guard<std::mutex> lock(m_mutex);

    addResults(_result);
    OT_LOG_D("Added Results to Curves");
    
}

void SimulationResults::handleUnknownMessageType(std::string _message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    this->getInstance()->_uiComponent->displayErrorPrompt("Unknown Message Type: " + _message);
}

void SimulationResults::storeLogDataInResultText() {
    std::lock_guard<std::mutex> lock(m_mutex);
    EntityResultText* output = _modelComponent->addResultTextEntity(solverName + "/Output", logData);

    std::list<ot::UID> topologyEntityIDList;
    std::list<ot::UID> topologyEntityVersionList;
    std::list<bool> topologyEntityForceVisibleList;
    std::list<ot::UID> dataEntityIDList;
    std::list<ot::UID> dataEntityVersionList;
    std::list<ot::UID> dataEntityParentList;

    topologyEntityIDList.push_back(output->getEntityID());
    topologyEntityVersionList.push_back(output->getEntityStorageVersion());
    topologyEntityForceVisibleList.push_back(false);

    dataEntityIDList.push_back(output->getTextDataStorageId());
    dataEntityVersionList.push_back(output->getTextDataStorageVersion());
    dataEntityParentList.push_back(output->getEntityID());

    
    ot::ModelServiceAPI::addEntitiesToModel(topologyEntityIDList, topologyEntityVersionList, topologyEntityForceVisibleList, dataEntityIDList, dataEntityVersionList, dataEntityParentList, "added Circuit Simulation results");
}

void SimulationResults::handleCircuitExecutionTiming(const QDateTime& _timePoint, std::string timeType) {
    std::lock_guard<std::mutex> lock(m_mutex);
    ot::StyledTextBuilder timeMessage;

    if(timeType == "startingInitTime") {
        
        timeMessage << "[" << ot::StyledText::Bold << ot::StyledText::Highlight << "Starting Initialization Time of CircuitExecution: " << ot::StyledText::ClearStyle
            << _timePoint.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();  
    } else if (timeType == "finishedInitTime") {
        timeMessage << "[" << ot::StyledText::Bold << ot::StyledText::Highlight << "Finished Initialization Time of CircuitExecution: " << ot::StyledText::ClearStyle
            << _timePoint.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    }
    else if (timeType == "finishedCircuitExecutionTime") {
        timeMessage << "[" << ot::StyledText::Bold << ot::StyledText::Highlight << "Finished Time of CircuitExecution: " << ot::StyledText::ClearStyle
            << _timePoint.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    }
    else {
        timeMessage << "[" << ot::StyledText::Bold << ot::StyledText::Highlight << "Unidentified Time: " << ot::StyledText::ClearStyle
            << _timePoint.toString("yyyy-MM-dd HH:mm:ss.zzz").toStdString();
    }
    

    this->getInstance()->_uiComponent->displayStyledMessage(timeMessage);
}

void SimulationResults::clearUp() {
    this->logData.clear();
    this->resultMap.clear();
}

std::vector<int> SimulationResults::findPercentage(const std::string& input) {
    
    std::vector<int> values;
    std::regex prozentRegex(R"((\d+(\.\d+)?)%)"); // z.B. 10% oder 10.0%
    std::smatch match;

    auto start = input.cbegin();
    while (std::regex_search(start, input.cend(), match, prozentRegex)) {
        int wert = static_cast<int>(std::stod(match[1]));
        values.push_back(wert);
        start = match.suffix().first; // next search area
    }

    return values;
}


