include_directories(../include)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
file(GLOB SRC "*.hpp" "*.cpp")
file(GLOB_RECURSE GLAD "../../../external/glad/*")
add_library(CORE ${SRC} ${GLAD})
target_link_libraries(CORE glfw UTILS PHYSICS EVENTS COMMON_RENDERING VULKAN_RENDERING OPENGL_RENDERING)
target_include_directories(CORE PUBLIC ../include ../../../external ../../../external/glad/include)

# target_compile_options(CORE PRIVATE -Wall -Wextra -Wpedantic -Werror)

# platform detection
if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
  set(OS "win")

  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

  # set(BUILD_SHARED_LIBS TRUE)
  if(${CMAKE_CL_64})
    set(ARCH "x64")
    set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

    # target_compile_options(CORE PRIVATE /W4 /WX)
    target_compile_options(CORE PRIVATE /W4)
  else()
    set(ARCH "Win32")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES Linux)
  set(OS "lin")

  if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(ARCH "x86_64")
  else()
    set(ARCH "i686")
  endif()

  target_compile_options(CORE PRIVATE -Wall -Wextra -Wpedantic)

# target_compile_options(CORE PRIVATE -Wall -Wextra -Wpedantic -Werror)
elseif(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  set(OS "mac")
  set(ARCH "x86_64")
endif()
