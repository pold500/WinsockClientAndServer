#pragma once
#include "UserCommand.h"

class ListFilesUserCommand: public UserCommand
{
	struct FileList
	{
		std::string name;
		size_t polygonCount;
		FileList(const std::string& _name,
		size_t _polygonCount):
		name(_name),
		polygonCount(_polygonCount)
		{}
	};
	std::vector<FileList> m_fileList;
	std::vector<std::string> m_namesList;
	std::vector<size_t> m_polyCount;
	SOCKET m_ClientSocket;
public:

	ListFilesUserCommand(const SOCKET _socket, const std::vector<std::string>& fileList,
		const std::vector<size_t>& polyCountList) :
		m_ClientSocket(_socket),
		m_namesList(fileList),
		m_polyCount(polyCountList)
	{
		for (size_t i = 0; i < m_namesList.size(); i++)
		{
			FileList fl(m_namesList[i], m_polyCount[i]);
			m_fileList.push_back(fl);
		}
	}
	virtual void execute() override;
	
};

