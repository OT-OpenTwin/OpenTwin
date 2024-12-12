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

void SimulationResults::addResults(QString _result) {
    std::lock_guard<std::mutex> lock(m_mutex);

    // JSON-Daten in ein QJsonDocument laden
    QJsonDocument doc = QJsonDocument::fromJson(_result.toUtf8());

    QJsonObject jsonObj = doc.object();

    QJsonArray resultsArray = jsonObj["results"].toArray();

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

