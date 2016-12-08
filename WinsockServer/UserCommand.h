#pragma once
#include "Helpers.h"
#include "GeometryStructures.h"

class UserCommand
{
public:
	virtual void execute() = 0;
};

enum class UserCmdType
{
	ListFiles,
	RequestGeometry,
	ListAvailableCommands,
	SendFileGeometry
};

std::unique_ptr<UserCommand> createListFilesCommand(const SOCKET socket, const std::vector<std::string>& fileNames, const std::vector<size_t>& polyCountVector);
std::unique_ptr<UserCommand> createSendCommandListCommand(const SOCKET socket, const std::vector<std::string>& fileNames);

std::unique_ptr<UserCommand> createSendGeometryCommand(const SOCKET socket, const ObjFileDataMap& objectsData, const std::string& object_name, const Helpers::ListInterval& listInterval);
std::unique_ptr<UserCommand> createGetGeometrySendProperFormat(const SOCKET socket);
std::unique_ptr<UserCommand> getLargeFileCmd(SOCKET socket);