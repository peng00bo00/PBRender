CMAKE_MINIMUM_REQUIRED ( VERSION 3.1.0 )

PROJECT ( PBRender )

SET(CMAKE_CXX_STANDARD 11)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

# ASSIMP
FIND_PACKAGE( ASSIMP REQUIRED )
INCLUDE_DIRECTORIES( ${ASSIMP_INCLUDE_DIR} )

ADD_SUBDIRECTORY ( src/ext/stbiw )

INCLUDE_DIRECTORIES ( src )
INCLUDE_DIRECTORIES ( src/core )

SET ( CORE_HEADERS
    src/core/geometry.h
    src/core/interaction.h
    src/core//modelloader.h
    src/core/primitive.h
    src/core/shape.h
    src/core/spectrum.h
    src/core/transform.h
)

SET ( CORE_SOURCE 
    src/core/geometry.cpp
    src/core/interaction.cpp
    src/core//modelloader.cpp
    src/core/primitive.cpp
    src/core/shape.cpp
    src/core/spectrum.cpp
    src/core/transform.cpp
)

FILE ( GLOB PBRender_SOURCE
    src/shapes/*
    src/accelerators/*
)

ADD_LIBRARY ( PBRender_LIB STATIC
    ${CORE_HEADERS}
    ${CORE_SOURCE}
    ${PBRender_SOURCE}
)

ADD_EXECUTABLE ( render src/main.cpp )
TARGET_LINK_LIBRARIES ( render PUBLIC PBRender_LIB stbiw ${ASSIMP_LIBRARIES})