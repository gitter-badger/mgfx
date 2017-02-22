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

SET(APP_SOURCES
    app/renderUI.cpp
    app/renderUI.h
)

SET(GRAPHICS3D_DEVICE_SOURCES
    graphics3d/device/IDevice.h 
)

SET(GRAPHICS3D_DEVICEGL_SOURCES
    graphics3d/device/GL/deviceGL.cpp
    graphics3d/device/GL/deviceGL.h
    graphics3d/device/GL/gl3w.c
    graphics3d/device/GL/glcorearb.h
    graphics3d/device/GL/texture.cpp
    graphics3d/device/GL/texture.h
    graphics3d/device/GL/shader.cpp
    graphics3d/device/GL/shader.h
    graphics3d/device/GL/imgui_impl_sdl_gl3.cpp
    graphics3d/device/GL/imgui_impl_sdl_gl3.h
)

SET(IMGUI_SOURCES
    graphics2d/imgui/imgui.cpp
    graphics2d/imgui/imgui_draw.cpp
    graphics2d/imgui/imgui_demo.cpp
    graphics2d/imgui/imgui_orient.cpp
)

SET(GRAPHICS3D_CAMERA_SOURCES
    graphics3d/camera/camera.h
)

SET(GRAPHICS3D_GEOMETRY_SOURCES
    graphics3d/geometry/indexer.cpp
    graphics3d/geometry/indexer.h
    graphics3d/geometry/objloader.cpp
    graphics3d/geometry/objloader.h
    graphics3d/geometry/tangentspace.cpp
    graphics3d/geometry/tangentspace.h
)

SET(GRAPHICS3D_UI_SOURCES
    graphics3d/ui/manipulator.h
    graphics3d/ui/manipulator.cpp
)

SET(GRAPHICS3D_SCENE_SOURCES
    graphics3d/scene/scene.h
    graphics3d/scene/scene.cpp
    graphics3d/scene/mesh.cpp
    graphics3d/scene/mesh.h
    )

SET(HOST_SOURCES
    host/main.cpp
)

SET(COMMON_SOURCES
    common/common.h
    common/mathutils.h
    common/mathutils.cpp
    common/fileutils.cpp
    common/fileutils.h
)

FILE(GLOB_RECURSE ASSETS_SHADERS assets/*.fragmentshader assets/*.vertexshader)
FILE(GLOB_RECURSE ASSETS_TEXTURES assets/*.png assets/*.jpg assets/*.bmp)
#FILE(GLOB_RECURSE ASSETS_MODELS assets/models/*.obj)

SET(ASSETS
${ASSETS_TEXTURES}
${ASSETS_SHADERS}
${ASSETS_MODELS}
)

# Sources
SET(SOURCES
    ${COMMON_SOURCES}
    ${ASSETS}
    ${IMGUI_SOURCES}
    ${GRAPHICS3D_CAMERA_SOURCES}
    ${GRAPHICS3D_GEOMETRY_SOURCES}
    ${GRAPHICS3D_UI_SOURCES}
    ${GRAPHICS3D_GL_SOURCES}
    ${GRAPHICS3D_SCENE_SOURCES}
    ${GRAPHICS3D_DEVICEGL_SOURCES}
    ${GRAPHICS3D_DEVICE_SOURCES}
    ${HOST_SOURCES}
    ${APP_SOURCES}
    )


INCLUDE_DIRECTORIES(imgui
    lib/sdl
    lib/sdl/include
    lib/glm
    lib
    common
    graphics3d
    graphics3d/device
    graphics2d
    graphics2d/imgui
    .
    ) 

# Create the library
ADD_EXECUTABLE (${PROJECT_NAME} WIN32 ${SOURCES} ${LIBRARY_SOURCES})

TARGET_LINK_LIBRARIES (${PROJECT_NAME}
${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/${PROCESSOR_ARCH}/SDL2.lib
${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/${PROCESSOR_ARCH}/SDL2main.lib
opengl32.lib
)

copy_files(lib/sdl/${PROCESSOR_ARCH}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

add_custom_command(TARGET GLShell POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:GLShell>/assets)
SOURCE_GROUP(2D)
SOURCE_GROUP(3D)
SOURCE_GROUP (2D\\ImGui FILES ${IMGUI_SOURCES})
SOURCE_GROUP (3D\\Geometry FILES ${GRAPHICS3D_GEOMETRY_SOURCES})
SOURCE_GROUP (3D\\Camera FILES ${GRAPHICS3D_CAMERA_SOURCES})
SOURCE_GROUP (3D\\UI FILES ${GRAPHICS3D_UI_SOURCES})
SOURCE_GROUP (3D\\Scene FILES ${GRAPHICS3D_SCENE_SOURCES})
SOURCE_GROUP (3D\\Device FILES ${GRAPHICS3D_DEVICE_SOURCES})
SOURCE_GROUP (3D\\Device\\GL FILES ${GRAPHICS3D_DEVICEGL_SOURCES})
SOURCE_GROUP (Host FILES ${HOST_SOURCES})
SOURCE_GROUP (Assets\\Shaders FILES ${ASSETS_SHADERS})
SOURCE_GROUP (Assets\\Models FILES ${ASSETS_MODELS})
SOURCE_GROUP (Assets\\Textures FILES ${ASSETS_TEXTURES})
SOURCE_GROUP (App FILES ${APP_SOURCES})
SOURCE_GROUP (Common FILES ${COMMON_SOURCES})
