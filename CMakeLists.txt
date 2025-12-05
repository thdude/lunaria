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

add_subdirectory(libs/vk-bootstrap)
target_link_libraries(lunaria vk-bootstrap::vk-bootstrap)

if(WIN32)
    add_subdirectory(libs/glfw)
    target_link_libraries(lunaria glfw)
elseif(UNIX)
    find_package(glfw3 3.4 REQUIRED)
    set(GLFW_LIB glfw)
    message(STATUS "Found GLFW")
endif()

if(WIN32)
    
elseif (UNIX)
    target_link_libraries(lunaria glfw ${Vulkan_LIBRARIES})
endif()