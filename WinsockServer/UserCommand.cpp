#include "stdafx.h"
#include "UserCommand.h"
#include "ListCommand.h"
#include "UserCommandNoOperation.h"
#include "GetVertexesCommand.h"
#include "GetGeometrySendProperFormat.h"
#include "SendLargeFileCmd.h"

//
//std::unique_ptr<UserCommand> createCommand(const UserCmdType& cmdType, const SOCKET socket)
//{
//	switch (cmdType)
//	{
//	case UserCmdType::ListAvailableCommands:
//		return std::make_unique<UserCommandNoOperation>(socket);
//		break;
//	case UserCmdType::ListFiles:
//		return std::make_unique<ListCommand>(socket);
//		break;
//		case UserC
//	default:
//		return std::make_unique<UserCommandNoOperation>(socket);
//	}
//}

std::unique_ptr<UserCommand> createListFilesCommand(const SOCKET socket, 
	const std::vector<std::string>& fileNames)
{
	return std::make_unique<ListFilesUserCommand>(socket, fileNames);
}

std::unique_ptr<UserCommand> createSendCommandListCommand(const SOCKET socket, 
	const std::vector<std::string>& fileNames)
{
	return std::make_unique<UserCommandNoOperation>(socket);
}

std::unique_ptr<UserCommand> createSendGeometryCommand(const SOCKET socket, const ObjFileDataMap& objectsData,
	const std::string& object_name, const Helpers::ListInterval& listInterval)
{
	return std::make_unique<GetVertexesCommand>(socket, *(*objectsData.find(object_name)).second, listInterval);
}

std::unique_ptr<UserCommand> createGetGeometrySendProperFormat(const SOCKET socket)
{
	return std::make_unique<GetGeometrySendProperFormat>(socket);
}
std::unique_ptr<UserCommand> getLargeFileCmd(SOCKET socket)
{
	return std::make_unique<SendLargeFileCmd>(socket);
}
