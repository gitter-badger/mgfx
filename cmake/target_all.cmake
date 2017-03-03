INCLUDE(define_platform_defaults)
INCLUDE(define_compiler_flags)
INCLUDE(ensure_outofsource)

MACRO_ENSURE_OUT_OF_SOURCE_BUILD("${PROJECT_NAME} requires an out of source build. Please create a separate build directory and run 'cmake /path/to/${PROJECT_NAME} [options]' there.")

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_64 TRUE)
  set(PROCESSOR_ARCH "x64")
else()
  set(ARCH_64 FALSE)
  set(PROCESSOR_ARCH "x86")
endif()

message(STATUS "System: ${CMAKE_SYSTEM}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Arch: ${PROCESSOR_ARCH}")

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    message(STATUS "TARGET_PC")
    set(TARGET_PC 1)
    INCLUDE(target_pc)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    message(STATUS "TARGET_MAC")
    INCLUDE(target_linux)
endif()

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    message(STATUS "TARGET_LINUX")
    INCLUDE(target_linux)
endif()

# global needed variables
SET (APPLICATION_NAME ${PROJECT_NAME})
SET (APPLICATION_VERSION_MAJOR "0")
SET (APPLICATION_VERSION_MINOR "1")
SET (APPLICATION_VERSION_PATCH "0")
SET (APPLICATION_VERSION "${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}")

