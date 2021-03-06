cmake_minimum_required(VERSION 3.1)

set (CMAKE_CXX_STANDARD 11)

PROJECT (GLShell)

SET (CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_LIST_DIR}/cmake
)

INCLUDE(target_all)
INCLUDE(copy_files)
#INCLUDE(cotire)

SET(APP_SOURCES
    app/settings.cpp
    app/settings.h
)

SET(GRAPHICS3D_DEVICE_SOURCES
    graphics3d/device/IDevice.h 
)

SET(GRAPHICS3D_DEVICEGL_SOURCES
    graphics3d/device/GL/deviceGL.cpp
    graphics3d/device/GL/deviceGL.h
    graphics3d/device/GL/quadGL.cpp
    graphics3d/device/GL/quadGL.h
    graphics3d/device/GL/bufferGL.cpp
    graphics3d/device/GL/bufferGL.h
    graphics3d/device/GL/gl3w.c
    graphics3d/device/GL/glcorearb.h
    graphics3d/device/GL/shader.cpp
    graphics3d/device/GL/shader.h
    graphics3d/device/GL/imguisdl_gl3.cpp
    graphics3d/device/GL/imguisdl_gl3.h
)

SET(GRAPHICS2D_IMGUI_SOURCES
    graphics2d/imgui/imgui.cpp
    graphics2d/imgui/imgui_draw.cpp
    graphics2d/imgui/imgui_demo.cpp
    graphics2d/imgui/imgui_orient.cpp
)

SET(GRAPHICS2D_UI_SOURCES
    graphics2d/ui/windowmanager.cpp
    graphics2d/ui/windowmanager.h
    graphics2d/ui/window.cpp
    graphics2d/ui/window.h
)

SET(GRAPHICS3D_CAMERA_SOURCES
    graphics3d/camera/camera.h
    graphics3d/camera/camera.cpp
)

SET(GRAPHICS3D_GEOMETRY_SOURCES
    graphics3d/geometry/indexer.cpp
    graphics3d/geometry/indexer.h
    graphics3d/geometry/vertex_spacial_sort.cpp
    graphics3d/geometry/vertex_spacial_sort.h
    graphics3d/geometry/mesh.cpp
    graphics3d/geometry/mesh.h
)

SET(GRAPHICS3D_UI_SOURCES
    graphics3d/ui/manipulator.h
    graphics3d/ui/manipulator.cpp
)

SET(GRAPHICS3D_SCENE_SOURCES
    graphics3d/scene/scene.h
    graphics3d/scene/scene.cpp
    )

SET(HOST_SOURCES
    host/main.cpp
)

SET(COMMON_SOURCES
    common/common.h
    common/common.cpp
    common/mathutils.h
    common/mathutils.cpp
    common/fileutils.cpp
    common/fileutils.h
    common/timer.cpp
    common/timer.h

    lib/easylogging/src/easylogging++.cc
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
    ${GRAPHICS2D_IMGUI_SOURCES}
    ${GRAPHICS2D_UI_SOURCES}
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
if (TARGET_PC)
ADD_EXECUTABLE (${PROJECT_NAME} WIN32 ${SOURCES} ${LIBRARY_SOURCES})
else ()
ADD_EXECUTABLE (${PROJECT_NAME} WIN32 ${SOURCES} ${LIBRARY_SOURCES})
endif()

TARGET_LINK_LIBRARIES (${PROJECT_NAME} ${PLATFORM_LINKLIBS})

copy_files(lib/sdl/${PROCESSOR_ARCH}/*.dll ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_files(logger.conf ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

add_custom_target(copy-assets ALL
    COMMAND cmake -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:GLShell>/assets
    DEPENDS GLShell)

SOURCE_GROUP(2D)
SOURCE_GROUP(3D)
SOURCE_GROUP (2D\\ImGui FILES ${GRAPHICS2D_IMGUI_SOURCES})
SOURCE_GROUP (2D\\UI FILES ${GRAPHICS2D_UI_SOURCES})
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
