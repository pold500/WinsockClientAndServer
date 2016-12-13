cmake_minimum_required(VERSION 2.8) # Проверка версии CMake.
									# Если версия установленой программы
									# старее указаной, произайдёт аварийный выход.

project(WinsockServer)				# Название проекта


set(cpp_files 
GetGeometrySendProperFormat.cpp  SendLargeFileCmd.cpp  UserCommandNoOperation.cpp  stdafx.cpp
GetVertexesCommand.cpp           Server.cpp            WSASocket.cpp
ListCommand.cpp                  UserCommand.cpp       WinsockServerMain.cpp
)
SET(header_files
CmdFabrique.h                  ListCommand.h       UserCommand.h             stdafx.h
GetGeometrySendProperFormat.h  SendLargeFileCmd.h  UserCommandNoOperation.h  targetver.h
GetVertexesCommand.h           Server.h            WSASocket.h
)

set(cppcodec_dir "..\\cppcodec\\cppcodec\\")
set(tinyobj_header_only "..\\tinyobjloader-master\\")
set(helper_lib "..\\HelperLibrary\\")
set(boost_lib "C:\\dev\\Libs\\boost_1_62_0\\")
set(cereal "..\\cereal-1.2.1\\include\\")

include_directories(${cppcodec_dir})			
include_directories(${tinyobj_header_only})
include_directories(${helper_lib})
include_directories(${boost_lib})
include_directories(${cereal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "..\\")

add_executable(Server ${cpp_files} ${header_files})	

target_link_libraries(Server "..\\static_libs\\$(PlatformTarget)\\$(Configuration)\\helperslib.lib")

ADD_CUSTOM_COMMAND(TARGET Server
          POST_BUILD
		  COMMAND echo ON
		  COMMAND echo %cd%
		  COMMAND cd
		  COMMAND echo copy "..\\models\\" "$(SolutionDir)\\$(Configuration)\\models"
          COMMAND XCOPY /E "..\\models\\*.*" "$(SolutionDir)\\$(Configuration)\\models\\" /y
		  )

