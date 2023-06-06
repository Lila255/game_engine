cmake_minimum_required(VERSION 3.10)
project(game)

# Set C++ standard to latest
set(CMAKE_CXX_STANDARD 20)

# Set vcpkg root directory
set(VCPKG_ROOT "$ENV{VCPKG_ROOT}")

# Set vcpkg toolchain file
if(DEFINED ENV{VCPKG_ROOT})
		set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
endif()

set(VCPKG_TARGET_TRIPLET "x64-windows")
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
# Use vcpkg to find packages
find_package(glfw3 CONFIG REQUIRED)
include_directories(${GLFW_INCLUDE_DIRS})
find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})
find_package(poly2tri CONFIG REQUIRED)
include_directories(${POLY2TRI_INCLUDE_DIRS})
find_package(box2d CONFIG REQUIRED)



# Set compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest /Zi /EHsc /nologo")

# Set source files
file(GLOB SOURCES src/*.cpp)

# Build executable
add_executable(game ${SOURCES})
target_link_libraries(game PRIVATE glfw GLEW::glew opengl32 poly2tri::poly2tri box2d::box2d)
# target_link_libraries(game glfw3 glew opengl32)

# print the build type
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
# if in debug mode, copy debug dlls
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
	add_custom_command(TARGET game POST_BUILD
										 COMMAND ${CMAKE_COMMAND} -E copy_if_different
										 "${VCPKG_ROOT}/installed/x64-windows/debug/bin/glew32d.dll"
										 "$<TARGET_FILE_DIR:game>"
										 DEPENDS game)
	add_custom_command(TARGET game POST_BUILD
										 COMMAND ${CMAKE_COMMAND} -E copy_if_different
										 "${VCPKG_ROOT}/installed/x64-windows/debug/bin/glfw3.dll"
										 "$<TARGET_FILE_DIR:game>"
										 DEPENDS game)
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
	add_custom_command(TARGET game POST_BUILD
										 COMMAND ${CMAKE_COMMAND} -E copy_if_different
										 "${VCPKG_ROOT}/installed/x64-windows/bin/glew32.dll"
										 "$<TARGET_FILE_DIR:game>"
										 DEPENDS game)
	add_custom_command(TARGET game POST_BUILD
										 COMMAND ${CMAKE_COMMAND} -E copy_if_different
										 "${VCPKG_ROOT}/installed/x64-windows/bin/glfw3.dll"
										 "$<TARGET_FILE_DIR:game>"
										 DEPENDS game)

	add_custom_command(TARGET game POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
		# project root directory
		"${CMAKE_SOURCE_DIR}/../../src/normal_vectors.txt"
		# "../../src/normal_vectors.txt"
		"$<TARGET_FILE_DIR:game>"
		DEPENDS game)

endif()

