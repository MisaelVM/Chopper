cmake_minimum_required (VERSION 3.18)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED true)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (CHOPPER_RENDERER_BACKEND STREQUAL "Vulkan")
	list (APPEND VCPKG_MANIFEST_FEATURES "vulkan-backend")
endif()

project ("Chopper")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

option (BUILD_SHARED_LIBS "Building shared libraries" ON)

add_subdirectory ("Chopper")
add_subdirectory ("Testbed")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
	target_compile_definitions (Chopper PRIVATE "DEBUG_BUILD")
	target_compile_definitions (Testbed PRIVATE "DEBUG_BUILD")
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
	target_compile_definitions (Chopper PRIVATE "RELEASE_BUILD")
	target_compile_definitions (Testbed PRIVATE "RELEASE_BUILD")
endif()
