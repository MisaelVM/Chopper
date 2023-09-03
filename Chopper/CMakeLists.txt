add_library (Chopper "src/common/definitions.h" "src/Chopper.h"    "src/core/Application.h" "src/core/Application.cpp" "src/core/EntryPoint.h")

target_include_directories (Chopper PUBLIC "src")
target_compile_definitions (Chopper PRIVATE "BUILD_LIBS")

set_target_properties (Chopper PROPERTIES LINKER_LANGUAGE CXX)
