cmake_minimum_required(VERSION 3.19)

project(safeshared
    VERSION 0.1
    DESCRIPTION "Collection of containers for the safe usage of shared content in concurrent environments"
    HOMEPAGE_URL "" #TODO: enter github repo
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Enable pthreads on UNIX
if(UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(STANDALONE ON)
else()
    set(STANDALONE OFF)
endif()

option(BUILD_TESTS "Build tests" ${STANDALONE})

if (CMAKE_CXX_COMPILER_ID MATCHES "(Clang|GNU)")
   list(APPEND flags "-Wall" "-Wextra" "-Wshadow" "-Wnon-virtual-dtor" "-pedantic")
elseif(MSVC)
   list(APPEND flags "/W4" "/WX")
endif()

# Include Directories
include_directories(
    PUBLIC 
        "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Files
set(GLOBBING_EXPRESSION 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/**.inl")
file(GLOB_RECURSE SOURCES 
    ${GLOBBING_EXPRESSION})

# Target(`safeshared`): library to include with `add_subdirectory()`
add_library(${PROJECT_NAME} INTERFACE ${SOURCES})
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE 
            Threads::Threads)
endif()

# Add tests, if needed
if(BUILD_TESTS)
	enable_testing()
	add_subdirectory(test)
endif()

# Organize dependency projects in folders
set_property(
    GLOBAL 
    PROPERTY USE_FOLDERS ON)
