cmake_minimum_required(VERSION 3.5.0)
project(lunaria VERSION 0.1.0 LANGUAGES CXX)

include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB_RECURSE all_SRCS 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

add_executable(lunaria ${all_SRCS})

find_package(Vulkan REQUIRED)
message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")

include(CPM.cmake)
CPMAddPackage("gh:charles-lunarg/vk-bootstrap#v1.4.321")
CPMAddPackage("gh:/glfw/glfw#3.4")
CPMAddPackage("gh:g-truc/glm#1.0.1")

target_link_libraries(lunaria glfw vk-bootstrap::vk-bootstrap)

if(WIN32)
    
elseif (UNIX)
    target_link_libraries(lunaria ${Vulkan_LIBRARIES})
endif()