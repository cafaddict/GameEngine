cmake_minimum_required(VERSION 3.13)

# set the project name and version
project(VulkanEngine)
set(ENGINE_VERSION_MAJOR 1)
set(ENGINE_VERSION_MINOR 0)

# platform detection
if(${CMAKE_SYSTEM_NAME} MATCHES Windows)
  set(OS "win")

  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /FORCE:MULTIPLE") 

  # set(BUILD_SHARED_LIBS TRUE)
  if(${CMAKE_CL_64})
    set(ARCH "x64")
    set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
  else()
    set(ARCH "Win32")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES Linux)
  set(OS "lin")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--allow-multiple-definition")


  if(${CMAKE_SIZEOF_VOID_P} EQUAL 8)
    set(ARCH "x86_64")
  else()
    set(ARCH "i686")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES Darwin)
  set(OS "mac")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl")
    add_compile_options(-Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-deprecated-copy-with-user-provided-copy -Wno-deprecated-copy -Wno-strict-prototypes)
  # set(ARCH "x86_64")
# Set linker flags to use the GNU linker and enable the --allow-multiple-definition option
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=ld -Wl,-z,muldefs")
endif()

# specify the C++ standard
# SET(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory("external/glfw-3.3.4")
add_subdirectory("external/glm")
add_subdirectory("external/stb_image")
add_subdirectory("external/tinyobjloader")
add_subdirectory("external/nativefiledialog")
add_subdirectory("external/assimp")
add_subdirectory(Engine)
add_subdirectory(SandBox)
add_subdirectory(SandBoxRefac)
add_subdirectory(ParticleSandBox)

# add_subdirectory(ScriptingLanguage)

# # Download and unpack googletest at configure time
# configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
# execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
#   RESULT_VARIABLE result
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)

# if(result)
#   message(FATAL_ERROR "CMake step for googletest failed: ${result}")
# endif()

# execute_process(COMMAND ${CMAKE_COMMAND} --build .
#   RESULT_VARIABLE result
#   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download)

# if(result)
#   message(FATAL_ERROR "Build step for googletest failed: ${result}")
# endif()

# # Prevent overriding the parent project's compiler/linker
# # settings on Windows
# set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# # Add googletest directly to our build. This defines
# # the gtest and gtest_main targets.
# add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
#   ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
#   EXCLUDE_FROM_ALL)

# # The gtest/gtest_main targets carry header search path
# # dependencies automatically when using CMake 2.8.11 or
# # later. Otherwise we have to add them here ourselves.
# if(CMAKE_VERSION VERSION_LESS 2.8.11)
#   include_directories("${gtest_SOURCE_DIR}/include")
# endif()

include_directories(external)
include_directories(Engine)
include_directories(EngineEditor)
include_directories(SandBox)
include_directories(ParticleSandBox)
