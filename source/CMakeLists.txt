cmake_minimum_required(VERSION 3.10)
project(mtree_gist C)

set(POSTGRESQL_INCLUDE_DIR "/usr/include/postgresql/15/server")
set(POSTGRESQL_EXTENSION_DIR "/usr/share/postgresql/15/extension")
set(POSTGRESQL_LIBRARY_DIR "/usr/share/postgresql/15/postgre")

set(FILES
    "mtree_text"
    "mtree_text_util"
    "mtree_text_array"
    "mtree_text_array_util"
    "mtree_int32"
    "mtree_int32_util"
    "mtree_int32_array"
    "mtree_int32_array_util"
    "mtree_float"
    "mtree_float_util"
    "mtree_float_array"
    "mtree_float_array_util"
    "mtree_util"
    "mtree_gist"
)

foreach(FILE ${FILES})
    list(APPEND OBJECT_FILES "${CMAKE_SOURCE_DIR}/${FILE}.c")
endforeach()

set(CMAKE_SHARED_LIBRARY_PREFIX "")
add_library(mtree_gist SHARED ${OBJECT_FILES})
set_target_properties(mtree_gist PROPERTIES
    LIBRARY_OUTPUT_NAME "mtree_gist"
    LIBRARY_SUFFIX ".so"
)
target_include_directories(mtree_gist PRIVATE ${POSTGRESQL_INCLUDE_DIR})

install(TARGETS mtree_gist LIBRARY DESTINATION ${POSTGRESQL_LIBRARY_DIR})

install(FILES
    "${CMAKE_SOURCE_DIR}/mtree_gist--1.0.sql"
    DESTINATION ${POSTGRESQL_EXTENSION_DIR}
)

add_custom_command(
    OUTPUT "${CMAKE_SOURCE_DIR}/mtree_gist_tmp.control"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/mtree_gist--1.0.sql" "${POSTGRESQL_EXTENSION_DIR}/mtree_gist--1.0.sql"
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/mtree_gist.control" "${CMAKE_SOURCE_DIR}/mtree_gist_tmp.control"
    COMMAND sed -i 's,%libdir%,'"${POSTGRESQL_LIBRARY_DIR}"',g' ${CMAKE_SOURCE_DIR}/mtree_gist_tmp.control
    COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/mtree_gist_tmp.control" "${POSTGRESQL_EXTENSION_DIR}/mtree_gist.control"
    COMMAND ${CMAKE_COMMAND} -E rm -f "${CMAKE_SOURCE_DIR}/mtree_gist_tmp.control"
    DEPENDS "${CMAKE_SOURCE_DIR}/mtree_gist.control" "${CMAKE_SOURCE_DIR}/mtree_gist--1.0.sql"
)

add_custom_target(copy_sql_and_control ALL DEPENDS "${CMAKE_SOURCE_DIR}/mtree_gist_tmp.control")