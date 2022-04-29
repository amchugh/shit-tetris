
cmake_minimum_required(VERSION 3.10)

project(shittetris VERSION 1.0)

IF (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, defaulting to Debug. \
Add `-DCMAKE_BUILD_TYPE=Release` at cmake time to generate a release build")
    set(CMAKE_BUILD_TYPE "Debug")
ELSE()
    message(STATUS "Building ${CMAKE_BUILD_TYPE}")
ENDIF()

# Specify our C++ standard version
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# We would like to output to the bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Add some compiler options
IF(NOT WIN32)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -ggdb3")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra")
ENDIF()

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}"
)

include(FetchContent)
# Get RAYLIB
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games
FetchContent_Declare(raylib GIT_REPOSITORY https://github.com/raysan5/raylib.git GIT_TAG master)
FetchContent_MakeAvailable(raylib)

# FetchContent_Declare(cimg GIT_REPOSITORY https://github.com/dtschump/CImg.git GIT_TAG master)
# FetchContent_MakeAvailable(cimg)

add_executable(shittetris main.cpp background_sim.cpp intro.cpp)
target_link_libraries(shittetris raylib)

IF (NOT WIN32 AND CMAKE_BUILD_TYPE MATCHES Debug)
    # We want to create a symbolic link to the rsc directory instead of copying it
    add_custom_command(TARGET shittetris POST_BUILD
        # The `|| exit 0` allows the ln command to fail without aborting the make :)
        COMMAND ln -s ${CMAKE_SOURCE_DIR}/rsc ${CMAKE_BINARY_DIR} 2> /dev/null || exit 0
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMENT "Creating symbolic link to repository rsc directory"
    )
ELSE()
    # Otherwise, we copy resources.
    file(COPY ${CMAKE_SOURCE_DIR}/rsc DESTINATION ${CMAKE_BINARY_DIR})
ENDIF()
