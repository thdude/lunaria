cmake_minimum_required(VERSION 3.5.0)
project(lunaria VERSION 0.1.0 LANGUAGES CXX)

include_directories(${PROJECT_SOURCE_DIR}/src)
file(GLOB all_SRCS 
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.h"
)

add_executable(lunaria ${all_SRCS})

find_package(Vulkan REQUIRED)
message(STATUS "Found Vulkan: $ENV{VULKAN_SDK}")

include(CPM.cmake)

# CPMAddPackage("gh:charles-lunarg/vk-bootstrap@latest")
# CPMAddPackage("gh:/glfw/glfw@latest")
CPMAddPackage(https://github.com/charles-lunarg/vk-bootstrap)
CPMAddPackage(https://github.com/glfw/glfw)


target_link_libraries("glfw::glfw charles-lunarg::vk-bootstrap")

if(WIN32)
    
elseif (UNIX)
    target_link_libraries(lunaria ${Vulkan_LIBRARIES})
endif()