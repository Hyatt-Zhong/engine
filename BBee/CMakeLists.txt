project(BBee)

set (CMAKE_CXX_STANDARD 17)

include_directories(../engine)

link_directories(../lib/SDL/x86)
link_directories(../lib/SDL_IMG/x86)
link_directories(../lib/box2d)

file(GLOB SRC_FILE *.*)

add_executable(BBee ${SRC_FILE})
target_link_libraries(BBee xEngine)

target_link_libraries(BBee debug box2d_d.lib)
target_link_libraries(BBee optimized box2d.lib)
