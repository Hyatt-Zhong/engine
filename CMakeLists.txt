#1.cmake verson，指定cmake版本
cmake_minimum_required(VERSION 3.2)

#2.project name，指定项目的名称，一般和项目的文件夹名称对应
project(xBBee)
set(CMAKE_CXX_STANDARD 17)
#3.head file path，头文件目录
include_directories(inc)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/out)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/out)

#set(EXECUTABLE_OUTPUT_PATH_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/out)
#set(EXECUTABLE_OUTPUT_PATH_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/out)

set(CMAKE_PDB_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/pdb)
set(CMAKE_PDB_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/pdb)

set(INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/inc)

set(BOOST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/boostlib)



# 添加 子目录
#add_subdirectory(jsoncpp)
add_subdirectory(engine)
add_subdirectory(test)
add_subdirectory(BBee)
add_subdirectory(modulebuilder)
add_subdirectory(jsoncpp)

