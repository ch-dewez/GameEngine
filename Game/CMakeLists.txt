file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

add_executable(Game ${SOURCES} ${HEADERS})

target_include_directories(Game
    PRIVATE
        ${CMAKE_SOURCE_DIR}/GameEngineCore/src
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(Game
    PRIVATE
        GameEngineCore
        glfw
        glm
        ${Vulkan_LIBRARIES}
)

# Compile shaders at build time
add_custom_target(GameShaders ALL)
add_dependencies(Game GameShaders)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders/Game")

add_custom_command(
    TARGET GameShaders
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/"
    -DOUTPUT_DIR="${CMAKE_BINARY_DIR}/shaders/Game"
    -DGLSLC_EXECUTABLE="${Vulkan_GLSLC_EXECUTABLE}"
    -P "${CMAKE_SOURCE_DIR}/cmake/CompileShaders.cmake"
    COMMENT "Compiling Game shaders"
)

# Copy assets to build directory
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/assets/Game")

add_custom_command(
    TARGET Game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/src/Assets"
    "${CMAKE_BINARY_DIR}/assets/Game"
    COMMENT "Copying Game assets to build directory"
)

# Set compile definitions based on configuration
target_compile_definitions(Game
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)
