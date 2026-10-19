cmake_minimum_required(VERSION 3.20)
project(LibraryManagementService LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(LibraryManagementService OT_LIBRARY_MANAGEMENT_SERVICE_ROOT_PATH LIBRARYMANAGEMENT_EXPORTS)
ot_disable_warnings(LibraryManagementService 4996)

ot_add_dependency(LibraryManagementService
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
    QtWidgets
)

ot_service_debug_launch(LibraryManagementService
    ARGS "@OPEN_TWIN_LOGGING_URL@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_LMS_PORT@"
         "@OPEN_TWIN_SERVICES_ADDRESS@:@OPEN_TWIN_GSS_PORT@"
         "@OPEN_TWIN_MONGODB_PWD@"
)

ot_finalize_lib(LibraryManagementService)
ot_add_test(LibraryManagementService)
