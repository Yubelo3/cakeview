cmake_minimum_required(VERSION 3.9)

project(Renderer)

if(POLICY CMP0072)
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(GLEW REQUIRED)
find_package(Eigen3 REQUIRED)
set( CMAKE_CXX_FLAGS "-std=c++17" )
set(CMAKE_BUILD_TYPE DEBUG)

aux_source_directory(src SRC_LIST)
add_executable(Renderer ${SRC_LIST})

target_link_libraries(Renderer glfw)
target_link_libraries(Renderer OpenGL::GL)
target_link_libraries(Renderer GLEW::glew)