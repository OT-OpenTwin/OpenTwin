cmake_minimum_required(VERSION 3.20)
project(OTFMC LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTFMC OT_FILE_MANAGER_CONNECTOR_ROOT_PATH)
ot_add_export(OTFMC OT_FMC_EXPORTS)

ot_add_dependency(OTFMC
    OTSystem
    OTCore
    OTGui
    OTWidgets
    OTCommunication
    OTFrontendConnectorAPI
    OTDataStorage
    OTModelEntities
    RJSON
    ZLIB
    MONGO_CXX
    QtWidgets
    QtNetwork
    WINLIB:userenv
    WINLIB:wsock32
    WINLIB:advapi32
)

ot_finalize_lib(OTFMC)
ot_add_test(OTFMC)
