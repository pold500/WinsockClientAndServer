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
	std::vector<std::string> split(const std::string & s, const char delim);
	std::vector<std::string> split(const std::string &s, const std::string& delim);

	struct ListInterval
	{
		std::vector<int> polygons_list;
		struct _polygons_range
		{
			size_t lower_bound;
			size_t upper_bound;
			_polygons_range():
				lower_bound(-1),
				upper_bound(-1)
			{

			}
		} 
		polygons_range;

		bool m_isList;
		ListInterval() : m_isList(false) {}
		bool validate() const
		{
			if (m_isList)
			{
				return !polygons_list.empty();
			}
			//Check range if it's range
			//in vertices range we can't have range lower border be higher then higher border
			else
			{
				return 
					(polygons_range.lower_bound < polygons_range.upper_bound) && 
					(polygons_range.lower_bound != -1 || 
					 polygons_range.upper_bound != -1 ||
					polygons_range.lower_bound != polygons_range.upper_bound);
			}
		}
		bool parse(const std::string& input); 
	};

	
};

#define console_log Helpers::GetLogStream()

#define printVar(x) console_log << ##x << " : " << x << "\n";

#define print(x)    console_log << x << "\n";

#endif


