cmake_minimum_required(VERSION 2.8)

project(WinsockServerClient)

set(boost_lib "C:\\dev\\Libs\\boost_1_62_0\\")
set(cppcodec_dir "..\\cppcodec\\cppcodec\\")
set(tinyobj_header_only "..\\tinyobjloader-master\\")
set(cereal "..\\cereal-1.2.1\\include\\")

add_subdirectory("WinsockClient")
add_subdirectory("WinsockServer")
add_subdirectory("HelperLibrary")

