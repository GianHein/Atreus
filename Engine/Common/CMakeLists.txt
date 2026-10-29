file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp")

add_library(Atreus.Common.Headers INTERFACE)
target_include_directories(Atreus.Common.Headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Public")

add_library(Atreus.Common.Objects OBJECT ${COMMON_SOURCES})
target_link_libraries(Atreus.Common.Objects PUBLIC Atreus.Common.Headers PRIVATE Tracy::TracyClient)
target_compile_definitions(Atreus.Common.Objects PRIVATE ATREUS_BUILDING ATREUS_PROFILING)