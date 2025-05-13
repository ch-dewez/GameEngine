
cmake_minimum_required(VERSION 3.16)

project(GameEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compile_commands.json generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Create symlink for compile_commands.json
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/compile_commands.json
    )
endif()

# Set default build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "Setting build type to Debug as none was specified")
endif()

# Add debug symbols for Debug builds only
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Options
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Set Vulkan SDK path before find_package
set(VULKAN_SDK "/Users/Charles/VulkanSDK/1.4.309.0/macOS")
set(Vulkan_INCLUDE_DIR "${VULKAN_SDK}/include")
set(Vulkan_LIBRARY "${VULKAN_SDK}/lib/libvulkan.dylib")

# Find Vulkan
find_package(Vulkan REQUIRED)

# Find glslc compiler
find_program(Vulkan_GLSLC_EXECUTABLE
    NAMES glslc
    HINTS
        ${VULKAN_SDK}/bin
        $ENV{VULKAN_SDK}/bin
    REQUIRED
)

if(NOT Vulkan_GLSLC_EXECUTABLE)
    message(FATAL_ERROR "glslc shader compiler not found!")
endif()

message(STATUS "Found glslc compiler: ${Vulkan_GLSLC_EXECUTABLE}")

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.8
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 0.9.9.8
)

FetchContent_Declare(
    SPIRV-Reflect
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Reflect.git
    GIT_TAG main 
)

FetchContent_Declare(
    stb_image
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_Declare(
    tiny_obj_loader
    GIT_REPOSITORY https://github.com/tinyobjloader/tinyobjloader
    GIT_TAG release
)


# Configure GLFW options
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

# Configure SPIRV-Reflect options
set(SPIRV_REFLECT_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_EXECUTABLE OFF CACHE BOOL "" FORCE)
set(SPIRV_REFLECT_STATIC_LIB ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glm SPIRV-Reflect stb_image tiny_obj_loader)

# Add main projects
add_subdirectory(GameEngineCore)
add_subdirectory(Game)

