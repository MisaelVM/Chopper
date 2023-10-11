find_package (spdlog CONFIG REQUIRED)
find_package (glfw3 CONFIG REQUIRED)

file (GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS "*.h" "*.cpp")

add_library (Chopper "${SRC_FILES}")
target_link_libraries (Chopper PRIVATE spdlog::spdlog)
target_link_libraries (Chopper PRIVATE ${GLFW_LIBRARIES})

target_include_directories (Chopper PUBLIC "src")
target_compile_definitions (Chopper PRIVATE "BUILD_LIBS")

if (CHOPPER_RENDERER_BACKEND STREQUAL "Vulkan")
	find_package (Vulkan REQUIRED)
	target_link_libraries (Chopper PRIVATE Vulkan::Vulkan)
	target_include_directories (Chopper PRIVATE "$ENV{VULKAN_SDK}/include")
endif()

set_target_properties (Chopper PROPERTIES LINKER_LANGUAGE CXX)
