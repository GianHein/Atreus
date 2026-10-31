# Capture the script path at file scope. Inside a function,
# CMAKE_CURRENT_LIST_DIR would point to the *caller's* directory.
set(ATREUS_EMBED_SCRIPT "${CMAKE_CURRENT_LIST_DIR}/EmbedFile.cmake")

function(atreus_embed_shaders target)
  set(gen_root "${CMAKE_CURRENT_BINARY_DIR}/generated")
  set(outputs "")

  foreach(shader IN LISTS ARGN)
    get_filename_component(abs  "${shader}" ABSOLUTE)
    get_filename_component(name "${shader}" NAME)        # quad.vertex.glsl
    string(MAKE_C_IDENTIFIER "${name}" symbol)           # quad_vertex_glsl
    set(out "${gen_root}/Shaders/${name}.h")

    add_custom_command(
      OUTPUT  "${out}"
      COMMAND ${CMAKE_COMMAND}
              -DINPUT=${abs} -DOUTPUT=${out} -DSYMBOL=${symbol}
              -P ${ATREUS_EMBED_SCRIPT}
      DEPENDS "${abs}" "${ATREUS_EMBED_SCRIPT}"
      COMMENT "Embedding shader ${name}"
      VERBATIM
    )
    list(APPEND outputs "${out}")
  endforeach()

  # Listing the headers as sources makes the target depend on them,
  # so they are generated before anything that includes them compiles.
  target_sources(${target} PRIVATE ${outputs})
  target_include_directories(${target} PRIVATE "${gen_root}")
endfunction()