#pragma once

// Embree
#include <embree4/rtcore.h>

// STB
#define STB_IMAGE_IMPLEMENTATION
#include <ext/stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <ext/stb/stb_image_write.h>

// Eigen
#include <Eigen/Eigen>

// core
#include <PBRender/core/common.h>

#include <PBRender/core/engine.h>
#include <PBRender/core/math.h>
#include <PBRender/core/scene.h>
#include <PBRender/core/vecmath.h>
#include <PBRender/core/transform.h>
#include <PBRender/core/camera.h>
#include <PBRender/core/ray.h>

// cameras
#include <PBRender/cameras/perspective.h>