cmake_minimum_required(VERSION 3.5)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 0)
set(VERSION_PATCH 1)
set(PROJECT_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

project(imgui-docking VERSION ${PROJECT_VERSION})


# Required Source Files
set(IMGUI_SOURCES imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp)
set(IMGUI_HEADERS imconfig.h imgui.h imgui_internal.h imstb_rectpack.h imstb_textedit.h imstb_truetype.h)

set(IMGUI_DEPENDENCIES)

# Backends

# Allegro ==============================================================================================================

if(IMGUI_BACKEND_ALLEGRO_5)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_allegro5.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_allegro5.h)
endif()


# Android ==============================================================================================================

if(IMGUI_BACKEND_ANDROID)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_android.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_android.h)
endif()


# DirectX ==============================================================================================================

if(IMGUI_BACKEND_DIRECTX_9)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_dx9.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_dx9.h)
endif()

if(IMGUI_BACKEND_DIRECTX_10)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_dx10.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_dx10.h)
endif()

if(IMGUI_BACKEND_DIRECTX_11)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_dx11.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_dx11.h)
endif()

if(IMGUI_BACKEND_DIRECTX_12)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_dx12.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_dx12.h)
endif()


# GLFW =================================================================================================================

if(IMGUI_BACKEND_GLFW)
    find_package(glfw3 REQUIRED)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_glfw.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_glfw.h)
    list(APPEND IMGUI_DEPENDENCIES glfw)
endif()


# GLUT =================================================================================================================

if(IMGUI_BACKEND_GLUT)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_glut.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_glut.h)
endif()


# METAL ================================================================================================================

if(IMGUI_BACKEND_METAL)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_metal.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_metal.h)
endif()


# OPENGL ===============================================================================================================

if(IMGUI_BACKEND_OPENGL)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    list(APPEND IMGUI_DEPENDENCIES OpenGL::GL)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_opengl3.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_opengl3.h backends/imgui_impl_opengl3_loader.h)
endif()

if(IMGUI_BACKEND_OPENGL_LEGACY)
    find_package(OpenGL REQUIRED COMPONENTS OpenGL)
    list(APPEND IMGUI_DEPENDENCIES OpenGL::GL)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_opengl2.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_opengl2.h)
endif()


# SDL ==================================================================================================================

if(IMGUI_BACKEND_SDL2)
    find_package(SDL2 REQUIRED)
    list(APPEND IMGUI_DEPENDENCIES ${SDL2_LIBRARIES})
    list(APPEND IMGUI_SOURCES backends/imgui_impl_sdl2.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_sdl2.h)

    if(IMGUI_BACKEND_SDL_RENDERER)
        list(APPEND IMGUI_SOURCES backends/imgui_impl_sdlrenderer2.cpp)
        list(APPEND IMGUI_HEADERS backends/imgui_impl_sdlrenderer2.h)
    endif ()
endif()

if(IMGUI_BACKEND_SDL3)
    find_package(SDL3 REQUIRED)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_sdl3.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_sdl3.h)
    list(APPEND IMGUI_DEPENDENCIES SDL3)

    if(IMGUI_BACKEND_SDL_RENDERER)
        list(APPEND IMGUI_SOURCES backends/imgui_impl_sdlrenderer3.cpp)
        list(APPEND IMGUI_HEADERS backends/imgui_impl_sdlrenderer3.h)
    endif ()
endif()


# VULKAN ===============================================================================================================

if(IMGUI_BACKEND_VULKAN)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_vulkan.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_vulkan.h)
endif()


# WGPU =================================================================================================================

if(IMGUI_BACKEND_WGPU)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_wgpu.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_wgpu.h)
endif()


# WIN32 ================================================================================================================

if(IMGUI_BACKEND_WIN32)
    list(APPEND IMGUI_SOURCES backends/imgui_impl_win32.cpp)
    list(APPEND IMGUI_HEADERS backends/imgui_impl_win32.h)
endif()


# Misc

if(IMGUI_STDLIB)
    list(APPEND IMGUI_SOURCES misc/cpp/imgui_stdlib.cpp)
    list(APPEND IMGUI_HEADERS misc/cpp/imgui_stdlib.h)
endif()

if(IMGUI_FREETYPE)
    find_package(Freetype REQUIRED)
    list(APPEND IMGUI_DEPENDENCIES Freetype::Freetype)
    list(APPEND IMGUI_SOURCES misc/freetype/imgui_freetype.cpp)
    list(APPEND IMGUI_HEADERS misc/freetype/imgui_freetype.h)
endif()

# Library

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_library(imgui-docking STATIC ${IMGUI_HEADERS} ${IMGUI_SOURCES})
target_link_libraries(imgui-docking PRIVATE ${IMGUI_DEPENDENCIES})