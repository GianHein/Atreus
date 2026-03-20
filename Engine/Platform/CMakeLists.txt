file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/PlatformStub.cpp
"// Auto-generated stub file for AtreusPlatform. This file is required to create a valid static library, but it does not contain any actual implementation.\n"
)

add_library(AtreusPlatform STATIC ${CMAKE_CURRENT_BINARY_DIR}/PlatformStub.cpp)

target_include_directories(AtreusPlatform
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
)

add_subdirectory(Windows)

set_target_properties(AtreusPlatform PROPERTIES FOLDER "Atreus/Platform")
target_link_libraries(AtreusPlatform PUBLIC AtreusBase AtreusPlatformWindows)