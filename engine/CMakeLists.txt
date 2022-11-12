project(xEngine)

include_directories(../inc/SDL)
include_directories(../box2d)
link_directories(../lib/SDL/x86)
link_directories(../lib/SDL_IMG/x86)
link_directories(../lib/box2d)

file(GLOB SRC_FILE *.*)
add_library(xEngine ${SRC_FILE})
target_link_libraries(xEngine SDL2.lib)
target_link_libraries(xEngine SDL2_image.lib)

target_link_libraries(xEngine debug box2d_d.lib)#debug ≈‰÷√debug∞Ê±æ box2d_d
target_link_libraries(xEngine optimized box2d.lib)#release ≈‰÷√release∞Ê±æ box2d
