add_subdirectory(Common)
add_subdirectory(Platform)
add_subdirectory(Graphics)

add_library(Atreus SHARED)
set_target_properties(Atreus PROPERTIES PREFIX "")
target_compile_definitions(Atreus PRIVATE ATREUS_BUILDING)

target_link_libraries(
  Atreus
  PUBLIC
    Atreus.Common.Headers
    Atreus.Platform.Headers
    Atreus.Graphics.Headers
  PRIVATE
    Atreus.Common.Objects
    Atreus.Platform.Objects
    Atreus.Graphics.Objects
)