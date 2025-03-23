set(GLAD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(GLAD_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/src/glad.c)

add_library(glad ${GLAD_SOURCE})
target_include_directories(glad PUBLIC ${GLAD_INCLUDE_DIR})
