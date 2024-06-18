CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(PBRender
    VERSION 0.0
    DESCRIPTION "A physically-based ray tracer based on Embree API."
    LANGUAGES CXX C
)

# set embree_DIR
SET(embree_DIR "/home/pengbo/embree-4.3.2.x86_64.linux/lib/cmake/embree-4.3.2")
FIND_PACKAGE(embree 4 REQUIRED)

SET(CMAKE_BUILD_TYPE Release)
SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

ADD_EXECUTABLE(main src/main.cpp)
TARGET_LINK_LIBRARIES(main embree)