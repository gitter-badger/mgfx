cmake_minimum_required(VERSION 3.0)
PROJECT (GLShell)

SET (CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/cmake
)

INCLUDE(copy_files)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
  set(PROCESSOR_ARCH "x64")
else()
  set(ARCH_64 FALSE)
  set(PROCESSOR_ARCH "x86")
endif()

FILE(GLOB GL_COMMON "graphics3d/GL/*.cpp" "graphics3d/GL/*.hpp")

SET(IMGUI_SOURCES
    graphics2d/imgui/imgui.cpp
    graphics2d/imgui/imgui_draw.cpp
    graphics2d/imgui/imgui_demo.cpp
    graphics2d/imgui/imgui_orient.cpp
)

SET(GL_SOURCES
    graphics3d/GL/gl3w.c
    graphics2d/GL/imgui_impl_sdl_gl3.cpp
    graphics2d/GL/imgui_impl_sdl_gl3.h
    graphics3d/camera.h
    graphics3d/manipulator.h
)

# Sources
SET(SOURCES
    ${GL_COMMON}
    ${IMGUI_SOURCES}
    ${GL_SOURCES}
    glrender.cpp
    glrender.h
    main.cpp
    common.h
    )

INCLUDE_DIRECTORIES(imgui
    sdl
    sdl/include
    glm
    graphics3d/GL
    graphics3d
    graphics2d
    graphics2d/imgui
    .
    ) 

# Create the library
ADD_EXECUTABLE (${PROJECT_NAME} WIN32 ${SOURCES} ${LIBRARY_SOURCES})

TARGET_LINK_LIBRARIES (${PROJECT_NAME}

${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/${PROCESSOR_ARCH}/SDL2.lib
${CMAKE_CURRENT_SOURCE_DIR}/sdl/lib/${PROCESSOR_ARCH}/SDL2main.lib
opengl32.lib

)

copy_files(sdl/${PROCESSOR_ARCH}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_files(graphics3d/models/*.* ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_files(graphics3d/textures/*.* ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_files(graphics3d/GL/shaders/*.* ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

SOURCE_GROUP ("Shell" FILES ${SOURCES})
SOURCE_GROUP ("ImGui" FILES ${IMGUI_SOURCES})
SOURCE_GROUP ("3D" FILES ${GL_SOURCES} ${GL_COMMON})
