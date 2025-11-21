# ============================================================
# DataStorage test CMake
# Reuses DataStorage_core object files and links against
# Mongo, Zlib, cURL, FTP client, OpenTwinCommunication, etc.
# ============================================================

# ------------------------------------------------------------
# Helper functions
# ------------------------------------------------------------
function(get_env_path OUT_VAR ENV_NAME)
    if(DEFINED ENV{${ENV_NAME}} AND NOT "$ENV{${ENV_NAME}}" STREQUAL "")
        set(_val "$ENV{${ENV_NAME}}")
        string(REPLACE "\"" "" _val "${_val}")   # strip quotes
        file(TO_CMAKE_PATH "${_val}" _val)       # normalize slashes
        set(${OUT_VAR} "${_val}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

function(join_paths OUT_VAR BASE SUBDIR)
    if(NOT "${BASE}" STREQUAL "" AND NOT "${SUBDIR}" STREQUAL "")
        set(_p "${BASE}/${SUBDIR}")
        file(TO_CMAKE_PATH "${_p}" _p)
        set(${OUT_VAR} "${_p}" PARENT_SCOPE)
    else()
        set(${OUT_VAR} "" PARENT_SCOPE)
    endif()
endfunction()

# ------------------------------------------------------------
# Environment: Zlib / cURL / OTSystem / Mongo / FTP / RapidJSON
# ------------------------------------------------------------

# Zlib
get_env_path(ZLIB_LIBPATHD_PATH     ZLIB_LIBPATHD)
get_env_path(ZLIB_LIBPATHR_PATH     ZLIB_LIBPATHR)
get_env_path(ZLIB_ROOT_PATH         ZLIB_ROOT)

# cURL / FTP / OpenTwinCommunication
get_env_path(CURL_LIBPATHD_PATH     CURL_LIBPATHD)
get_env_path(CURL_LIBPATHR_PATH     CURL_LIBPATHR)
get_env_path(CURL_ROOT_PATH         CURL_ROOT)

get_env_path(FTP_LIBCURL_ROOT_PATH  FTP_LIBCURL_ROOT)
get_env_path(FTP_CLIENT_ROOT_PATH   FTP_CLIENT_ROOT)

# OTSystem root and include subdir
get_env_path(OT_SYSTEM_ROOT_PATH   		OT_SYSTEM_ROOT)
get_env_path(OT_CORE_ROOT_PATH     		OT_CORE_ROOT)
get_env_path(OT_COMMUNICATION_ROOT_PATH OT_COMMUNICATION_ROOT)
get_env_path(OT_INC_PATH           		OT_INC)

# RapidJSON
get_env_path(R_JSON_INCD_PATH       R_JSON_INCD)
get_env_path(R_JSON_INCR_PATH       R_JSON_INCR)

# OTSystem library subdirs
get_env_path(OT_CLIBD_PATH          OT_CLIBD)
get_env_path(OT_CLIBR_PATH          OT_CLIBR)

# Mongo C / C++ include and libraries
get_env_path(MONGO_CXX_INC_PATH     MONGO_CXX_INC)
get_env_path(MONGO_C_INC_PATH       MONGO_C_INC)

get_env_path(MONGO_BOOST_INCD_PATH  MONGO_BOOST_INCD)
get_env_path(MONGO_BOOST_INCR_PATH  MONGO_BOOST_INCR)

get_env_path(MONGO_CXX_LIBPATHD_PATH MONGO_CXX_LIBPATHD)
get_env_path(MONGO_CXX_LIBPATHR_PATH MONGO_CXX_LIBPATHR)
get_env_path(MONGO_C_LIBPATHD_PATH   MONGO_C_LIBPATHD)
get_env_path(MONGO_C_LIBPATHR_PATH   MONGO_C_LIBPATHR)

get_env_libname(MONGO_CXX_LIBS_NAME MONGO_CXX_LIB)  # "bsoncxx.lib;mongocxx.lib"
get_env_libname(MONGO_C_LIBS_NAME   MONGO_C_LIB)    # "bson-1.0.lib;mongoc-1.0.lib"

# Build concrete OTSystem library directories
join_paths(OT_SYSTEM_DEBUG_LIB_DIR   		"${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBD_PATH}")
join_paths(OT_SYSTEM_RELEASE_LIB_DIR 		"${OT_SYSTEM_ROOT_PATH}" "${OT_CLIBR_PATH}")
join_paths(OT_CORE_DEBUG_LIB_DIR     		"${OT_CORE_ROOT_PATH}"   "${OT_CLIBD_PATH}")
join_paths(OT_CORE_RELEASE_LIB_DIR   		"${OT_CORE_ROOT_PATH}"   "${OT_CLIBR_PATH}")
join_paths(OT_COMMUNICATION_DEBUG_LIB_DIR   "${OT_COMMUNICATION_ROOT_PATH}"   "${OT_LIBD_PATH}")
join_paths(OT_COMMUNICATION_RELEASE_LIB_DIR "${OT_COMMUNICATION_ROOT_PATH}"   "${OT_LIBR_PATH}")
# ------------------------------------------------------------
# Collect all test sources from tests/src/
# ------------------------------------------------------------
file(GLOB_RECURSE DataStorage_TEST_SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# ------------------------------------------------------------
# Test executable
# Reuse compiled object files from DataStorage_core
# ------------------------------------------------------------
add_executable(DataStorage_tests
    ${DataStorage_TEST_SOURCES}
    $<TARGET_OBJECTS:DataStorage_core>
)

# Same export macro as the DLL, so symbols are visible
target_compile_definitions(DataStorage_tests
    PRIVATE
        DATASTORAGE_EXPORTS
)

# ------------------------------------------------------------
# Include directories for tests
# ------------------------------------------------------------
target_include_directories(DataStorage_tests
    PRIVATE
        # Local test headers
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        # Public DataStorage headers
        "${CMAKE_CURRENT_SOURCE_DIR}/../include"
)

# OTSystem include root
if(OT_SYSTEM_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(DataStorage_tests PRIVATE
        "${OT_SYSTEM_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# OTCore include root (needed for #include <OTCore/Variable.h> etc.)
if(OT_CORE_ROOT_PATH AND OT_INC_PATH)
    target_include_directories(DataStorage_tests PRIVATE
        "${OT_CORE_ROOT_PATH}/${OT_INC_PATH}"
    )
endif()

# RapidJSON includes
if(R_JSON_INCD_PATH)
    target_include_directories(DataStorage_tests PRIVATE "${R_JSON_INCD_PATH}")
endif()

if(R_JSON_INCR_PATH AND NOT R_JSON_INCR_PATH STREQUAL R_JSON_INCD_PATH)
    target_include_directories(DataStorage_tests PRIVATE "${R_JSON_INCR_PATH}")
endif()

# Mongo C++ / C / Boost includes
if(MONGO_CXX_INC_PATH)
    target_include_directories(DataStorage_tests PRIVATE "${MONGO_CXX_INC_PATH}")
endif()

if(MONGO_C_INC_PATH)
    target_include_directories(DataStorage_tests PRIVATE "${MONGO_C_INC_PATH}")
endif()

if(MONGO_BOOST_INCD_PATH)
    target_include_directories(DataStorage_tests PRIVATE "${MONGO_BOOST_INCD_PATH}")
endif()

if(MONGO_BOOST_INCR_PATH AND NOT MONGO_BOOST_INCR_PATH STREQUAL MONGO_BOOST_INCD_PATH)
    target_include_directories(DataStorage_tests PRIVATE "${MONGO_BOOST_INCR_PATH}")
endif()

# ------------------------------------------------------------
# Windows-specific linking (Zlib, cURL, OTSystem, Mongo, FTP, etc.)
# ------------------------------------------------------------
if(WIN32)

    # --- OTSystem library directories ---
    if(OT_SYSTEM_DEBUG_LIB_DIR)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Debug>:${OT_SYSTEM_DEBUG_LIB_DIR}>
        )
    endif()

    if(OT_SYSTEM_RELEASE_LIB_DIR)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Release>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:RelWithDebInfo>:${OT_SYSTEM_RELEASE_LIB_DIR}>
            $<$<CONFIG:MinSizeRel>:${OT_SYSTEM_RELEASE_LIB_DIR}>
        )
    endif()

	# --- OTCore library directories ---
	if(OT_CORE_DEBUG_LIB_DIR)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
		)
	endif()

	if(OT_CORE_RELEASE_LIB_DIR)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
			$<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
			$<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
		)
	endif()
	
	# --- OTCore library directories ---
	if(OT_CORE_DEBUG_LIB_DIR)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Debug>:${OT_CORE_DEBUG_LIB_DIR}>
		)
	endif()

	if(OT_CORE_RELEASE_LIB_DIR)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Release>:${OT_CORE_RELEASE_LIB_DIR}>
			$<$<CONFIG:RelWithDebInfo>:${OT_CORE_RELEASE_LIB_DIR}>
			$<$<CONFIG:MinSizeRel>:${OT_CORE_RELEASE_LIB_DIR}>
		)
	endif()
	
	# --- OpenTwinCommunication (SIM_PLAT_ROOT\Libraries\OpenTwinCommunication\x64\$(Configuration)) ---
	if(OT_COMMUNICATION_DEBUG_LIB_DIR)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Debug>:${OT_COMMUNICATION_DEBUG_LIB_DIR}>
		)
	endif()

	if(OT_COMMUNICATION_RELEASE_LIB_DIR)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Release>:${OT_COMMUNICATION_RELEASE_LIB_DIR}>
			$<$<CONFIG:RelWithDebInfo>:${OT_COMMUNICATION_RELEASE_LIB_DIR}>
			$<$<CONFIG:MinSizeRel>:${OT_COMMUNICATION_RELEASE_LIB_DIR}>
		)
	endif()

    # --- Zlib library directories ---

    # Variant 1: ZLIB_LIBPATHD / ZLIB_LIBPATHR
    if(ZLIB_LIBPATHD_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_LIBPATHD_PATH}>
        )
    endif()

    if(ZLIB_LIBPATHR_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Release>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${ZLIB_LIBPATHR_PATH}>
        )
    endif()

    # Variant 2: $(ZLIB_ROOT)\$(Configuration)\lib
    if(ZLIB_ROOT_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Debug>:${ZLIB_ROOT_PATH}/Debug/lib>
            $<$<CONFIG:RelWithDebInfo>:${ZLIB_ROOT_PATH}/Release/lib>
            $<$<CONFIG:MinSizeRel>:${ZLIB_ROOT_PATH}/Release/lib>
            $<$<CONFIG:Release>:${ZLIB_ROOT_PATH}/Release/lib>
        )
    endif()

    # --- cURL library directory from CURL_LIB_PATH (optional) ---
    if(CURL_LIB_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            ${CURL_LIB_PATH}
        )
    endif()

	# --- FTP / libcurl (mirrors DebugTest/ReleaseTest vcxproj) ---
	if(CURL_ROOT_PATH)
		target_link_directories(DataStorage_tests PRIVATE
			$<$<CONFIG:Debug>:${CURL_LIBPATHD_PATH}>
			$<$<CONFIG:RelWithDebInfo>:${CURL_LIBPATHR_PATH}>
			$<$<CONFIG:MinSizeRel>:${CURL_LIBPATHR_PATH}>
			$<$<CONFIG:Release>:${CURL_LIBPATHR_PATH}>
		)
	endif()

	if(FTP_CLIENT_ROOT_PATH)
		# Try several possible locations where ftpclient.lib might live
		target_link_directories(DataStorage_tests PRIVATE
			# Exact path as in the original .vcxproj:
			$<$<CONFIG:Debug>:${FTP_CLIENT_ROOT_PATH}/Build/lib/Debug>
			$<$<CONFIG:RelWithDebInfo>:${FTP_CLIENT_ROOT_PATH}/Build/lib/Release>
			$<$<CONFIG:MinSizeRel>:${FTP_CLIENT_ROOT_PATH}/Build/lib/Release>
			$<$<CONFIG:Release>:${FTP_CLIENT_ROOT_PATH}/Build/lib/Release>

			# Additional generic fallbacks (in case your layout slightly differs)
			${FTP_CLIENT_ROOT_PATH}
			${FTP_CLIENT_ROOT_PATH}/Build/lib
		)
	endif()

    # --- Mongo C++ / C library directories from your *_LIBPATH* vars ---
    if(MONGO_CXX_LIBPATHD_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_CXX_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_CXX_LIBPATHR_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Release>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_CXX_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_CXX_LIBPATHR_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHD_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Debug>:${MONGO_C_LIBPATHD_PATH}>
        )
    endif()

    if(MONGO_C_LIBPATHR_PATH)
        target_link_directories(DataStorage_tests PRIVATE
            $<$<CONFIG:Release>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:RelWithDebInfo>:${MONGO_C_LIBPATHR_PATH}>
            $<$<CONFIG:MinSizeRel>:${MONGO_C_LIBPATHR_PATH}>
        )
    endif()

    # --- Link libraries ---
    # Windows system libraries
    target_link_libraries(DataStorage_tests PRIVATE
        Userenv
        Bcrypt
    )

    # OTSystem and OTCore
    target_link_libraries(DataStorage_tests PRIVATE
        OTSystem
        OTCore
    )

    # Mongo C++ / C (from your env vars)
    target_link_libraries(DataStorage_tests PRIVATE
        ${MONGO_CXX_LIBS_NAME}   # "bsoncxx.lib;mongocxx.lib"
        ${MONGO_C_LIBS_NAME}     # "bson-1.0.lib;mongoc-1.0.lib"
    )

    # FTP client + libcurl + zlib + OpenTwinCommunication
    target_link_libraries(DataStorage_tests PRIVATE
        OTCommunication
        $<$<CONFIG:Debug>:zlibd;libcurl_debug>
        $<$<CONFIG:RelWithDebInfo>:zlib;libcurl>
        $<$<CONFIG:MinSizeRel>:zlib;libcurl>
        $<$<CONFIG:Release>:zlib;libcurl>
    )

endif()

# ------------------------------------------------------------
# GoogleTest integration
# GTest::gtest_main is provided by the parent CMake (DataStorage).
# ------------------------------------------------------------
target_link_libraries(DataStorage_tests
    PRIVATE
        GTest::gtest_main
)

include(GoogleTest)

add_test(NAME DataStorage_tests COMMAND DataStorage_tests)

# Use the executable directory as working directory (DLL lookup etc.)
set_tests_properties(DataStorage_tests PROPERTIES
    WORKING_DIRECTORY $<TARGET_FILE_DIR:DataStorage_tests>
)
