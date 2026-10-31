include(AtreusShaders)

if (WIN32)
  set(platform_dir Win32)
elseif(UNIX)
  set(platform_dir Unix)
endif()

file(GLOB GRAPHICS_SOURCES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/Private/OpenGL/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/Private/OpenGL/${platform_dir}/*.cpp"
)

add_library(Atreus.Graphics.Headers INTERFACE)
target_include_directories(Atreus.Graphics.Headers INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/Public")
target_link_libraries(Atreus.Graphics.Headers INTERFACE Atreus.Common.Headers Atreus.Platform.Headers Atreus.Core.Headers)

add_library(Atreus.Graphics.Objects OBJECT ${GRAPHICS_SOURCES})
target_include_directories(Atreus.Graphics.Objects PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Private/OpenGL")
target_link_libraries(Atreus.Graphics.Objects PUBLIC Atreus.Graphics.Headers PRIVATE Tracy::TracyClient)
target_compile_definitions(Atreus.Graphics.Objects PRIVATE ATREUS_BUILDING ATREUS_PROFILING)

atreus_embed_shaders(Atreus.Graphics.Objects
  "${CMAKE_CURRENT_SOURCE_DIR}/Private/OpenGL/Shaders/quad.vertex.glsl"
  "${CMAKE_CURRENT_SOURCE_DIR}/Private/OpenGL/Shaders/quad.fragment.glsl"
)

if (WIN32)
  target_link_libraries(Atreus.Graphics.Objects PRIVATE opengl32 gdi32 $<BUILD_INTERFACE:glad>)
elseif(UNIX)
  find_package(X11 REQUIRED)
  find_package(OpenGL REQUIRED COMPONENTS OpenGL GLX)
  target_link_libraries(Atreus.Graphics.Objects PRIVATE X11 OpenGL::OpenGL OpenGL::GLX $<BUILD_INTERFACE:glad>)
endif()