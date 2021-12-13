CMAKE_MINIMUM_REQUIRED ( VERSION 3.1.0 )

PROJECT ( PBRender )

INCLUDE_DIRECTORIES ( src )
INCLUDE_DIRECTORIES ( src/core )

SET ( CORE_HEADERS
    src/core/geometry.h
)

SET ( CORE_SOURCE 
    src/core/geometry.cpp
)

ADD_LIBRARY ( PBRender_LIB STATIC
    ${CORE_HEADERS}
    ${CORE_SOURCE}
)

ADD_EXECUTABLE ( render src/main.cpp )
TARGET_LINK_LIBRARIES ( render PUBLIC PBRender_LIB )