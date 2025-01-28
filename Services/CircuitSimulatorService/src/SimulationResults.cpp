#include "SimulationResults.h"

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
    this->getInstance()->_uiComponent->displayMessage(_message);
}

void SimulationResults::displayError(std::string _message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    ot::StyledTextBuilder errorMessage;
    errorMessage << "[" << ot::StyledText::Bold << ot::StyledText::Error << "Error" << ot::StyledText::ClearStyle << "] " << _message;
    this->getInstance()->_uiComponent->displayStyledMessage(errorMessage);
}

void SimulationResults::handleResults(const QJsonValue& _result) {
    std::lock_guard<std::mutex> lock(m_mutex);

    addResults(_result);
}

void SimulationResults::handleUnknownMessageType(std::string _message) {
    std::lock_guard<std::mutex> lock(m_mutex);
    this->getInstance()->_uiComponent->displayErrorPrompt("Unknown Message Type: " + _message);
}


