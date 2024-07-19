cmake_minimum_required(VERSION 3.5)


set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# Initialize the project
project(OpenShaderDesigner VERSION ${PROJECT_VERSION})

# Set CPP Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

# Setup Build Folder
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_SYSTEM_NAME})

find_package(PkgConfig REQUIRED)

find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(SDL2 REQUIRED)

if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

include_directories(Include)
include_directories(External)

add_executable(OpenShaderDesigner
        Source/Entry.cpp

        # Core
        Source/Core/Window.cpp
        Source/Core/Console.cpp
        Source/Core/EventSystem.cpp
        Source/Core/Engine.cpp
        Source/Core/Renderer.cpp

        # Editor
        Source/Editor/EditorSystem.cpp
        Source/Editor/EditorWindow.cpp
        Source/Editor/ConsoleWindow.cpp
        Source/Editor/Profiler.cpp

        # Graph
        Source/Graph/ShaderGraph.cpp

        # Nodes
        Source/Graph/Nodes/Math.cpp

        # ImGui
        External/imgui-docking/imgui_demo.cpp
        External/imgui-docking/imgui_draw.cpp
        External/imgui-docking/imgui_tables.cpp
        External/imgui-docking/imgui.cpp
        External/imgui-docking/imgui_widgets.cpp
        External/imgui-docking/backends/imgui_impl_sdl2.cpp
        External/imgui-docking/backends/imgui_impl_opengl3.cpp
        External/imgui-docking/misc/cpp/imgui_stdlib.cpp
        External/imgui-docking/misc/freetype/imgui_freetype.cpp
        Include/OpenGL/BufferObject.h
)


target_link_libraries(OpenShaderDesigner PRIVATE
        Freetype::Freetype
        GLEW::GLEW
        OpenGL::GL
        ${SDL2_LIBRARIES}
)