file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

add_library(GameEngineCore STATIC ${SOURCES} ${HEADERS})

# Compile shaders at build time
add_custom_target(GameEngineCoreShaders ALL)
add_dependencies(GameEngineCore GameEngineCoreShaders)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/shaders/GameEngineCore")

add_custom_command(
    TARGET GameEngineCoreShaders
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
    -DSOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src"
    -DOUTPUT_DIR="${CMAKE_BINARY_DIR}/shaders/GameEngineCore"
    -DGLSLC_EXECUTABLE="${Vulkan_GLSLC_EXECUTABLE}"
    -P "${CMAKE_SOURCE_DIR}/cmake/CompileShaders.cmake"
    COMMENT "Compiling GameEngineCore shaders"
)

target_compile_definitions(GameEngineCore 
    PUBLIC 
        GLFW_INCLUDE_VULKAN
        STB_IMAGE_IMPLEMENTATION
    PRIVATE
        $<$<CONFIG:Debug>:DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
)

target_include_directories(GameEngineCore
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${stb_image_SOURCE_DIR}
    PRIVATE
        ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(GameEngineCore
    PUBLIC
        glfw
        glm
        ${Vulkan_LIBRARIES}
        spirv-reflect-static
)
