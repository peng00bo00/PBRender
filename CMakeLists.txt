CMAKE_MINIMUM_REQUIRED ( VERSION 3.1.0 )

PROJECT ( PBRender )

ADD_SUBDIRECTORY ( src/ext/stbiw )

INCLUDE_DIRECTORIES ( src )
INCLUDE_DIRECTORIES ( src/core )

SET ( CORE_HEADERS
    src/core/interaction.h
    src/core/geometry.h
    src/core/shape.h
    src/core/transform.h
)

SET ( CORE_SOURCE 
    src/core/interaction.cpp
    src/core/geometry.cpp
    src/core/shape.cpp
    src/core/transform.cpp
)

FILE ( GLOB PBRender_SOURCE
    src/shapes/*
)

ADD_LIBRARY ( PBRender_LIB STATIC
    ${CORE_HEADERS}
    ${CORE_SOURCE}
    ${PBRender_SOURCE}
)

ADD_EXECUTABLE ( render src/main.cpp )
TARGET_LINK_LIBRARIES ( render PUBLIC PBRender_LIB stbiw )