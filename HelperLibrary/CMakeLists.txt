cmake_minimum_required(VERSION 2.8) # �������� ������ CMake.
									# ���� ������ ������������ ���������
									# ������ ��������, ��������� ��������� �����.

project(HelpersLib)				# �������� �������

SET(cpp_files 
Helpers.cpp  stdafx.cpp
LogStream.cpp
)
SET(header_files
GeometryStructures.h  Helpers.h  LogStream.h  stdafx.h  targetver.h
)



include_directories(${cppcodec_dir})			
include_directories(${tinyobj_header_only})
include_directories(${boost_lib})
include_directories(${cereal})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "..\\static_libs\\$(PlatformTarget)\\")

add_library(HelpersLib STATIC ${cpp_files} ${header_files})	
