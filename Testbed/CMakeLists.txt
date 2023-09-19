find_package (spdlog CONFIG REQUIRED)
find_package (glfw3 CONFIG REQUIRED)

file (GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "*.h" "*.cpp")

add_executable (Testbed "${SRC_FILES}")

target_include_directories (Testbed PUBLIC "src" "${PROJECT_SOURCE_DIR}/Chopper/src")
target_link_libraries (Testbed PUBLIC Chopper)
target_link_libraries (Testbed PRIVATE spdlog::spdlog_header_only)
target_link_libraries (Chopper PRIVATE glfw)
