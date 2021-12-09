cmake_minimum_required(VERSION 3.16)
project(breakupModel)

set(CMAKE_CXX_STANDARD 17)

#Appends the the module path to contain additional CMake modules for this project
list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

#Incorporate yaml-cpp into the project
include(yaml)

#Incorporate spdlog into the project
include(spdlog)

#Also include the cpp files
file(GLOB_RECURSE SRC
        "${PROJECT_SOURCE_DIR}/src/breakupModel/*.h"
        "${PROJECT_SOURCE_DIR}/src/breakupModel/*.cpp")

set(mainFile ${PROJECT_SOURCE_DIR}/src/main.cpp)

add_library(${PROJECT_NAME}_lib ${SRC})

#Adds the include Path PROJECT_SOURCE_DIR/src to the target breakupModel
target_include_directories(${PROJECT_NAME}_lib PUBLIC "${PROJECT_SOURCE_DIR}/src")

#Links the executable against the yaml-cpp
target_link_libraries(${PROJECT_NAME}_lib
        yaml-cpp
        spdlog
        )

if (NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(STATUS "Linking tbb libary")
    target_link_libraries(${PROJECT_NAME}_lib
            tbb)
endif()

#Option to simulation or not
option(BUILD_BREAKUP_MODEL_SIM "Set to on if the simulation should be built (Default: ON)" ON)
if(BUILD_BREAKUP_MODEL_SIM)
    #Creates the executable target from the source files
    add_executable(${PROJECT_NAME} "${mainFile}")
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_lib)
endif()

#Option to build tests or not
option(BUILD_BREAKUP_MODEL_TESTS "Set to on if the tests should be built (Default: ON)" ON)
#Only build the tests if they are enabled
if(BUILD_BREAKUP_MODEL_TESTS)
    message("-- Building the Breakup Model Tests")
    #Enables CTest, must be in the top-level CMakeList.txt, otherwise it won't work
    enable_testing()

    #Subdirectory where the tests are located
    add_subdirectory(${PROJECT_SOURCE_DIR}/test)
endif()