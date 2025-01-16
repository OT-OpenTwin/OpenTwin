#pragma once
//Service Header
#include "BlockEntityHandler.h"


//Qt Header
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "QtCore/qjsonobject.h"

//Open Twin Header
#include "OTServiceFoundation/BusinessLogicHandler.h"

//C++ Header
#include <vector>
#include <map>
#include <string>
#include <mutex>

class SimulationResults : public BusinessLogicHandler
{

public:
    
    static SimulationResults* getInstance();
    void addToResultMap(const std::string& key, double value);
    std::map<std::string, std::vector<double>>& getResultMap() {   return resultMap; }

    void addResults(const QJsonValue& _result);
    void setSolverInformation(std::string solverName, std::string simulationType, std::string circuitName);

    //Message handling
    void displayMessage(std::string _message);
    void displayError(std::string _message);
    void handleResults(const QJsonValue& _result);
    void handleUnknownMessageType(std::string _message);
    //Setter
    void setVecAmount(int amount)
    {
        this->vecAmount = amount;
    }

    //Getter
    int getVecAmount()
    {
        return this->vecAmount;
    }
private:

    SimulationResults() { this->vecAmount = 0; }
    static SimulationResults* instance;
    std::map<std::string, std::vector<double>> resultMap;
    int vecAmount;
    BlockEntityHandler m_blockEntityHandler;
    std::string solverName;
    std::string simulationType;
    std::string circuitName;
    std::mutex m_mutex;


};


