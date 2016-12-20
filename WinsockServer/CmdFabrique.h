#ifndef CmdFabrique_h__
#define CmdFabrique_h__

std::unique_ptr<UserCommand> createListFilesCommand(const SOCKET socket, const std::vector<std::string>& fileNames, const std::vector<size_t>& polyCountVector);
std::unique_ptr<UserCommand> createSendCommandListCommand(const SOCKET socket, const std::vector<std::string>& fileNames);
std::unique_ptr<UserCommand> createSendGeometryCommand(const SOCKET socket, const ObjFileDataMap& objectsData,
													   const std::string& object_name, const Helpers::PolygonCmd& cmdParams);
std::unique_ptr<UserCommand> createSendGeometryCommand_v2(const SOCKET socket, ObjFileData_v2* objectData, const Helpers::PolygonCmd& cmdParams);
std::unique_ptr<UserCommand> createGetGeometrySendProperFormat(const SOCKET socket);
std::unique_ptr<UserCommand> getLargeFileCmd(SOCKET socket);

#endif // CmdFabrique_h__


