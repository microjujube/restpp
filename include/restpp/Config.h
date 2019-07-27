//
// Created by Administrator on 2019/7/27.
//

#ifndef RESTPP_CONFIG_H
#define RESTPP_CONFIG_H

// Copyright 2015-2016 Ettus Research LLC
// Copyright 2018 Ettus Research, a National Instruments Company
//
// SPDX-License-Identifier: GPL-3.0-or-later
//

#ifdef _MSC_VER
// Bring in "and", "or", and "not"
#include <iso646.h>

// Define ssize_t
#include <stddef.h>
typedef ptrdiff_t ssize_t;

#endif /* _MSC_VER */

// Define cross-platform macros
#if defined(_MSC_VER)
#define RESTPP_EXPORT         __declspec(dllexport)
#define RESTPP_IMPORT         __declspec(dllimport)
#define RESTPP_INLINE         __forceinline
#define RESTPP_DEPRECATED     __declspec(deprecated)
#define RESTPP_ALIGNED(x)     __declspec(align(x))
#define RESTPP_UNUSED(x)      x
#elif defined(__MINGW32__)
#define RESTPP_EXPORT         __declspec(dllexport)
#define RESTPP_IMPORT         __declspec(dllimport)
#define RESTPP_INLINE         inline
#define RESTPP_DEPRECATED     __declspec(deprecated)
#define RESTPP_ALIGNED(x)     __declspec(align(x))
#define RESTPP_UNUSED(x)      x __attribute__((unused))
#elif defined(__GNUC__) && __GNUC__ >= 4
#define RESTPP_EXPORT         __attribute__((visibility("default")))
#define RESTPP_IMPORT         __attribute__((visibility("default")))
#define RESTPP_INLINE         inline __attribute__((always_inline))
#define RESTPP_DEPRECATED     __attribute__((deprecated))
#define RESTPP_ALIGNED(x)     __attribute__((aligned(x)))
#define RESTPP_UNUSED(x)      x __attribute__((unused))
#elif defined(__clang__)
#define RESTPP_EXPORT         __attribute__((visibility("default")))
#define RESTPP_IMPORT         __attribute__((visibility("default")))
#define RESTPP_INLINE         inline __attribute__((always_inline))
#define RESTPP_DEPRECATED     __attribute__((deprecated))
#define RESTPP_ALIGNED(x)     __attribute__((aligned(x)))
#define RESTPP_UNUSED(x)      x __attribute__((unused))
#else
#define RESTPP_EXPORT
#define RESTPP_IMPORT
#define RESTPP_INLINE         inline
#define RESTPP_DEPRECATED
#define RESTPP_ALIGNED(x)
#define RESTPP_UNUSED(x)      x
#endif

// API declaration macro

// Define API declaration macro
#ifdef RESTPP_STATIC_LIB
#define RESTPP_API
#else
#ifdef RESTPP_DLL_EXPORTS
#define RESTPP_API RESTPP_EXPORT
#else
#define RESTPP_API RESTPP_IMPORT
#endif // RESTPP_DLL_EXPORTS
#endif // RESTPP_STATIC_LIB

// Platform defines for conditional code:
// Taken from boost/config/select_platform_config.hpp,
// However, we define macros, not strings, for platforms.
#if (defined(linux) || defined(__linux) || defined(__linux__) || defined(__GLIBC__)) && !defined(_CRAYC) && !defined(__FreeBSD_kernel__) && !defined(__GNU__)
#define RESTPP_PLATFORM_LINUX
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define RESTPP_PLATFORM_WIN32
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define RESTPP_PLATFORM_MACOS
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__FreeBSD_kernel__)
#define RESTPP_PLATFORM_BSD
#endif

#endif //RESTPP_CONFIG_H
