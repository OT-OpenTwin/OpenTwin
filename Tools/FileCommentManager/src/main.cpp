// Project header
#include "Logger.h"
#include "AppBase.h"

// Qt header
#include <QtCore/qcoreapplication.h>

int main(int _argc, char* _argv[]) {
    int argc = 0;
    QCoreApplication a(argc, nullptr);

    // Parse args
    for (int i = 0; i < _argc; i++) {
        if (std::string(_argv[i]) == "--silent") {
            Logger::setSilent(true);
        }
    }

    // Create app
    AppBase app;

    return a.exec();
}
