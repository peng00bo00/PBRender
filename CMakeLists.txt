CMAKE_MINIMUM_REQUIRED(VERSION 3.9.0)

PROJECT(PBRender
    VERSION 0.0
    DESCRIPTION "A physically-based ray tracer based on Embree API."
    LANGUAGES CXX C
)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

## Add dependencies
FIND_PACKAGE(embree 4 CONFIG REQUIRED)  ## embree
FIND_PACKAGE(TBB CONFIG REQUIRED tbb)   ## TBB
FIND_PACKAGE(Eigen3 REQUIRED)           ## eigen

## 
INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(src/core)

SET(CORE_HEADERS
    src/core/common.h
    src/core/engine.h
    src/core/math.h
    src/core/scene.h
    src/core/vecmath.h

    src/core/PBRender.h
)

SET(CORE_SOURCE 
    src/core/engine.cpp
    src/core/math.cpp
    src/core/scene.cpp
    src/core/vecmath.cpp
)

# FILE(GLOB PBRender_SOURCE
#     src/accelerators/*
#     src/cameras/*
#     src/integrators/*
#     src/lights/*
#     src/materials/*
#     src/samplers/*
#     src/shapes/*
#     src/textures/*
# )

ADD_LIBRARY(PBRender_LIB STATIC
    ${CORE_HEADERS}
    ${CORE_SOURCE}
    # ${PBRender_SOURCE}
)

ADD_EXECUTABLE(main src/main.cpp)
TARGET_LINK_LIBRARIES(main PUBLIC PBRender_LIB embree Eigen3::Eigen)