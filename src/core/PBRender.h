#pragma once

#include <assert.h>
#include <cmath>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace PBRender {

static float Infinity = std::numeric_limits<float>::infinity();

// Geometry
template <typename T>
class Vector2;

template <typename T>
class Vector3;

template <typename T>
class Point3;

template <typename T>
class Point2;

template <typename T>
class Normal3;

template <typename T>
class Bounds2;

template <typename T>
class Bounds3;

class Ray;

// Global Inline Functions
inline float Lerp(float t, float v1, float v2) { return (1 - t) * v1 + t * v2; }
}