#ifndef __C_SERVER_HEADER__
#define __C_SERVER_HEADER__
#include "Helpers.h"
#include <vector>
#include <map>
#include <string>
#include "tiny_obj_loader.h"
#include "GeometryStructures.h"

class WSASocketC;

namespace tinyobj
{
	struct attrib_t;
}

class UserCommand;



class Server
{
	using thread_ptr = Helpers::custom_deleter_unique_ptr<std::thread>;
	std::set<int> m_clientTokensSet;
	std::vector<thread_ptr> m_clientProcessingThreads;
	Helpers::Socket<SOCKET> m_listenSocket;
	WSASocketC* m_wsaSocket;
	


	//where we store obj files
	std::vector<std::pair<std::string, std::string>> m_objectFilesPathNamePairs;
	//map [name] => object file data
	ObjFileDataMap m_objFilesMap;
	ObjFileDataMap_v2 m_objFileMap_v2;
	void processClientFunction(const SOCKET ClientSocket, const int threadClientToken);
	int  createClientToken(std::set<int>& client_tokens);

	
	std::unique_ptr<ObjFileData>    loadAndParseObjectFile(const char * filename, const char * basepath = "", bool triangulate = true);
	std::unique_ptr<ObjFileData_v2> loadAndParseObjectFile_v2(const char * filename, const char * basepath = "", bool triangulate = true);
	void loadObjFiles();
	
	std::unique_ptr<UserCommand> parseUserCmd(const std::string& user_input, const SOCKET socket);

	//std::vector<Server::ObjFileData> getPolygonsForModel(const std::string& obj_file_name, const bool isInterval, const std::vector<int>& polygon_numbers_list) const;
	std::vector<std::string> getFileNames() const;
	std::vector<size_t> getPolyCount() const;
	mutable std::vector<std::string> m_fileNames;
	mutable std::vector<size_t> m_filePolyCount;
public:
	Server(WSASocketC* socketClass);
	void Update();


};

#endif
