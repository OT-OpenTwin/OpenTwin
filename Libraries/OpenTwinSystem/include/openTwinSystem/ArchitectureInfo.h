#pragma once

// #########################################################################################################################################################################

// OS Info

// WINDOWS
#if defined(_WIN32) || defined(_WIN64)
//! The current OS is windows
#define OT_OS_WINDOWS

// UNIX
#elif defined(unix) || defined(__unix) || defined (__unix__)
//! The current OS is Unix
#define OT_OS_UNIX

// APPLE
#elif defined(__APPLE__) || defined(__MACH__)
//! The current OS is Mac
#define OT_OS_MAC

// LINUX
#elif defined(__linux__) || defined(linux) || defined(__linux)
//! The current OS is Linux
#define OT_OS_LINUX

// FreeBSD
#elif defined (__FreeBSD__)
//! The current OS is FreeBSD
#define OT_OS_FreeBSD

// ANDROID
#elif defined(__ANDROID__)
//! The current OS is Android
#define OT_OS_ANDROID
#endif

// #########################################################################################################################################################################

// Architecture Info

// 64Bit
#if defined(_WIN64)
//! 64bit build
#define OT_OS_64Bit

// 32Bit
#else
//! 32bit build
#define OT_OS_32Bit

#endif