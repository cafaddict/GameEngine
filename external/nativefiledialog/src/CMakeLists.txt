include_directories(../include)

set(SRC)
list(APPEND SRC ../include/nfd.h common.h nfd_common.c nfd_common.h)

if(APPLE)
    # macOS-specific files
    list(APPEND SRC nfd_cocoa.m)
elseif(UNIX)
    # Linux-specific files
    list(APPEND SRC nfd_linux.c)
elseif(WIN32)
    # Windows-specific files
    list(APPEND SRC nfd_win.c)
else()
    message(FATAL_ERROR "Unsupported platform!")
endif()

add_library(nativefiledialog ${SRC})
target_include_directories(nativefiledialog PUBLIC ../include)

if(APPLE)
    target_link_libraries(nativefiledialog PRIVATE "-framework Cocoa")
endif()