cmake_minimum_required(VERSION 3.20)
project(PyritService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(PyritService OT_PYRIT_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(PyritService 4996)

ot_add_dependency(PyritService
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

ot_finalize_lib(PyritService)
ot_add_test(PyritService)
