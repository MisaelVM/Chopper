add_executable (Testbed "src/main.cpp")

target_include_directories (Testbed PUBLIC "src" "${PROJECT_SOURCE_DIR}/Chopper/src")
target_link_libraries (Testbed PUBLIC Chopper)
