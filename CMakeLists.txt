cmake_minimum_required(VERSION 3.11)

# set the project name and version
project(VulkanEngine)
set (ENGINE_VERSION_MAJOR 1)
set (ENGINE_VERSION_MINOR 0)

# platform detection
if (${CMAKE_SYSTEM_NAME} MATCHES Windows)
  set (OS "win")

  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  # set(BUILD_SHARED_LIBS TRUE)
  if (${CMAKE_CL_64})
    set (ARCH "x64")
    set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
    # add_compile_options(/W4 /WX)
  else ()
    set (ARCH "Win32")
  endif ()
elseif (${CMAKE_SYSTEM_NAME} MATCHES Linux)
  set (OS "lin")
  if (${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set (ARCH "x86_64")
  else ()
    set (ARCH "i686")
  endif ()
elseif (${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  set (OS "mac")
  set (ARCH "x86_64")
endif ()



# specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
# SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")





add_subdirectory(Engine)
# add_subdirectory(EngineShell)
# add_subdirectory(SandBox)
# add_subdirectory(ScriptingLanguage)