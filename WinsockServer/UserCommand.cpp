#include "stdafx.h"
#include "UserCommand.h"
#include "ListCommand.h"
#include "UserCommandNoOperation.h"
#include "GetVertexesCommand.h"
#include "GetGeometrySendProperFormat.h"
#include "SendLargeFileCmd.h"
#include "SendGeometryCmd_v2.h"
#include "StringResponseCmd.h"

std::unique_ptr<UserCommand> createStringResponseCommand(const SOCKET socket, const std::string& responseText)
{
	return std::make_unique<StringResponseCmd>(socket, responseText);
}

std::unique_ptr<UserCommand> createListFilesCommand(const SOCKET socket, 
	const std::vector<std::string>& fileNames, const std::vector<size_t>& polyCountVector)
{
	return std::make_unique<ListFilesUserCommand>(socket, fileNames, polyCountVector);
}

std::unique_ptr<UserCommand> createSendCommandListCommand(const SOCKET socket, 
	const std::vector<std::string>& fileNames)
{
	return std::make_unique<UserCommandNoOperation>(socket);
}


std::unique_ptr<UserCommand> createSendGeometryCommand(const SOCKET socket, const ObjFileDataMap& objectsData, const std::string& object_name, const Helpers::PolygonCmd& cmdParams)
{
	return std::make_unique<SendGeometryCmd>(socket, *(*objectsData.find(object_name)).second, cmdParams);
}

std::unique_ptr<UserCommand> createSendGeometryCommand_v2(const SOCKET socket, ObjFileData_v2* objectData, const Helpers::PolygonCmd& cmdParams)
{
	return std::make_unique<SendGeometryCmd_v2>(socket, objectData, cmdParams);
}


std::unique_ptr<UserCommand> createGetGeometrySendProperFormat(const SOCKET socket)
{
	return std::make_unique<GetGeometrySendProperFormat>(socket);
}
std::unique_ptr<UserCommand> getLargeFileCmd(SOCKET socket)
{
	return std::make_unique<SendLargeFileCmd>(socket);
}
