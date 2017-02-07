cmake_minimum_required(VERSION 3.0)
PROJECT (GLShell)

SET (CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/cmake
)

INCLUDE(copy_files)

FILE(GLOB GL_COMMON "GL/common/*.cpp" "GL/common/*.hpp")

SET(IMGUI_SOURCES
    imgui/imgui_impl_sdl_gl3.cpp
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_orient.cpp
)

SET(GL_SOURCES
    GL/gl3w.c
)

# Sources
SET(SOURCES
    ${GL_COMMON}
    ${IMGUI_SOURCES}
    ${GL_SOURCES}
    main.cpp
    common.h
    )

INCLUDE_DIRECTORIES(imgui
    sdl
    sdl/include
    glm
    GL/common
    GL
    .
    ) 

# Create the library
ADD_EXECUTABLE (${PROJECT_NAME} WIN32 ${SOURCES} ${LIBRARY_SOURCES})
TARGET_LINK_LIBRARIES(${PROJECT_NAME}

    ${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x86/SDL2.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/x86/SDL2main.lib
    opengl32.lib

)

copy_files(sdl/x86/*.dll ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

SOURCE_GROUP ("Shell" FILES ${SOURCES})
SOURCE_GROUP ("ImGui" FILES ${IMGUI_SOURCES})
SOURCE_GROUP ("GL" FILES ${GL_SOURCES} ${GL_COMMON})
