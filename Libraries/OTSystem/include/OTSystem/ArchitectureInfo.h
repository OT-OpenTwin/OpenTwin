//! \file ArchitectureInfo.h
//! \brief The ArchitectureInfo contains multiple definitions that specify the current OS architecture.
//! \author Alexander Kuester (alexk95)
//! \date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// #########################################################################################################################################################################

// OS Info

// WINDOWS
#if defined(_WIN32) || defined(_WIN64)
//! \def OT_OS_WINDOWS
//! \brief If defined the current OS is Windows.
#define OT_OS_WINDOWS

// UNIX
#elif defined(unix) || defined(__unix) || defined (__unix__)
//! \def OT_OS_UNIX
//! \brief If defined the current OS is Unix.
#define OT_OS_UNIX

// APPLE
#elif defined(__APPLE__) || defined(__MACH__)
//! \def OT_OS_MAC
//! \brief If defined the current OS is Mac.
#define OT_OS_MAC

// LINUX
#elif defined(__linux__) || defined(linux) || defined(__linux)
//! \def OT_OS_LINUX
//! \brief If defined the current OS is Linux.
#define OT_OS_LINUX

// FreeBSD
#elif defined (__FreeBSD__)
//! \def OT_OS_FreeBSD
//! \brief If defined the current OS is FreeBSD.
#define OT_OS_FreeBSD

// ANDROID
#elif defined(__ANDROID__)
//! \def OT_OS_FreeBSD
//! \brief If defined the current OS is Android.
#define OT_OS_ANDROID
#endif

// #########################################################################################################################################################################

// Architecture Info

// 64Bit
#if defined(_WIN64)
//! \def OT_OS_64Bit
//! \brief If defined the current OS has a 64 bit architecture.
#define OT_OS_64Bit

// 32Bit
#else
//! \def OT_OS_32Bit
//! \brief If defined the current OS has a 32 bit architecture.
#define OT_OS_32Bit

#endif

// #########################################################################################################################################################################

// Compiler Info

#if defined(__GNUC__)

//! \def OT_COMPILER_GNU
//! \brief If define the current compiler is GNU.
#define OT_COMPILER_GNU

#elif defined(__clang__)

//! \def OT_COMPILER_CLANG
//! \brief If define the current compiler is CLANG.
#define OT_COMPILER_CLANG

#elif defined(_MSC_VER)

//! \def OT_COMPILER_MSC
//! \brief If define the current compiler is Microsoft Visual C++.
#define OT_COMPILER_MSC

#endif
