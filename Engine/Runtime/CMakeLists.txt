file(GLOB_RECURSE RUNTIME_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp")

add_library(Atreus.Runtime.Headers INTERFACE)
target_include_directories(Atreus.Runtime.Headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Public")
target_link_libraries(
  Atreus.Runtime.Headers
  INTERFACE
  Atreus.Common.Headers
  Atreus.Platform.Headers
  Atreus.Core.Headers
  Atreus.Graphics.Headers
)
# add_library(Atreus.Runtime.Objects OBJECT ${RUNTIME_SOURCES})
# target_link_libraries(Atreus.Runtime.Objects PUBLIC Atreus.Runtime.Headers PRIVATE Tracy::TracyClient)
# target_compile_definitions(Atreus.Runtime.Objects PRIVATE ATREUS_BUILDING ATREUS_PROFILING)