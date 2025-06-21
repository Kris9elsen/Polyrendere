cmake_minimum_required(VERSION 3.10)

# Set project name and C++ standard
project(arv_eks VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Define source and header directories
set(SRC_DIR "src")
set(HEADER_DIR "headers")

# Collect all source and header files
file(GLOB_RECURSE SOURCES "${SRC_DIR}/*.cpp")
file(GLOB_RECURSE HEADERS "${HEADER_DIR}/*.h" "${HEADER_DIR}/*.hpp")

# Add executable target
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
include_directories(${HEADER_DIR})

# Optional: Enable warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
elseif (MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
endif()