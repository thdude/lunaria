cmake_minimum_required(VERSION 3.5.0)
project(lunaria VERSION 0.1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE all_SRCS 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(lunaria ${all_SRCS})
#set_target_properties(lunaria PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIRECTORY}/$<0:>)

find_package(Vulkan REQUIRED)
message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")

include(CPM.cmake)
CPMAddPackage("gh:charles-lunarg/vk-bootstrap#v1.4.321")
CPMAddPackage("gh:glfw/glfw#3.4")
CPMAddPackage("gh:g-truc/glm#1.0.1")
CPMAddPackage("gh:GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator#v3.3.0")
CPMAddPackage("gh:zeux/volk#1.4.304")

target_compile_definitions(lunaria PRIVATE VK_NO_PROTOTYPES GLM_FORCE_DEPTH_ZERO_TO_ONE GLM_FORCE_LEFT_HANDED)
target_link_libraries(lunaria glfw vk-bootstrap::vk-bootstrap glm::glm volk::volk GPUOpen::VulkanMemoryAllocator)
target_include_directories(lunaria PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

#compile shaders
set(GLSLANG "glslangValidator")
macro(add_shader inName outName)
    set(SHADER_IN_PATH "${CMAKE_CURRENT_SOURCE_DIR}/shaders/${inName}")
    set(SHADER_OUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/shaders/${outName}.spv")
    list(APPEND SHADER_OUT_NAMES ${SHADER_OUT_PATH}) 
    add_custom_command(
            MAIN_DEPENDENCY ${SHADER_IN_PATH}
            OUTPUT ${SHADER_OUT_PATH}
            COMMAND ${GLSLANG} ${SHADER_IN_PATH} -o ${SHADER_OUT_PATH} --target-env vulkan1.3 ${ARGN}
            VERBATIM)
endmacro()

add_shader("render.vert" "render.vert")
add_shader("render.frag" "render.frag")
add_custom_target(build_shaders DEPENDS ${SHADER_OUT_NAMES})
add_dependencies(lunaria build_shaders)

if(WIN32)
    
elseif (UNIX)
    target_link_libraries(lunaria ${Vulkan_LIBRARIES})
endif()