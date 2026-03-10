file(GLOB_RECURSE ATREUS_PLATFORM_WINDOWS_SOURCES CONFIGURE_DEPENDS
  "Private/*.cpp"
)
add_library(AtreusPlatformWindows STATIC PRIVATE ${ATREUS_PLATFORM_WINDOWS_SOURCES})

target_include_directories(AtreusPlatformWindows
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Private>
)

set_target_properties(AtreusPlatformWindows PROPERTIES FOLDER "Atreus/Host/Platform")
target_link_libraries(AtreusPlatformWindows PRIVATE AtreusBase AtreusPlatform)