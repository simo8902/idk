cmake_minimum_required(VERSION 3.27)
project(LupusFire_core)

add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

# GLAD
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/glad/include)
set(GLAD_SOURCE ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c
        src/Scene.cpp
        src/Scene.h
        src/Components/Component.cpp
        src/Components/Component.h
        src/Components/Renderer.cpp
        src/Components/Renderer.h
        src/Components/Transform.cpp
        src/Components/Transform.h
        src/Utils.cpp
        src/Utils.h
        src/Components/BoundingBox.cpp
        src/Components/BoundingBox.h)

# IMGUI
set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/libs/imgui)

#GLM
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/glm/include)

#GLFW
set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/GLFW/include)
set(GLFW_LIBRARIES ${CMAKE_SOURCE_DIR}/libs/GLFW/lib/libglfw3.a)

file(GLOB IMGUI_SOURCES
        ${IMGUI_PATH}/*.cpp
        ${IMGUI_PATH}/*.h
        ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp
        ${IMGUI_PATH}/backends/imgui_impl_glfw.h
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_PATH}/backends/imgui_impl_opengl3.h
)

add_library(glad ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)
add_library(ImGui ${IMGUI_SOURCES})

include_directories(${IMGUI_PATH})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${GLAD_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(LupusFire_core main.cpp ${GLAD_SOURCE}
        src/Shader.cpp
        src/Shader.h
    src/GameObject.cpp
    src/GameObject.h
    src/GUI.cpp
    src/GUI.h
        src/Cube.cpp
        src/Cube.h
        src/Camera.cpp
        src/Camera.h)

target_link_libraries(LupusFire_core PRIVATE ${GLFW_LIBRARIES} ImGui opengl32)