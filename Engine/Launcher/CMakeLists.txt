file(GLOB_RECURSE ATREUS_LAUNCHER_FILES CONFIGURE_DEPENDS "Source/*.cpp")

add_executable(Launcher ${ATREUS_LAUNCHER_FILES})
target_link_libraries(Launcher PRIVATE AtreusCommon AtreusPlatform)