#pragma once
// C++ Header
#include <vector>
#include <map>
#include <string>


// Service Header
#include "ConnectionManager.h"



class SimulationResults: public QObject {
    Q_OBJECT
public:

    static SimulationResults* getInstance();
    void addToResultMap(const std::string& key, double value);
    std::map<std::string, std::vector<double>>& getResultMap() { return resultMap; }

    //Setter
    void setVecAmount(int amount) {
        this->vecAmount = amount;
    }

    //Getter
    int getVecAmount() {
        return this->vecAmount;
    }

    void triggerCallback(std::string messageType, std::string message);

Q_SIGNALS:
    void callback(std::string messageType, std::string message);

private:

    SimulationResults() { this->vecAmount = 0; }
    static SimulationResults* instance;
    std::map<std::string, std::vector<double>> resultMap;
    int vecAmount;
   
};