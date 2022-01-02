CMAKE_MINIMUM_REQUIRED ( VERSION 3.9.0 )

PROJECT ( PBRender )

SET(CMAKE_CXX_STANDARD 14)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# ASSIMP
FIND_PACKAGE ( ASSIMP REQUIRED )
INCLUDE_DIRECTORIES ( ${ASSIMP_INCLUDE_DIR} )

# STBIM
ADD_SUBDIRECTORY ( src/ext/stbim )

# OpenMP
find_package( OpenMP REQUIRED )


INCLUDE_DIRECTORIES ( src )
INCLUDE_DIRECTORIES ( src/core )

SET ( CORE_HEADERS
    src/core/camera.h
    src/core/geometry.h
    src/core/integrator.h
    src/core/interaction.h
    src/core/interpolation.h
    src/core/light.h
    src/core/lightdistrib.h
    src/core/lowdiscrepancy.h
    src/core/material.h
    src/core/microfacet.h
    src/core//modelloader.h
    src/core/primitive.h
    src/core/reflection.h
    src/core/rng.h
    src/core/sampler.h
    src/core/sampling.h
    src/core/scene.h
    src/core/shape.h
    src/core/sobolmatrices.h
    src/core/spectrum.h
    src/core/texture.h
    src/core/transform.h
)

SET ( CORE_SOURCE 
    src/core/camera.cpp
    src/core/geometry.cpp
    src/core/integrator.cpp
    src/core/interaction.cpp
    src/core/interpolation.cpp
    src/core/light.cpp
    src/core/lightdistrib.cpp
    src/core/lowdiscrepancy.cpp
    src/core/material.cpp
    src/core/microfacet.cpp
    src/core//modelloader.cpp
    src/core/reflection.cpp
    src/core/primitive.cpp
    src/core/sampler.cpp
    src/core/sampling.cpp
    src/core/scene.cpp
    src/core/shape.cpp
    src/core/sobolmatrices.cpp
    src/core/spectrum.cpp
    src/core/texture.cpp
    src/core/transform.cpp
)

FILE ( GLOB PBRender_SOURCE
    src/accelerators/*
    src/cameras/*
    src/integrators/*
    src/lights/*
    src/materials/*
    src/samplers/*
    src/shapes/*
    src/textures/*
)

ADD_LIBRARY ( PBRender_LIB STATIC
    ${CORE_HEADERS}
    ${CORE_SOURCE}
    ${PBRender_SOURCE}
)

ADD_EXECUTABLE ( render src/main.cpp )
TARGET_LINK_LIBRARIES ( render PUBLIC PBRender_LIB STBIM ${ASSIMP_LIBRARIES} OpenMP::OpenMP_CXX )