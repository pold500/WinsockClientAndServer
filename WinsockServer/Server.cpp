#include "stdafx.h"
#pragma warning( push )
#pragma warning( disable : 4101)
#pragma warning( disable : 4003)

#include "Server.h"
#include "WSASocket.h"
#include "Helpers.h"
#include "tiny_obj_loader.h"
#include "UserCommand.h"
#include <array>
#include "GeometryStructures.h"
#include "CmdFabrique.h"

namespace fs = std::experimental::filesystem;
using namespace Helpers;

const int DEFAULT_BUFLEN = 2 << 9;


Server::Server(WSASocketC* wsaSocket):
	m_wsaSocket(wsaSocket),
	m_listenSocket(wsaSocket->createListenSocket())
{
	loadObjFiles();
}

std::vector<std::string> Server::getFileNames() const
{
	if (m_fileNames.empty())
	{
		m_fileNames.reserve(m_objectFilesPathNamePairs.size());
		std::for_each(begin(m_objectFilesPathNamePairs), end(m_objectFilesPathNamePairs),
			[&](const decltype(m_objectFilesPathNamePairs[0])& _pair)
		{
			m_fileNames.push_back(_pair.second);
		});
	}
	return m_fileNames;
}

std::vector<size_t> Server::getPolyCount() const
{
	if (m_filePolyCount.empty())
	{
		m_filePolyCount.reserve(m_objFilesMap.size());
		for (const auto& object :  m_objFilesMap)
		{
			m_filePolyCount.push_back(object.second->m_polygons.size());
		}
	}
	return m_filePolyCount;
}


std::unique_ptr<UserCommand> Server::parseUserCmd(const std::string& user_input, const SOCKET socket)
{
		
	if (user_input.empty())
		return createListFilesCommand(socket, getFileNames(), getPolyCount());

	std::vector<std::string> cmd_tokens = Helpers::split(user_input, " ");
	if (cmd_tokens[0] == "getlargefile")
	{
		return getLargeFileCmd(socket);
	}
	if (cmd_tokens[0] == "list")
	{
		return createListFilesCommand(socket, getFileNames(), getPolyCount());
	}
	if (cmd_tokens[0] == "get")
	{
		const size_t cmd_params_count = 4;
		if (cmd_tokens.size() == cmd_params_count && cmd_tokens[1] == "polygons")
		{
			auto polygonCmd = Helpers::parsePolygonCmd(cmd_tokens);
			if (polygonCmd.is_initialized())
			{
				return createSendGeometryCommand(socket, m_objFilesMap, cmd_tokens[2], *polygonCmd);
			}
		}
		else if(cmd_tokens[1] == "polygons_v2" && !cmd_tokens[2].empty())
		{
			auto polygonCmd = Helpers::parsePolygonCmd(cmd_tokens);
			auto object = m_objFileMap_v2.find(cmd_tokens[2]);
			if ( object != m_objFileMap_v2.end() && polygonCmd.is_initialized())
			{
				return createSendGeometryCommand_v2(socket, (*object).second.get(), *polygonCmd);
			}
		}
	}
	

	return createSendCommandListCommand(socket, getFileNames());
}

void Server::processClientFunction(const SOCKET _clientSocket, const int threadClientToken)
{
	console_log << "starting client thread, client token is: " << threadClientToken << "\n";
	//worker thread
	Helpers::Socket<SOCKET> ClientSocket(_clientSocket);

	while (true)
	{
		//Analyze command
		try 
		{
			const auto client_request = Helpers::receivePacket(ClientSocket.getSocket());
			if (client_request.get())
			{
				const CStringPacket* stringPacket = static_cast<const CStringPacket*>(client_request.get());
				auto user_command = parseUserCmd(stringPacket->GetStringData(), ClientSocket.getSocket());
				user_command->execute();
			}
			else
			{
				//terminate thread
				console_log << "Client thread listen cycle terminated. \n";
				break;
			}
		}
		catch (std::exception& ex)
		{
			console_log << "processClientFunction thread caught exception: " << ex.what() << "\n";
			break;
		}
	}
}

void Server::Update()
{
	console_log << "Starting to accept connections from clients... \n";
	
	SOCKET ClientSocket = accept(m_listenSocket.getSocket(), NULL, NULL); //blocking operation

	if (m_listenSocket.getSocket() == INVALID_SOCKET)
	{
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(m_listenSocket);
		//re-init the listening socket and start listening again
		while((m_listenSocket = m_wsaSocket->createListenSocket()).getSocket() == INVALID_SOCKET);
		
	}

	console_log << "We got a connecting client!... \n";
	auto threadFunc = [ClientSocket, this]() { this->processClientFunction(ClientSocket, createClientToken(m_clientTokensSet)); };
	auto func_thread_deleter = [](std::thread* thread) { thread->detach(); };
	m_clientProcessingThreads.push_back(thread_ptr(new std::thread(threadFunc), func_thread_deleter));
}

std::vector<Polygon3D> GetPolygonsFromShapes(const std::vector<tinyobj::shape_t>& shapesVector, 
											 const tinyobj::attrib_t& attrib)
{
	std::vector<Polygon3D> polygonVector;
	size_t total_number_of_polygons = 0;
	
	for (const auto& shape : shapesVector)
	{
		const size_t polygons_count_in_shape = shape.mesh.num_face_vertices.size();
		
		//polygonVector.reserve(polygons_count_in_shape);
		const size_t vertices_count_in_polygon = 3; //always 3 for triangulated model. and we have triangulated.
		for (size_t polyId = 0; polyId < polygons_count_in_shape; polyId++)
		{
			Polygon3D polygon(total_number_of_polygons + polyId);
			for (size_t i = 0; i < vertices_count_in_polygon; i++)
			{
				Point3D<float> vertex;
				vertex.x = attrib.vertices[shape.mesh.indices[polyId].vertex_index];
				vertex.y = attrib.vertices[shape.mesh.indices[polyId + 1].vertex_index];
				vertex.z = attrib.vertices[shape.mesh.indices[polyId + 2].vertex_index];
				polygon.m_polygonVertices[i] = vertex;
			}
			polygonVector.push_back(polygon);
		}
		total_number_of_polygons += polygons_count_in_shape;
	}
	return polygonVector;
}

int Server::createClientToken(std::set<int>& client_tokens)
{
	const int max_clients = 1000;
	auto getNewToken = [&max_clients]()->int { return rand() % max_clients; };
	int newToken = getNewToken();
	while (client_tokens.count(newToken))
	{
		newToken = getNewToken();
	}
	client_tokens.insert(end(client_tokens), newToken);
	return newToken;
}

std::unique_ptr<ObjFileData_v2> Server::loadAndParseObjectFile_v2(const char * filename, const char * basepath /*= ""*/, bool triangulate /*= true*/)
{
	console_log << "Loading obj file: " << filename << "\n";

	std::string err;
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &err, filename,
		basepath, triangulate);


	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return nullptr;
	}
	auto objData_v2 = std::make_unique<ObjFileData_v2>();
	const size_t coordsPerVertex = 3;
	
	for (size_t i = 0; i < attributes.vertices.size(); i+= coordsPerVertex)
	{
		objData_v2->m_vertices.push_back(Point3D<float>(attributes.vertices[i], attributes.vertices[i + 1], attributes.vertices[i + 2]));
	}
	for (const auto& shape : shapes)
	{
		size_t faces_read = 0;
		for (size_t i = 0; i < shape.mesh.indices.size();)
		{
			const size_t indexesPerFace = shape.mesh.num_face_vertices[faces_read];
			std::vector<int> face_indexes;
			for (size_t j = 0; j < indexesPerFace; j++)
			{
				face_indexes.push_back(shape.mesh.indices[i+j].vertex_index + 1);
			}
			i += indexesPerFace;
			faces_read++;
			objData_v2->m_faces.push_back(face_indexes);
			
		}
	}
		
	return objData_v2;
}

std::unique_ptr<ObjFileData> Server::loadAndParseObjectFile(const char * filename,  const char * basepath /*= ""*/, bool triangulate /*= true*/)
{
	
	console_log << "Loading obj file: " << filename << "\n";
		

	std::string err;
	tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	bool ret = tinyobj::LoadObj(&attributes, &shapes, &materials, &err, filename,
		basepath, triangulate);


	if (!err.empty()) {
		std::cerr << err << std::endl;
	}

	if (!ret) {
		printf("Failed to load/parse .obj.\n");
		return nullptr;
	}

	if (ret)
	{
		auto polygons = GetPolygonsFromShapes(shapes, attributes);
		fs::path p(filename);
		
		auto objFileData = std::make_unique<ObjFileData>(p.filename().string(), polygons);
		return objFileData;
	}
	return nullptr;
	//PrintInfo(attributes, shapes, materials);
}

void Server::loadObjFiles()
{
	const auto loadPath = fs::current_path().append(fs::path("/models"));
	
	//List *.obj file paths and store them
	if (fs::exists(loadPath) && fs::is_directory(loadPath))
	{
		fs::directory_iterator end_iter;
		for (fs::directory_iterator dir_iter(loadPath); dir_iter != end_iter; ++dir_iter)
		{
			if (fs::is_regular_file(dir_iter->status()))
			{
				console_log << (*dir_iter).path().string() << "\n";
				m_objectFilesPathNamePairs.push_back(std::make_pair((*dir_iter).path().string(),
					(*dir_iter).path().filename().string()));
			}
		}
	}
	else
	{
		console_log << "Path to obj files is wrong! Terminating. Path searched was: " << loadPath.string() << "\n";
		std::getchar();
		std::exit(-1);
	}
	//Then load them and parse their geometry
	for (const auto& filePath : m_objectFilesPathNamePairs)
	{
		auto fileData = loadAndParseObjectFile(filePath.first.c_str());
		auto fileData_v2 = loadAndParseObjectFile_v2(filePath.first.c_str());
		if (fileData.get())
		{
			/*std::shared_ptr<ObjFileData> ofd();
			std::shared_ptr<ObjFileData_v2> ofd2();*/
			m_objFilesMap[filePath.second] = std::move(fileData);
			m_objFileMap_v2[filePath.second] = std::move(fileData_v2);
		}
	}
}
