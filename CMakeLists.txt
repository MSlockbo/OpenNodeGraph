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
find_package(assimp REQUIRED)
find_package(stb REQUIRED)
find_package(Freetype REQUIRED)
find_package(GLEW REQUIRED)
find_package(glm REQUIRED)
find_package(OpenGL REQUIRED COMPONENTS OpenGL)
find_package(SDL2 REQUIRED)
find_package(RapidJSON REQUIRED)

if(MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

include_directories(Include)
include_directories(External)

# Add External Libraries
add_subdirectory(External/portable-file-dialogs)
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
        Source/Core/Window.cpp      Include/Core/Window.h
        Source/Core/Console.cpp     Include/Core/Console.h
        Source/Core/EventSystem.cpp Include/Core/EventSystem.h
        Source/Core/Engine.cpp      Include/Core/Engine.h
        Source/Renderer/Renderer.cpp Include/Renderer/Renderer.h

        # Editor
        Include/Editor/MainMenuBar.h
        Include/Editor/EditorWindow.h
        Source/Editor/EditorSystem.cpp  Include/Editor/EditorSystem.h
        Source/Editor/EditorWindow.cpp  Include/Editor/EditorWindow.h
        Source/Editor/ConsoleWindow.cpp Include/Editor/ConsoleWindow.h
        Source/Editor/Profiler.cpp      Include/Editor/Profiler.h

        # File System
        Source/FileSystem/FileManager.cpp Include/FileSystem/FileManager.h

        # Assets
        Source/Project/Project.cpp         Include/Project/Project.h
        Source/Renderer/Assets/Texture.cpp Include/Renderer/Assets/Texture.h

        # Graph
        Source/Graph/ShaderGraph.cpp Include/Graph/ShaderGraph.h

        # Nodes
        Source/Graph/Nodes/Shaders.cpp           Include/Graph/Nodes/Shaders.h
        Source/Graph/Nodes/Math/Constants.cpp    Include/Graph/Nodes/Math/Constants.h
        Source/Graph/Nodes/Math/Functions.cpp    Include/Graph/Nodes/Math/Functions.h
        Source/Graph/Nodes/Math/Comparison.cpp   Include/Graph/Nodes/Math/Comparison.h
        Source/Graph/Nodes/Math/Trigonometry.cpp Include/Graph/Nodes/Math/Trigonometry.h
        Source/Graph/Nodes/Math/Vector.cpp       Include/Graph/Nodes/Math/Vector.h
        
        # Utilities
        Include/Utility/Timer.h
        Include/Graph/Nodes/Math/Common.h
        Source/Graph/Nodes/Math/Common.cpp
)

# Preprocessor Definitions
target_compile_definitions(OpenShaderDesigner PRIVATE
        PROJECT_VERSION="${PROJECT_VERSION}"
        PROJECT_VERSION_MAJOR=${VERSION_MAJOR}
        PROJECT_VERSION_MINOR=${VERSION_MINOR}
        PROJECT_VERSION_PATCH=${VERSION_PATCH}
        PROJECT_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(OpenShaderDesigner PRIVATE
        Freetype::Freetype
        GLEW::GLEW
        OpenGL::GL
        ${SDL2_LIBRARIES}
        assimp::assimp
        rapidjson
        open-cpp-utils
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