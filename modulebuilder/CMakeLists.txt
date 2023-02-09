project(nativelib)

include_directories(../engine)

#link_directories(../lib/SDL/x86)
#link_directories(../lib/SDL_IMG/x86)
#link_directories(../lib/box2d)
#要编译64位就链接64位的库
link_directories(../lib/SDL/x64)
link_directories(../lib/SDL_IMG/x64)
link_directories(../lib/box2d/x64)

link_directories(../lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include)

file(GLOB SRC_FILE *.cpp)
add_library(nativelib SHARED ${SRC_FILE})

target_link_libraries(nativelib xEngine)

target_link_libraries(nativelib debug box2d_d.lib)
target_link_libraries(nativelib optimized box2d.lib)
target_link_libraries(nativelib jsoncpp.lib)