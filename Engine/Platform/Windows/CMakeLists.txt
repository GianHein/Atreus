file(GLOB_RECURSE ATREUS_PLATFORM_WINDOWS_SOURCES CONFIGURE_DEPENDS "Source/*.cpp")
add_library(AtreusPlatformWindows STATIC ${ATREUS_PLATFORM_WINDOWS_SOURCES})

target_include_directories(AtreusPlatformWindows
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Source>
)

set_target_properties(AtreusPlatformWindows PROPERTIES FOLDER "Atreus/Platform")
target_link_libraries(AtreusPlatformWindows PRIVATE AtreusCommon AtreusPlatform)