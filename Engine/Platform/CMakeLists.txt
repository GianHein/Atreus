if (WIN32)
  set(platform_dir Win32)
elseif(UNIX)
  set(platform_dir Unix)
endif()

file(GLOB_RECURSE PLATFORM_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Private/${platform_dir}/Source/*.cpp")

add_library(Atreus.Platform.Headers INTERFACE)
target_include_directories(Atreus.Platform.Headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Public")
target_link_libraries(Atreus.Platform.Headers INTERFACE Atreus.Common.Headers)

add_library(Atreus.Platform.Objects OBJECT ${PLATFORM_SOURCES})
target_link_libraries(Atreus.Platform.Objects PUBLIC Atreus.Platform.Headers)
target_compile_definitions(Atreus.Platform.Objects PRIVATE ATREUS_BUILDING)

if (WIN32)
  target_link_libraries(Atreus.Platform.Objects PRIVATE user32 dbghelp)
endif()