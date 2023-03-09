project(engine_editor)

# platform detection
if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
  set(OS "win")

  if(${CMAKE_CL_64})
    set(ARCH "x64")
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
elseif(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  set(OS "mac")
  set(ARCH "x86_64")
endif()

add_subdirectory(UI)

add_library(EDITOR Editor.hpp Editor.cpp)
target_link_libraries(EDITOR UI)
target_include_directories(EDITOR PUBLIC UI ../external)
