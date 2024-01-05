add_library(STB_IMAGE stb_image.h stb_image.cpp)
target_include_directories(STB_IMAGE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})