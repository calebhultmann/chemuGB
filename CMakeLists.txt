cmake_minimum_required(VERSION 3.30)

project(chemuGB VERSION 2.0 LANGUAGES CXX)

# Global Settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) #what does this do?

# Put binaries in build/<preset>/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# Dependencies
#  SDL3 - Graphics
include(FetchContent)

FetchContent_Declare(
	SDL3
	GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
	GIT_TAG release-3.4.8
)

FetchContent_MakeAvailable(SDL3)

#  OpenGL - ImGui backend
find_package(OpenGL REQUIRED)

# Imgui (Vendored)
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui)

add_library(imgui
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp

	${IMGUI_DIR}/backends/imgui_impl_sdl3.cpp
	${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

target_include_directories(imgui PUBLIC
	${IMGUI_DIR}
	${IMGUI_DIR}/backends
)

# Executable
add_executable(Emulator
	src/main.cpp
	# Other necessary build files in core
)

target_include_directories(Emulator PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Linking
target_link_libraries(Emulator PRIVATE
	SDL3::SDL3
	OpenGL::GL
	imgui
)

# Compiler Warnings
if(MSVC)
    target_compile_options(Emulator PRIVATE
        /W3
        /MP # multithreaded build
        /WX # warnings as errors
        /ZI # program database for edit and continue
    )
else()
    target_compile_options(Emulator PRIVATE 
        -Wall 
        -Wextra 
        -Wpedantic
    )
endif()

# VS nicety
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    PROPERTY VS_STARTUP_PROJECT Emulator
)