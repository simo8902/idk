cmake_minimum_required(VERSION 3.27)
project(idk_core)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-dev")
set(CMAKE_VERBOSE_MAKEFILE ON)
cmake_policy(SET CMP0167 NEW)

# set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=lld")

add_definitions(-DSOURCE_DIR=\"${CMAKE_SOURCE_DIR}\")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    if (WIN32)
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
       #  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")

       #  set(ENV{ASAN_OPTIONS} "detect_leaks=1:halt_on_error=1")
    elseif (UNIX)
        # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls")
        # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined")
    endif()
endif()

if(MSVC)
       # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fsanitize=address")
       #  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /fsanitize=address")
       #  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zi")
       #  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DEBUG")
        add_compile_options(/MP)
endif()

set(ICONS_FONT_AWESOME_6_DIR ${CMAKE_SOURCE_DIR}/external/fontawesome)

# STB_IMAGE
set(STB_IMAGE_DIR ${CMAKE_SOURCE_DIR}/external/stb_image)

include_directories(${STB_IMAGE_DIR})
include_directories(${ICONS_FONT_AWESOME_6_DIR})

##############################
# System packages
##############################
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

file(GLOB_RECURSE PROJECT_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
)

list(REMOVE_ITEM PROJECT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Utilities/assetparser.cpp")

add_executable(idk_core main.cpp ${PROJECT_SOURCES})
add_executable(CoreAssetExporter src/Engine/Utilities/assetparser.cpp)

target_link_libraries(CoreAssetExporter PRIVATE imgui glad glfw z sqlite3)
target_compile_definitions(CoreAssetExporter PRIVATE ASSET_EXPORTER)

link_directories("${PROJECT_SOURCE_DIR}/cmake-build-debug")

set(IMGUI_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

# libData
set(LIBDATA_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/libdata)
add_library(DATA SHARED ${LIBDATA_PATH}/libData.cpp)

target_include_directories(DATA PUBLIC
        ${IMGUI_PATH}
        ${LIBDATA_PATH}
)

target_link_libraries(DATA
        PRIVATE
        Threads::Threads
        OpenGL::GL
        imgui
)

set(BOOST_LIBRARY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/boost/lib")
set(REQUIRED_DLLS
        "C:/msys64/clang64/bin/libclang_rt.asan_dynamic-x86_64.dll"
        "C:/msys64/clang64/bin/libc++.dll"
        # "C:/msys64/clang64/bin/libwinpthread-1.dll"
)

foreach(DLL ${REQUIRED_DLLS})
    add_custom_command(
            TARGET idk_core POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${DLL} $<TARGET_FILE_DIR:idk_core>
            COMMENT "Copying ${DLL} to build directory"
    )
endforeach()

add_subdirectory(external/glad)
add_subdirectory(external/glfw3)
add_subdirectory(external/boost)
add_subdirectory(external/glm)
add_subdirectory(external/imgui)
add_subdirectory(external/ImGuizmo)
add_subdirectory(external/sqlite3)
add_subdirectory(external/zlib-1.3.1)

target_include_directories(idk_core PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/external/stb_image
        ${CMAKE_CURRENT_SOURCE_DIR}/external/fontawesome
        ${CMAKE_CURRENT_SOURCE_DIR}/external/boost/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/glfw3/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external/imguizmo
        ${CMAKE_CURRENT_SOURCE_DIR}/external/zlib-1.3.1
        ${IMGUI_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rendering
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Rendering/Renderer
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Physics
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/SceneManagement
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Utilities
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/Lighting
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/ECS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Engine/ECS/Utils
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Editor/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Editor/Utilities
        ${LIBDATA_PATH}
)

target_link_libraries(idk_core PRIVATE glad glfw imgui ImGuizmo
        ${BOOST_LIBRARY_DIR}/libboost_filesystem.a
        ${BOOST_LIBRARY_DIR}/libboost_system.a
        opengl32
        Threads::Threads
        OpenGL::GL
        DATA
)
include_directories(${CMAKE_SOURCE_DIR}/external/pdf/)

if (WIN32)
    target_link_libraries(idk_core PRIVATE
            gdi32
            user32
            shell32
    )
endif()