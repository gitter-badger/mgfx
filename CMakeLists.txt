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

SET(GRAPHICS3D_GL_SOURCES
    graphics3d/GL/gl3w.c
    graphics2d/GL/imgui_impl_sdl_gl3.cpp
    graphics2d/GL/imgui_impl_sdl_gl3.h
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
)

SET(HOST_SOURCES
    main.cpp
)

# Sources
SET(SOURCES
    ${GL_COMMON}
    ${IMGUI_SOURCES}
    ${GRAPHICS3D_CAMERA_SOURCES}
    ${GRAPHICS3D_GEOMETRY_SOURCES}
    ${GRAPHICS3D_UI_SOURCES}
    ${GRAPHICS3D_GL_SOURCES}
    ${HOST_SOURCES}
    glrender.cpp
    glrender.h
    common.h
    )

INCLUDE_DIRECTORIES(imgui
    lib/sdl
    lib/sdl/include
    lib/glm
    graphics3d
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
copy_files(assets/models/*.* ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_files(assets/textures/*.* ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )
copy_files(assets/GL/shaders/*.* ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration) )

SOURCE_GROUP ("2D/ImGui" FILES ${IMGUI_SOURCES})
SOURCE_GROUP ("3D/Geometry" FILES ${GRAPHICS3D_GEOMETRY_SOURCES})
SOURCE_GROUP ("3D/Camera" FILES ${GRAPHICS3D_CAMERA_SOURCES})
SOURCE_GROUP ("3D/UI" FILES ${GRAPHICS3D_UI_SOURCES})
SOURCE_GROUP ("3D/GL" FILES ${GRAPHICS3D_UI_SOURCES})
SOURCE_GROUP ("Host" FILES ${HOST_SOURCES})
