cmake_minimum_required(VERSION 3.20)
project(PythonExecution LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_bin_python(PythonExecution OT_PYTHON_EXECUTION_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(PythonExecution 4996)
ot_add_compile_definitions(PythonExecution
    QT_NO_KEYWORDS
)
ot_set_subsystem(PythonExecution CONSOLE)

ot_add_dependency(PythonExecution
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTBlockEntities
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    PYTHON
    QtCore
    QtNetwork
    QtWidgets
)

ot_finalize_bin(PythonExecution)
ot_add_test(PythonExecution)
