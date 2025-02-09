cmake_minimum_required(VERSION 3.27)
project(idk_core)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dev")
cmake_policy(SET CMP0167 NEW)
set(CMAKE_VERBOSE_MAKEFILE ON)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")


add_definitions(-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (WIN32)
       # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
       # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
    elseif (UNIX)
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
    endif()
endif()


if(EXISTS "${CMAKE_SOURCE_DIR}/.clang-tidy")
    set(CLANG_TIDY_EXCLUDE_DIRS "external")
else()
    message(STATUS "No .clang-tidy file found, clang-tidy not set.")
endif()

set(BOOST_ROOT "${CMAKE_SOURCE_DIR}/external/boost/")
set(BOOST_INC_DIR "${CMAKE_SOURCE_DIR}/external/boost/include/")
set(BOOST_LIB_DIR "${CMAKE_SOURCE_DIR}/external/boost/lib")

find_package(Boost 1.87 REQUIRED COMPONENTS filesystem system)

if(Boost_FOUND)
    message(STATUS "Boost found!")
    include_directories(${BOOST_INC_DIR})
    link_directories(${BOOST_LIB_DIR})
else ()
    message(FATAL_ERROR "Boost not found")
endif ()

include_directories(
        ${PROJECT_SOURCE_DIR}/src/Engine/Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities
        ${PROJECT_SOURCE_DIR}/src/Engine/Lighting
        ${PROJECT_SOURCE_DIR}/src/Editor/Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities
)

set(HEADER_FILES
        # Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/Initialization.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/GameObject.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/Component.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/AssetItem.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/AssetManager.h

        # Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Shader.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Renderer.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Camera.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Material.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/MeshRenderer.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Mesh.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/MeshFilter.h

        # Physics
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/BoxCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/Cube.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/Cylinder.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/CylinderCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/BoxCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/CapsuleCollider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/Collider.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/Ray.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/Sphere.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics/SphereCollider.h

        # Scene Management
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/Scene.h

        # Lighting
        ${PROJECT_SOURCE_DIR}/src/Engine/Lighting/DirectionalLight.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Lighting/LightManager.h
        ${PROJECT_SOURCE_DIR}/src/Engine/Lighting/Light.h

        # Utilities
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities/Transform.h

        # ========================== Editor ==========================
        # Core
        ${PROJECT_SOURCE_DIR}/src/Editor/Core/HierarchyManager.h
        ${PROJECT_SOURCE_DIR}/src/Editor/Core/InspectorManager.h
        ${PROJECT_SOURCE_DIR}/src/Editor/Core/ProjectExplorer.h

        ${PROJECT_SOURCE_DIR}/src/Editor/Utilities/SelectionManager.h
)
set(SOURCE_FILES
        # Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/AssetManager.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Core/Initialization.cpp

        # Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Shader.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Renderer.cpp
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering/Mesh.cpp

        # Physics

        # Scene Management
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement/Scene.cpp

        # Lighting

        # ========================== Editor ==========================
        # Core
        ${PROJECT_SOURCE_DIR}/src/Editor/Core/HierarchyManager.cpp
        ${PROJECT_SOURCE_DIR}/src/Editor/Core/InspectorManager.cpp
        ${PROJECT_SOURCE_DIR}/src/Editor/Core/ProjectExplorer.cpp

        ${PROJECT_SOURCE_DIR}/src/Editor/Utilities/SelectionManager.cpp
        src/Engine/Utilities/Profiler.h
        # src/Engine/Rendering/ShaderManager.cpp
       # src/Engine/Rendering/ShaderManager.h
        src/Engine/Physics/Capsule.h
        src/Engine/Rendering/Shader.cpp
        src/Engine/Rendering/Shader.h
        src/Engine/Rendering/ShaderManager.cpp
        src/Engine/Rendering/ShaderManager.h
)

# GLFW 3
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/external/glfw/include")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/external/glfw/lib/libglfw3.a")

include_directories(${GLFW_INCLUDE_DIR})

set(ICONS_FONT_AWESOME_6_DIR ${CMAKE_SOURCE_DIR}/external/fontawesome)

# GLAD
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glad/include)
set(GLAD_SOURCE ${CMAKE_SOURCE_DIR}/external/glad/src/glad.c
        src/Engine/Utilities/FPSCounter.cpp
        src/Engine/Utilities/FPSCounter.h
)

add_library(glad ${GLAD_SOURCE})
include_directories(${GLAD_INCLUDE_DIR})


# ImGuizmo
set(IMGUIzmo_PATH ${CMAKE_SOURCE_DIR}/external/ImGuizmo)

# IMGUI
set(IMGUI_PATH ${CMAKE_SOURCE_DIR}/external/imgui)

#GLM
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/external/glm/include)

# STB_IMAGE
set(STB_IMAGE_DIR ${CMAKE_SOURCE_DIR}/external/stb_image)

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

add_library(ImGui ${IMGUI_SOURCES})

include_directories(${IMGUI_PATH})
include_directories(${IMGUIzmo_PATH})
include_directories(${GLM_INCLUDE_DIR})
include_directories(${STB_IMAGE_DIR})
include_directories(${ICONS_FONT_AWESOME_6_DIR})
include_directories(
        ${PROJECT_SOURCE_DIR}/src/Engine/Core
        ${PROJECT_SOURCE_DIR}/src/Engine/Rendering
        ${PROJECT_SOURCE_DIR}/src/Engine/Physics
        ${PROJECT_SOURCE_DIR}/src/Engine/SceneManagement
        ${PROJECT_SOURCE_DIR}/src/Engine/Utilities
        ${PROJECT_SOURCE_DIR}/src/Engine/Lighting
        ${PROJECT_SOURCE_DIR}/src/Engine/Core
        ${PROJECT_SOURCE_DIR}/src/Editor/Utilities
)

##############################
# System packages
##############################
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)


add_library(libData SHARED src/libdata/libData.cpp)
set(CMAKE_SHARED_LIBRARY_PREFIX "")

add_executable(idk_core main.cpp ${SOURCE_FILES})

link_directories("${PROJECT_SOURCE_DIR}/cmake-build-debug")

target_link_libraries(libData
        PRIVATE
        glad
        ImGui
        ${GLFW_LIBRARY}
        Boost::filesystem
        Boost::system
        Threads::Threads
        OpenGL::GL
)

target_link_libraries(idk_core
        PRIVATE
        glad
        ImGui
        ${GLFW_LIBRARY}
        Boost::filesystem
        Boost::system
        Threads::Threads
        OpenGL::GL
        libData
)

target_include_directories(idk_core PUBLIC "${GLFW3_INCLUDE_DIR}")

if (UNIX)
    target_link_libraries(idk_core
            PRIVATE
            opengl32 gdi32
    )
endif()


if (MSVC)
    target_compile_options(idk_core PRIVATE /W4 /EHsc /std:c++20)
else()
    target_compile_options(idk_core PRIVATE -Wall -Wextra -std=c++20)
endif()
