cmake_minimum_required(VERSION 3.19)
project(safeshared_tests)

set(CMAKE_CXX_STANDARD 17)

# Enable pthreads on UNIX
if(UNIX)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
endif()

# Download Catch2
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.1.0
)
FetchContent_MakeAvailable(Catch2)

# Files
set(GLOBBING_EXPRESSION 
    "${CMAKE_CURRENT_SOURCE_DIR}/**.cpp")
file(GLOB_RECURSE SOURCES 
    ${GLOBBING_EXPRESSION})

# Target(`safeshared_tests`): executable tests
add_executable(${PROJECT_NAME} 
	${SOURCES})
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
        Catch2::Catch2WithMain)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} 
        PRIVATE 
            Threads::Threads)
endif()

# Automatically discover tests
include(CTest)
include(Catch)
catch_discover_tests(${PROJECT_NAME})

# Treat warnings as errors
if(MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
else()
  target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Place Catch2 projects into separate folder
set_target_properties(Catch2
    PROPERTIES FOLDER "Catch2")
set_target_properties(Catch2WithMain 
    PROPERTIES FOLDER "Catch2")

# Configure tests as default startup project
set_property(
    DIRECTORY ${CMAKE_SOURCE_DIR} 
    PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
