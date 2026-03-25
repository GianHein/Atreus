file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/RuntimeStub.cpp
"// Auto-generated stub file for AtreusRuntime. This file is required to create a valid shared library, but it does not contain any actual implementation.\n"
)

add_library(AtreusRuntime SHARED ${CMAKE_CURRENT_BINARY_DIR}/RuntimeStub.cpp)

set_target_properties(AtreusRuntime PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
)

add_subdirectory(Core)

if (MSVC)
  target_link_libraries(AtreusRuntime PRIVATE AtreusCore)
  target_link_options(AtreusRuntime PRIVATE "/WHOLEARCHIVE:$<TARGET_FILE_NAME:AtreusCore>")
else()
  target_link_libraries(AtreusRuntime PRIVATE -Wl,--whole-archive AtreusCore -Wl,--no-whole-archive)
endif()