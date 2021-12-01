#if defined(_MSC_VER)
#define NOMINMAX
#pragma once
#endif

#ifndef PBRT_CORE_PBRT_H
#define PBRT_CORE_PBRT_H

// core/pbrt.h*
// Global Include Files
#include <type_traits>
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include "error.h"
#ifdef PBRT_HAVE_MALLOC_H
#include <malloc.h>  // for _alloca, memalign
#endif
#ifdef PBRT_HAVE_ALLOCA_H
#include <alloca.h>
#endif
#include <assert.h>
#include <string.h>
#include <glog/logging.h>

// Platform-specific definitions
#if defined(_WIN32) || defined(_WIN64)
  #define PBRT_IS_WINDOWS
#endif

#if defined(_MSC_VER)
  #define PBRT_IS_MSVC
  #if _MSC_VER == 1800
    #define snprintf _snprintf
  #endif
#endif

#ifndef PBRT_L1_CACHE_LINE_SIZE
  #define PBRT_L1_CACHE_LINE_SIZE 64
#endif

#include <stdint.h>
#if defined(PBRT_IS_MSVC)
#include <float.h>
#include <intrin.h>
#pragma warning(disable : 4305)  // double constant assigned to float
#pragma warning(disable : 4244)  // int -> float conversion
#pragma warning(disable : 4843)  // double -> float conversion
#pragma warning(disable : 4267)  // size_t -> int
#pragma warning(disable : 4838)  // another double -> int
#endif

// Global Macros
#define ALLOCA(TYPE, COUNT) (TYPE *) alloca((COUNT) * sizeof(TYPE))

namespace pbrt {

// Global Forward Declarations
class Scene;
class Integrator;
class SamplerIntegrator;
// geometry
template <typename T>
class Vector2;
template <typename T>
class Vector3;
template <typename T>
class Point3;
template <typename T>
class Point2;
}