#include "stdafx.h"
#include "Server.h"
#include "WSASocket.h"
#include "Helpers.h"
#include "tiny_obj_loader.h"
#include "UserCommand.h"
#include <array>
#include "GeometryStructures.h"

namespace fs = std::experimental::filesystem;

const int DEFAULT_BUFLEN = 2 << 9;

std::vector<Polygon3D> GetPolygonsFromShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib);


Server::Server(WSASocketC* wsaSocket):
	m_wsaSocket(wsaSocket),
	m_listenSocket(wsaSocket->createListenSocket())
{
	loadObjFiles();
}

//struct UserCmd
//{
//	enum class UserCmdType
//	{
//		List,
//		GetPolygons,
//		Unknown
//	};
//	UserCmdType type;
//	std::vector<std::string> parameters;
//	UserCmd(): type (UserCmdType::Unknown){}
//	inline UserCmdType GetType() const { return type; }
//};


//���������� ���� ������ ������ � ������ ���������, ��� ����������� ����� ��� �������� ������� 3D ������.
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
std::unique_ptr<UserCommand> Server::parseUserCmd(const std::string& user_input, const SOCKET socket)
{
		
	if (user_input.empty())
		return createListFilesCommand(socket, getFileNames());

	std::vector<std::string> cmd_tokens = Helpers::split(user_input, " ");
	if (cmd_tokens[0] == "getlargefile")
	{
		return getLargeFileCmd(socket);
	}
	if (cmd_tokens[0] == "list")
	{
		return createListFilesCommand(socket, getFileNames());
	}
	if (cmd_tokens[0] == "get")
	{
		//cmd example:
		//get polygons model_name.obj 1,2,3,12
		//or
		//get polygons model_name.obj 1..12
		if (cmd_tokens.size() > 3)
		{
			if (cmd_tokens[0] == "polygons")
			{
				const std::string model_name = cmd_tokens[2];

				const std::string listOrInterval = cmd_tokens[3];
				if (!model_name.empty() && !listOrInterval.empty())
				{
					Helpers::ListInterval listInterval;
					if (listInterval.parse(listOrInterval))
					{
						return createSendGeometryCommand(socket, m_objFilesMap, model_name, listInterval);
					}
				}
			}
		}
		else
		{
			//wrong cmd format: forgot to specify parameters
			return createGetGeometrySendProperFormat(socket);
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
			const std::string client_request = Helpers::receivePacket(ClientSocket.getSocket());
			auto user_command = parseUserCmd(client_request, ClientSocket.getSocket());
			user_command->execute();
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

std::vector<Polygon3D> GetPolygonsFromShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& attrib)
{
	std::vector<Polygon3D> polygonVector;
	const size_t number_of_polygons = shape.mesh.num_face_vertices.size();
	polygonVector.reserve(number_of_polygons);
	const size_t vertices_count_in_polygon = 3; //always 3 for triangulated model. and we have triangulated.
	for (size_t polyId = 0; polyId < number_of_polygons; polyId++)
	{
		Polygon3D polygon;
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
		auto polygons = GetPolygonsFromShape(shapes[0], attributes);
		auto objFileData = std::make_unique<ObjFileData>(filename, polygons);
		//objFileData->m_shapes = shapes;
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
		std::abort();
	}
	//Then load them and parse their geometry
	for (const auto& filePath : m_objectFilesPathNamePairs)
	{
		std::unique_ptr<ObjFileData> fileData = loadAndParseObjectFile(filePath.first.c_str());//std::make_unique<ObjFileData>();
		if (fileData.get())
		{
			m_objFilesMap[filePath.second].reset(fileData.get());
		}
	}
}
