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

find_package(glfw3 3.3 REQUIRED)
set(GLFW_LIB glfw)
message(STATUS "Found GLFW")

if(WIN32)
    target_link_libraries(lunaria glfw3 vulkan-1)
elseif (UNIX)
    target_link_libraries(lunaria glfw ${Vulkan_LIBRARIES})
endif()