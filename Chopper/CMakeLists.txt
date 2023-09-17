find_package (spdlog CONFIG REQUIRED)

file (GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "*.h" "*.cpp")

add_library (Chopper "${SRC_FILES}")
target_link_libraries (Chopper PRIVATE spdlog::spdlog)

target_include_directories (Chopper PUBLIC "src")
target_compile_definitions (Chopper PRIVATE "BUILD_LIBS")

set_target_properties (Chopper PROPERTIES LINKER_LANGUAGE CXX)

