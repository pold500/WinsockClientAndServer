#ifndef __HELPERS__
#define __HELPERS__


#include <memory>
#include <string>
#include <array>
#include <functional>
#include <vector>
#include <WinSock2.h>		
#include <iostream>
#include "LogStream.h"
#include "base64_default_rfc4648.hpp"
#include "GeometryStructures.h"

namespace Helpers
{

	LogStream& GetLogStream();
	
	template<typename T>
	using custom_deleter_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

	template<typename T>
	class Socket
	{
		T m_socket;
	public:
		Socket() : m_socket(::INVALID_SOCKET) {}
		Socket(const T& socket) : m_socket(socket)
		{

		}
		~Socket()
		{
			::closesocket(m_socket);
			m_socket = INVALID_SOCKET;
		}
		inline const T getSocket() const
		{
			return m_socket;
		}
		operator T&() { return m_socket; }
		operator T() const { return m_socket; }
	};
	size_t readSizePacket(const std::string& sizePacket);
	std::string createSizePacket(const size_t user_packet_size);
	std::string createEncodedPacket(const std::string& user_data);
	std::string decodePacket(const std::string& user_packet);
	void sendPacket(const SOCKET ClientSocket, const std::string& data);
	std::string receivePacket(const SOCKET ClientSocket);

	template<typename SizeType>
	size_t calculatePacketLengthPrefix()
	{
		std::string packet_size;
		packet_size.resize(sizeof(SizeType));
		std::fill_n(begin(packet_size), sizeof(SizeType), 'a');
		const size_t encodedSizePacketLength = base64::encode(packet_size).length();
		return encodedSizePacketLength;
	}

	std::vector<std::string> split(const std::string &s, const std::string& delim);
	std::string serializeInt(int intValue);
	int deserializeInt(const std::string& strValue);
	std::string serializeFloat(float floatValue);
	float deserializeFloat(const std::string& stringValue);

	struct ListInterval
	{
		std::vector<int> vertices_list;
		std::array<int, 2> vertices_range;
		bool m_isList;
		ListInterval() : m_isList(false) {}
		bool validate() const
		{
			//in vertices range we can't have range lower border be higher then higher border
			return (vertices_range[0] < vertices_range[1]);
		}
		bool parse(const std::string& input);
	};

	std::string serializePolygonData(const std::vector<Polygon3D>& polygonsVector);
	void deseralizePolygonData(const std::string& input, std::vector<Polygon3D>& polygonsVector);
};

#define console_log Helpers::GetLogStream()


#endif

