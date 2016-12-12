cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
									# Если версия установленой программы
									# старее указаной, произайдёт аварийный выход.

project(HelpersLib)				# Название проекта

set(cpp_files 
Helpers.cpp  stdafx.cpp
)
SET(header_files
GeometryStructures.h  Helpers.h  LogStream.h  stdafx.h  targetver.h
)

set(cppcodec_dir "..\\cppcodec\\cppcodec\\")
set(tinyobj_header_only "..\\tinyobjloader-master\\")
set(boost_lib "C:\\dev\\Libs\\boost_1_62_0\\")
set(cereal "..\\cereal-1.2.1\\include\\")

include_directories(${cppcodec_dir})			
include_directories(${tinyobj_header_only})
include_directories(${boost_lib})
include_directories(${cereal})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "..\\static_libs\\$(PlatformTarget)\\")

add_library(HelpersLib STATIC ${cpp_files} ${header_files})	
