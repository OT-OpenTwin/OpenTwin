#pragma once

#include <string>

class StudioConnector
{
public:
    static StudioConnector& getInstance()
    {
        static StudioConnector instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void openProject(const std::string &fileName);
    void saveProject();
    void extractInformation();

private:
    StudioConnector() {};
    ~StudioConnector() {};

};
