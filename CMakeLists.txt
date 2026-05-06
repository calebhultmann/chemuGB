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

target_link_libraries(imgui PUBLIC
	SDL3::SDL3
)

# Executable
add_executable(Emulator
	src/main.cpp
	src/chemuPixelEngine.cpp
	src/chemuPixelEngine.h
	src/errors.h
	src/bus.cpp
	src/bus.h
	src/cartridge.cpp
	src/cartridge.h
	src/cpu.cpp
	src/cpu.h
	src/emulator.cpp
	src/emulator.h
	src/joypad.cpp
	src/joypad.h
	src/ppu.cpp
	src/ppu.h
	
	# Mappers
	src/mapper.cpp
	src/mapper.h
	src/mapper_NO_MBC.cpp
	src/mapper_NO_MBC.h
	src/mapper_MBC1.cpp
	src/mapper_MBC1.h
)


# Linking
target_link_libraries(Emulator PRIVATE
	SDL3::SDL3
	OpenGL::GL
	imgui
)

target_include_directories(Emulator PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/src
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