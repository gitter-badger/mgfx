MACRO(ADD_MSVC_PRECOMPILED_HEADER)
    SET(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} /YuCommon.h")
    set_source_files_properties(Common/Common.cpp
        PROPERTIES
        COMPILE_FLAGS "/YcCommon.h"
        )
ENDMACRO(ADD_MSVC_PRECOMPILED_HEADER)
