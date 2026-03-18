cmake_minimum_required(VERSION 3.20)
project(app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# After your find_package calls, add:
include(FetchContent)

FetchContent_Declare(cgltf
    GIT_REPOSITORY https://github.com/jkuhlmann/cgltf.git
    GIT_TAG v1.14
)
FetchContent_MakeAvailable(cgltf)
target_include_directories(${PROJECT_NAME} PRIVATE ${cgltf_SOURCE_DIR})

# Find required packages
find_package(glfw3 REQUIRED)
find_package(Vulkan REQUIRED)
find_package(PkgConfig REQUIRED)

find_path(GLM_INCLUDE_DIR
    NAMES glm/glm.hpp
    PATHS 
        ${CMAKE_SOURCE_DIR}/include/glm
        /usr/include 
        /usr/local/include
)

# Source files
set(SOURCES
    source/main.cpp
    source/vulkan_context.cpp
    source/platform.cpp
    source/drivers/render.cpp
    source/drivers/swapchain.cpp
    source/drivers/pipeline.cpp
    source/drivers/commands.cpp
    source/drivers/sync.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_precompile_headers(${PROJECT_NAME} PRIVATE
    <vulkan/vulkan.h>
    <GLFW/glfw3.h>
    <glm/glm.hpp>
    <vector>
    <string>
    <memory>
    <array>
    <span>
)

# Include directories
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
    ${Vulkan_INCLUDE_DIRS}
    ${GLM_INCLUDE_DIR}
    ${STB_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    glfw
    Vulkan::Vulkan
    ${CMAKE_DL_LIBS}
)

# Compiler warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wno-missing-field-initializers
    )
endif()

# Debug/Release flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()

find_program(GLSLC glslc REQUIRED)

file(GLOB_RECURSE GLSL_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/source/drivers/shaders/*.glsl"
)

set(SPIRV_OUTPUT_DIR "${CMAKE_BINARY_DIR}/src/drivers/shaders")
file(MAKE_DIRECTORY ${SPIRV_OUTPUT_DIR})

foreach(GLSL ${GLSL_SOURCES})
    get_filename_component(FILE_NAME ${GLSL} NAME_WE)  # NAME_WE = name without extension
    set(SPIRV "${SPIRV_OUTPUT_DIR}/${FILE_NAME}.spv")

    # Determine stage from filename (vert.glsl -> vertex, frag.glsl -> fragment)
    if(FILE_NAME STREQUAL "vert")
        set(STAGE vertex)
    elseif(FILE_NAME STREQUAL "frag")
        set(STAGE fragment)
    elseif(FILE_NAME STREQUAL "comp")
        set(STAGE compute)
    endif()

    add_custom_command(
        OUTPUT ${SPIRV}
        COMMAND ${GLSLC} -fshader-stage=${STAGE} ${GLSL} -o ${SPIRV}
        DEPENDS ${GLSL}
        COMMENT "Compiling shader: ${FILE_NAME}.glsl"
        USES_TERMINAL
    )

    list(APPEND SPIRV_OUTPUTS ${SPIRV})
endforeach()

add_custom_target(shaders ALL DEPENDS ${SPIRV_OUTPUTS})
add_dependencies(${PROJECT_NAME} shaders)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/include/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/src/
    DESTINATION include
    FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
)

install(DIRECTORY ${SPIRV_OUTPUT_DIR}/
    DESTINATION share/${PROJECT_NAME}/shaders
    FILES_MATCHING PATTERN "*.spv"
)
