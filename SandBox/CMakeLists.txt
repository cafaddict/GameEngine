project(Sandbox)



add_executable(SandBox SandBox.cpp)

# target_link_libraries(SandBox PUBLIC EDITOR ENGINE glfw TINY_OBJ_LOADER STB_IMAGE)
# target_include_directories(SandBox PUBLIC ../Engine ../EngineEditor ../external)
target_link_libraries(SandBox PUBLIC STB_IMAGE EDITOR ENGINE)
target_include_directories(SandBox PUBLIC ../EngineEditor)


# platform detection
if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
  set(OS "win")

  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

  # set(BUILD_SHARED_LIBS TRUE)
  if(${CMAKE_CL_64})
    set(ARCH "x64")
    set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    set_target_properties(SandBox
PROPERTIES 
RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR})
    target_compile_options(SandBox PRIVATE /W4)
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

  target_compile_options(SandBox PRIVATE -Wall -Wextra -Wpedantic)
elseif(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  set(OS "mac")
  set(ARCH "x86_64")
endif()