cmake_minimum_required(VERSION 3.20)
project(OTGui LANGUAGES CXX)

include("$ENV{OT_CMAKE_DIR}/OTProject.cmake")

ot_initialize_lib(OTCrypto OT_CRYPTO_ROOT_PATH OT_CRYPTO_EXPORTS)

ot_add_dependency(OTCrypto
    OTSystem
    OTCore
    RJSON
    OPENSSL
    OSLibs
)

ot_finalize_lib(OTCrypto)
ot_add_test(OTCrypto)
