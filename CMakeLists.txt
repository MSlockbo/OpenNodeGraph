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

# Find dependenices
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

# Add External Libraries
add_subdirectory(External/open-cpp-utils)
add_subdirectory(External/glw)

# Configure ImGui
set(IMGUI_BACKEND_SDL2 ON)
set(IMGUI_BACKEND_OPENGL ON)
set(IMGUI_STDLIB ON)
set(IMGUI_FREETYPE ON)

# Add ImGui and any extensions
add_subdirectory(External/imgui-docking)
add_subdirectory(External/imgui-extras)
add_subdirectory(External/imnode-graph)

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
)

target_link_libraries(OpenShaderDesigner PRIVATE
        Freetype::Freetype
        GLEW::GLEW
        OpenGL::GL
        ${SDL2_LIBRARIES}
        imgui-docking
        imgui-extras
        imnode-graph
        glw
)

# DOXYGEN ==============================================================================================================
# https://vicrucann.github.io/tutorials/quick-cmake-doxygen/

find_package(Doxygen)

if(DOXYGEN_FOUND)
    get_filename_component(DOXYGEN_PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)
    set(DOXYGEN_CONFIG_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
    set(DOXYGEN_CONFIG_OUT ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile)

    configure_file(${DOXYGEN_CONFIG_IN} ${DOXYGEN_CONFIG_OUT} @ONLY)
    message("Doxygen Build Started.")

    if(WIN32)
        add_custom_target(OpenShaderDesigner-Documentation ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_OUT}
                COMMAND start firefox "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/html/index.html"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating Doxygen Documentation"
                VERBATIM)
    else()
        add_custom_target(OpenShaderDesigner-Documentation ALL
                COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_CONFIG_OUT}
                COMMAND firefox "${CMAKE_CURRENT_SOURCE_DIR}/Documentation/html/index.html"
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMENT "Generating Doxygen Documentation"
                VERBATIM)
    endif()
else()
    message("Doxygen not found.")
endif()