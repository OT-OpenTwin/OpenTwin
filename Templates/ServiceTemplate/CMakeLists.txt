cmake_minimum_required(VERSION 3.20)
project(ServiceTemplate LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(ServiceTemplate OT_SERVICE_TEMPLATE_ROOT_PATH SESSIONSERVICE_EXPORTS)

ot_add_dependency(ServiceTemplate
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
    BOOST
)

ot_finalize_lib(ServiceTemplate)
ot_add_test(ServiceTemplate)
