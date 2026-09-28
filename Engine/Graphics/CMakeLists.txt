add_library(AtreusGraphics OBJECT)
file(GLOB_RECURSE ATREUS_GRAPHICS_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Public/*.h)
target_sources(AtreusGraphics
  PUBLIC
    FILE_SET HEADERS
    BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/Public
    FILES ${ATREUS_GRAPHICS_HEADERS}
)

target_link_libraries(AtreusGraphics PUBLIC AtreusCommon AtreusPlatform)

target_compile_definitions(AtreusGraphics PRIVATE ATR_EXPORT)

file(GLOB_RECURSE ATREUS_GRAPHICS_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.cpp)
file(GLOB_RECURSE ATREUS_GRAPHICS_PRIVATE_HEADERS CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Private/*.h)

target_sources(AtreusGraphics PRIVATE ${ATREUS_GRAPHICS_SOURCES} ${ATREUS_GRAPHICS_PRIVATE_HEADERS})
target_include_directories(AtreusGraphics PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Private)

if(WIN32)
  set(ATREUS_GRAPHICS_BACKEND_DIR ${CMAKE_SOURCE_DIR}/Platforms/Win32/Graphics)
else()
  message(FATAL_ERROR "Atreus: AtreusGraphics has no backend implemented for this target platform")
endif()

file(GLOB ATREUS_GRAPHICS_BACKEND_SOURCES CONFIGURE_DEPENDS ${ATREUS_GRAPHICS_BACKEND_DIR}/Source/*.cpp)
file(GLOB ATREUS_GRAPHICS_BACKEND_PRIVATE_HEADERS CONFIGURE_DEPENDS ${ATREUS_GRAPHICS_BACKEND_DIR}/Source/*.h)

target_sources(AtreusGraphics PRIVATE ${ATREUS_GRAPHICS_BACKEND_SOURCES} ${ATREUS_GRAPHICS_BACKEND_PRIVATE_HEADERS})
target_include_directories(AtreusGraphics PRIVATE ${ATREUS_GRAPHICS_BACKEND_DIR}/Source)

if(WIN32)
  target_link_libraries(AtreusGraphics PUBLIC user32 opengl32 glad)
endif()