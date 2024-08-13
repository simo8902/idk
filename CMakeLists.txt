cmake_minimum_required(VERSION 3.27)
project(LupusFire_core)

add_definitions(-DSOURCE_DIR="${CMAKE_SOURCE_DIR}")

include_directories(
        ${PROJECT_SOURCE_DIR}/src/Engine/Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities
)

set(HEADER_FILES
        # Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/Initialization.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/GameObject.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/SubGameObject.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/Component.h

        # Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Shader.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Renderer.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Camera.h

        # Physics
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/BoxCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/Capsule.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/CapsuleCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/Collider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/Cube.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/Cylinder.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/CylinderCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/Ray.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/Sphere.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.h/SphereCollider.h

        # Scene Management
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/Scene.h
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/HierarchyManager.h
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/InspectorManager.h

        # Utilities
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities/Transform.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities/ProjectExplorer.h
)

set(SOURCE_FILES
        # Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/Initialization.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/GameObject.cpp

        # Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Shader.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Renderer.cpp

        # Physics
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.cpp/Capsule.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.cpp/Cube.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.cpp/Cylinder.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.cpp/Sphere.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/.cpp/SphereCollider.cpp

        # Scene Management
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/Scene.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/HierarchyManager.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/InspectorManager.cpp

        # Utilities
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities/ProjectExplorer.cpp
)


# GLAD
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/glad/include)
set(GLAD_SOURCE ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)

# GLAD
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/glad/include)
set(GLAD_SOURCE ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)

# ImGuizmo
set(IMGUIzmo_PATH ${CMAKE_SOURCE_DIR}/libs/ImGuizmo)

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
        ${IMGUIzmo_PATH}/*.cpp
        ${IMGUIzmo_PATH}/*.h
)

add_library(glad ${CMAKE_SOURCE_DIR}/libs/glad/src/glad.c)
add_library(ImGui ${IMGUI_SOURCES})

include_directories(${IMGUI_PATH})
include_directories(${IMGUIzmo_PATH})
include_directories(${GLFW_INCLUDE_DIR})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${GLAD_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 17)

add_executable(LupusFire_core main.cpp ${GLAD_SOURCE} ${SOURCE_FILES} ${HEADER_FILES})

target_link_libraries(LupusFire_core PRIVATE ${GLFW_LIBRARIES} ImGui opengl32 imm32)