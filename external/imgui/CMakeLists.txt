add_library(imgui STATIC
        ${CMAKE_CURRENT_LIST_DIR}/imgui.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui_demo.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui_draw.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui_tables.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imgui_widgets.cpp
        ${CMAKE_CURRENT_LIST_DIR}/backends/imgui_impl_glfw.cpp
        ${CMAKE_CURRENT_LIST_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_CURRENT_LIST_DIR}/backends
)

target_link_libraries(imgui PUBLIC glfw)