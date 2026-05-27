file(GLOB_RECURSE ATREUS_CORE_HEADERS CONFIGURE_DEPENDS "Public/*.h")
file(GLOB_RECURSE ATREUS_CORE_SOURCES CONFIGURE_DEPENDS "Private/*.cpp")

add_library(AtreusCore SHARED ${ATREUS_CORE_HEADERS} ${ATREUS_CORE_SOURCES})

# Needed so the launcher can find symbols in the library, as ATR_API only
# exports symbols when ATR_EXPORT is defined.
target_compile_definitions(AtreusCore PRIVATE ATR_EXPORT)

target_include_directories(AtreusCore
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
    $<INSTALL_INTERFACE:Public>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Private>
)

set_target_properties(AtreusCore PROPERTIES FOLDER "Atreus")
target_link_libraries(AtreusCore PRIVATE AtreusCommon AtreusPlatform)