cmake_minimum_required(VERSION 3.20)
project(PythonExecutionService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(PythonExecutionService OT_PYTHON_EXECUTION_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(PythonExecutionService 4996)

ot_add_dependency(PythonExecutionService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    QtCore
    QtNetwork
)

ot_finalize_lib(PythonExecutionService)
ot_add_test(PythonExecutionService)
