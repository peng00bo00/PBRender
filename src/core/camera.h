#pragma once

#include "PBRender.h"

#include "geometry.h"
#include "transform.h"

namespace PBRender {

// Camera Declarations
class Camera {
    
};

struct CameraSample {
    Point2f pFilm;
    Point2f pLens;
    float time;
};

class ProjectiveCamera : public Camera {

};

}