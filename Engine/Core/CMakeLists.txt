file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp")

add_library(Atreus.Core.Headers INTERFACE)
target_include_directories(Atreus.Core.Headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Public")
target_link_libraries(Atreus.Core.Headers INTERFACE Atreus.Common.Headers Atreus.Platform.Headers)

add_library(Atreus.Core.Objects OBJECT ${CORE_SOURCES})
target_link_libraries(Atreus.Core.Objects PUBLIC Atreus.Core.Headers PRIVATE Tracy::TracyClient)
target_compile_definitions(Atreus.Core.Objects PRIVATE ATREUS_BUILDING ATREUS_PROFILING)