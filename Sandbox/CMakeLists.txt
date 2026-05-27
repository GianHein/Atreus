file(GLOB_RECURSE SANDBOX_FILES CONFIGURE_DEPENDS "Source/*.cpp")

add_executable(Sandbox ${SANDBOX_FILES})
target_link_libraries(Sandbox PRIVATE AtreusCommon)