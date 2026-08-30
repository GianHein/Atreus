cmake_minimum_required(VERSION 3.23)

project(Atreus VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

add_subdirectory(Engine/Common)
add_subdirectory(Engine/Platform)

add_library(Atreus SHARED
  $<TARGET_OBJECTS:AtreusPlatform>
  Examples/Breakout/Source/Main.cpp
)

add_library(Atreus::Atreus ALIAS Atreus)

set_target_properties(Atreus PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)

target_link_libraries(Atreus
  PUBLIC
    AtreusCommon
    AtreusPlatform
)

include(CMakePackageConfigHelpers)

set(ATREUS_INSTALL_CMAKEDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Atreus)

install(TARGETS Atreus AtreusCommon AtreusPlatform
  EXPORT AtreusTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT AtreusTargets
  FILE AtreusTargets.cmake
  NAMESPACE Atreus::
  DESTINATION ${ATREUS_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/AtreusConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  CMake/AtreusConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/install/AtreusConfig.cmake
  INSTALL_DESTINATION ${ATREUS_INSTALL_CMAKEDIR}
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/install/AtreusConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/AtreusConfigVersion.cmake
  DESTINATION ${ATREUS_INSTALL_CMAKEDIR}
)

add_subdirectory(Examples/Breakout)