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

ot_finalize_lib(LibraryManagementService)
ot_add_test(LibraryManagementService)
