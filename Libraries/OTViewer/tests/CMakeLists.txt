cmake_minimum_required(VERSION 3.20)

add_executable(OTViewer_tests
    "${CMAKE_CURRENT_SOURCE_DIR}/src/test.cpp"
)

target_compile_definitions(OTViewer_tests PRIVATE
  $<$<CONFIG:Debug>:_DEBUG>
  $<$<NOT:$<CONFIG:Debug>>:NDEBUG>
  _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
)

if(MSVC)
  target_compile_options(OTViewer_tests PRIVATE /W3)
  set_property(TARGET OTViewer_tests PROPERTY
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
  )
endif()

if(DEFINED ENV{OT_INC} AND NOT "$ENV{OT_INC}" STREQUAL "")
  set(_ot_inc "$ENV{OT_INC}")
else()
  set(_ot_inc "include")
endif()

target_include_directories(OTViewer_tests PRIVATE
  "${CMAKE_CURRENT_SOURCE_DIR}/${_ot_inc}"
  "${CMAKE_CURRENT_LIST_DIR}/../${_ot_inc}"
)

if(DEFINED ENV{GOOGLE_TEST_INC} AND NOT "$ENV{GOOGLE_TEST_INC}" STREQUAL "")
  target_include_directories(OTViewer_tests PRIVATE "$ENV{GOOGLE_TEST_INC}")
endif()

if(OT_LIBTESTD_PATH AND OT_LIBTESTR_PATH)
  target_link_directories(OTViewer_tests PRIVATE
    $<$<CONFIG:Debug>:${CMAKE_CURRENT_LIST_DIR}/../${OT_LIBTESTD_PATH}>
    $<$<NOT:$<CONFIG:Debug>>:${CMAKE_CURRENT_LIST_DIR}/../${OT_LIBTESTR_PATH}>
  )
else()
  if(DEFINED ENV{OT_LIBTESTD} AND DEFINED ENV{OT_LIBTESTR})
    target_link_directories(OTViewer_tests PRIVATE
      $<$<CONFIG:Debug>:${CMAKE_CURRENT_LIST_DIR}/../$ENV{OT_LIBTESTD}>
      $<$<NOT:$<CONFIG:Debug>>:${CMAKE_CURRENT_LIST_DIR}/../$ENV{OT_LIBTESTR}>
    )
  endif()
endif()

if(DEFINED ENV{GOOGLE_TEST_LIBPATHD} AND NOT "$ENV{GOOGLE_TEST_LIBPATHD}" STREQUAL "")
  target_link_directories(OTViewer_tests PRIVATE $<$<CONFIG:Debug>:$ENV{GOOGLE_TEST_LIBPATHD}>)
endif()
if(DEFINED ENV{GOOGLE_TEST_LIBPATHR} AND NOT "$ENV{GOOGLE_TEST_LIBPATHR}" STREQUAL "")
  target_link_directories(OTViewer_tests PRIVATE $<$<NOT:$<CONFIG:Debug>>:$ENV{GOOGLE_TEST_LIBPATHR}>)
endif()

target_link_libraries(OTViewer_tests PRIVATE
  OTViewer
)

if(DEFINED ENV{GOOGLE_TEST_LIB} AND NOT "$ENV{GOOGLE_TEST_LIB}" STREQUAL "")
  target_link_libraries(OTViewer_tests PRIVATE $ENV{GOOGLE_TEST_LIB})
endif()

if(COMMAND add_test)
  add_test(NAME OTViewer_tests COMMAND OTViewer_tests)
endif()
