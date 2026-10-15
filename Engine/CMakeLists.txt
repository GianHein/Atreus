add_subdirectory(Common)
add_subdirectory(Platform)
# add_subdirectory(Core)

add_library(Atreus SHARED Engine.cpp)
target_compile_definitions(Atreus PRIVATE ATREUS_BUILDING)

target_link_libraries(Atreus PRIVATE Atreus.Common.Objects)