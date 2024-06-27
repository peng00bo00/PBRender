CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

IF (NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE Release)
ENDIF()

SET(CMAKE_CXX_STANDARD 20)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_EXTENSIONS OFF)

PROJECT(PBRender
    VERSION 0.0
    DESCRIPTION "A physically-based ray tracer based on Embree API."
    LANGUAGES CXX C
)

## Add dependencies
FIND_PACKAGE(embree 4 CONFIG REQUIRED)              ## embree
FIND_PACKAGE(TBB CONFIG REQUIRED COMPONENTS tbb)    ## TBB
FIND_PACKAGE(Eigen3 REQUIRED)                       ## Eigen

FILE(GLOB_RECURSE sources src/*.cpp)

add_executable(main ${sources})
# AUX_SOURCE_DIRECTORY(/src/core sources)
# TARGET_SOURCES(main PUBLIC ${sources})
TARGET_INCLUDE_DIRECTORIES(main PUBLIC include)

TARGET_LINK_LIBRARIES(main PUBLIC embree Eigen3::Eigen TBB::tbb)