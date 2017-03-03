# Set system vars

if (CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(LINUX TRUE)
endif(CMAKE_SYSTEM_NAME MATCHES "Linux")

if (CMAKE_SYSTEM_NAME MATCHES "Darwin")
	set (OSX TRUE)
endif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
