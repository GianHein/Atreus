file(GLOB_RECURSE BREAKOUT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp")

add_library(Breakout MODULE ${BREAKOUT_SOURCES})
set_target_properties(Breakout PROPERTIES PREFIX "")
target_link_libraries(Breakout PUBLIC Atreus)