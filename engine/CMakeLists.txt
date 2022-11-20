project(xEngine)

set (CMAKE_CXX_STANDARD 17)

include_directories(../inc/SDL)
include_directories(../box2d)
link_directories(../lib/SDL/x86)
link_directories(../lib/SDL_IMG/x86)
link_directories(../lib/box2d)

file(GLOB SRC_FILE *.*)
add_library(xEngine ${SRC_FILE})
target_link_libraries(xEngine SDL2.lib)
target_link_libraries(xEngine SDL2_image.lib)

#set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Box2D)
#file(GLOB_RECURSE SRC_DIR_LIST ${SRC_DIR}/*.cpp ${SRC_DIR}/*h)	#遍历获取

target_link_libraries(xEngine debug box2d_d.lib)#debug 配置debug版本 box2d_d
target_link_libraries(xEngine optimized box2d.lib)#release 配置release版本 box2d
