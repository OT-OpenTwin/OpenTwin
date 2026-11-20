cmake_minimum_required(VERSION 3.20)
project(GatewayService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(GatewayService OT_GATEWAY_SERVICE_ROOT_PATH GATEWAYSERVICE_EXPORTS)

ot_add_dependency(GatewayService
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

ot_finalize_lib(GatewayService)
ot_deploy_app_configuration(GatewayService)
ot_add_test(GatewayService)