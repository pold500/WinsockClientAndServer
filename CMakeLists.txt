cmake_minimum_required(VERSION 2.8)

project(WinsockServerClient)

set(boost_lib "C:\\dev\\Libs\\boost_1_62_0\\")

add_subdirectory("WinsockClient")
add_subdirectory("WinsockServer")
add_subdirectory("HelperLibrary")

