project(engine)

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

add_subdirectory(Utils)
add_subdirectory(Rendering)
add_subdirectory(Events)
add_subdirectory(Core)

# add_subdirectory(Physics)
# add_subdirectory(Events)
add_library(ENGINE Engine.hpp Engine.cpp)
target_link_libraries(ENGINE UTILS RENDERING EVENTS CORE)
target_include_directories(ENGINE PUBLIC Core Events Physics Rendering Utils ../external)
