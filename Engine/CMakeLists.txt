add_subdirectory(Common)
add_subdirectory(Platform)
# add_subdirectory(Core)

add_library(Atreus SHARED)
target_compile_definitions(Atreus PRIVATE ATREUS_BUILDING)

target_link_libraries(Atreus PUBLIC Atreus.Common.Headers Atreus.Platform.Headers PRIVATE Atreus.Common.Objects Atreus.Platform.Objects)