cmake_minimum_required(VERSION 3.20)
project(OTViewer LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTViewer OT_VIEWER_ROOT_PATH)
ot_add_export(OTViewer)

ot_add_dependency(OTViewer
    OTSystem
    OTCore
    OTGui
    OTWidgets
    OTDataStorage
    OTRubberband
    OTRubberbandOSG
    OTCommunication
    OTServiceFoundation
    OTModelEntities

    QtFull
    ADS
    OSG
    QWT

    RJSON
    MONGO_C
    MONGO_CXX
    EARCUT

    WINLIB:userenv
    WINLIB:ws2_32
    WINLIB:advapi32
    WINLIB:shell32
    WINLIB:bcrypt
)

ot_finalize_lib(OTViewer)
ot_add_test(OTViewer)
