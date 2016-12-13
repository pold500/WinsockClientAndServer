cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
									# Если версия установленой программы
									# старее указаной, произайдёт аварийный выход.

project(WinsockClient)				# Название проекта

set(cpp_files 
WSAClient.cpp  WinsockClient.cpp  stdafx.cpp
)
SET(header_files
WSAClient.h  stdafx.h  targetver.h
)

set(cppcodec_dir "..\\cppcodec\\cppcodec\\")
set(tinyobj_header_only "..\\tinyobjloader-master\\")
set(helper_lib "..\\HelperLibrary\\")
set(cereal "..\\cereal-1.2.1\\include\\")

include_directories(${cppcodec_dir})			
include_directories(${tinyobj_header_only})
include_directories(${helper_lib})
include_directories(${boost_lib})
include_directories(${cereal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..\\")

add_executable(Client ${cpp_files} ${header_files})	

target_link_libraries(Client "..\\static_libs\\$(PlatformTarget)\\$(Configuration)\\helperslib.lib")