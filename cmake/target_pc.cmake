SET(PLATFORM_LINKLIBS
    opengl32.lib
    winmm.lib   # SDL - sound, etc.
    version.lib # SDL - windows keyboard
    imm32.lib   # SDL - windows keyboard
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/${PROCESSOR_ARCH}/SDL2.lib
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/sdl/lib/${PROCESSOR_ARCH}/SDL2main.lib
)
