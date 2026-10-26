if (WIN32)
  add_library(glad STATIC
    src/gl.c
    src/wgl.c
  )
elseif(UNIX)
  add_library(glad STATIC
    src/gl.c
    src/glx.c
  )
endif()

target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)