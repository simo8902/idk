set(SQLITE3_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../sqlite3)

add_library(sqlite3 STATIC ${SQLITE3_PATH}/sqlite3.c)

target_include_directories(sqlite3 PUBLIC
        ${SQLITE3_PATH}
)

set_target_properties(sqlite3 PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(idk_core PUBLIC sqlite3)