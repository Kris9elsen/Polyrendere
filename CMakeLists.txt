cmake_minimum_required(VERSION 3.10)

# Set project name and C++ standard
project(Polyrender VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER g++)

# Define source and header directories
set(SRC_DIR "sources")
set(HEADER_DIR "headers")

# Collect all source and header files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${HEADER_DIR}/*.h" "${HEADER_DIR}/*.hpp")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
include_directories(${HEADER_DIR})

find_package(X11 REQUIRED)
include_directories(${X11_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})

# Optional: Enable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()