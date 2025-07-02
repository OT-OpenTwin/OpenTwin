cmake_minimum_required(VERSION 3.15)
project(OTSystem LANGUAGES CXX)

# Projekt-Optionen
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Header-Dateien
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Präprozessor-Definitionen
add_definitions(
    -DOPENTWINSYSTEM_EXPORTS
    -D_USRDLL
    -D_WINDOWS
)

# Quell- und Header-Dateien
set(SOURCES
    src/AppExitCodes.cpp
    src/DateTime.cpp
    src/FileSystem.cpp
    src/SystemProcess.cpp
    src/dllmain.cpp
    src/Network.cpp
    src/OperatingSystem.cpp
    src/PortManager.cpp
    src/SystemInformation.cpp
    src/UrlEncoding.cpp
)

# Optional: Header Files einfügen (nur zur IDE-Anzeige)
file(GLOB_RECURSE HEADERS "include/OTSystem/*.h")

# Erzeuge eine DLL (da _USRDLL definiert ist)
add_library(OTSystem SHARED ${SOURCES} ${HEADERS})

target_compile_definitions(OTSystem PRIVATE
    UNICODE
    _UNICODE
    OPENTWINSYSTEM_EXPORTS
    _WINDOWS
)

# Link Libraries
target_link_libraries(OTSystem
    Advapi32
    Userenv
    pdh
    wsock32
)

