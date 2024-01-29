#pragma once

#include <string>

#include "OTCore/JSON.h"

#include <qobject.h>

class ProgressInfo
{
public:
    static ProgressInfo& getInstance()
    {
        static ProgressInfo instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void setMainObject(QObject* main) { mainObject = main; }

    void setProgressState(bool visible, const std::string& message, bool continuous);
    void setProgressValue(int percentage);

private:
    ProgressInfo() : mainObject(nullptr) {};
    ~ProgressInfo() {};

    QObject* mainObject;
};

class ProgressInfoHelper
{
public:
    ProgressInfoHelper() {};
    ~ProgressInfoHelper() { ProgressInfo::getInstance().setProgressState(false, "", false); };

    ProgressInfo& getProgressInfo() { return ProgressInfo::getInstance(); }
};
