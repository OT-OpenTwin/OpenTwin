cmake_minimum_required(VERSION 3.20)
project(OpenEMSService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OpenEMSService OT_OPENEMS_SERVICE_ROOT_PATH SESSIONSERVICE_EXPORTS)
ot_disable_warnings(OpenEMSService 4996)

ot_add_dependency(OpenEMSService
    OTSystem
    OTCore
    OTGui
    OTCommunication
    OTGuiAPI
    OTServiceFoundation
    OTModelEntities
    OTDataStorage
    OTModelAPI
    OTResultDataAccess
    RJSON
    CURL
    MONGO_C
    MONGO_CXX
    MONGO_BOOST
    QtCore
    QtNetwork
)

ot_finalize_lib(OpenEMSService)
ot_add_test(OpenEMSService)
