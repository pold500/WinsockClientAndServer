#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"
#include <sstream>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>

using namespace Helpers;

boost::optional<PolygonCmd::PolygonRange> tryToParseRange(const std::string& range)
{
	
	std::vector<std::string> rangeValues = Helpers::split(range, "..");
	if (rangeValues.size() > 1)
	{
		PolygonCmd::PolygonRange polyRange;
		polyRange.lower_bound = std::stoi(rangeValues[0]);
		size_t i = 1;
		while (rangeValues[i].empty())
		{
			i++;
		}
		polyRange.upper_bound = std::stoi(rangeValues[i]);
		return polyRange;
	}
	return boost::optional<PolygonCmd::PolygonRange>();
}

boost::optional<PolygonCmd::PolygonList> tryToParseList(const std::string& range)
{
	std::vector<std::string> listValues = Helpers::split(range, ",");
	if (listValues.size() > 1)
	{
		PolygonCmd::PolygonList polyList;
		for (const auto& listValue : listValues)
		{
			if (!listValue.empty())
			{
				polyList.polygons_list.push_back(std::stoi(listValue));
			}
		}
		return polyList;
	}
	return boost::optional<PolygonCmd::PolygonList>();
}

boost::optional<PolygonCmd> Helpers::parsePolygonCmd(const std::vector<std::string>& command_tokens)
{
	//cmd format:
	//get polygons model_name.obj 1,2,3,4,5
	//get polygons model_name.obj 1-5
	bool is_number_of_tokens_correct = command_tokens.size() == 4;
	bool is_cmd_spelled_right = command_tokens[0] == "get" && command_tokens[1] == "polygons";
	bool is_model_name_present_and_correct = !command_tokens[2].empty();
	bool is_range_present = command_tokens[3].find("..") != std::string::npos;
	bool is_list_present = command_tokens[3].find(",") != std::string::npos;
	
	if (is_number_of_tokens_correct && is_cmd_spelled_right && is_model_name_present_and_correct && (is_range_present || is_list_present))
	{
		Helpers::PolygonCmd polygonCmd(false);
		polygonCmd.file_name = command_tokens[2];

		{
			auto polyRange = tryToParseRange(command_tokens[3]);
			if (polyRange.is_initialized())
			{
				polygonCmd.polyRange = *polyRange;
			}
			else
			{
				auto polyList = tryToParseList(command_tokens[3]);
				if (polyList.is_initialized())
				{
					polygonCmd.polyList = *polyList;
				}
			}
		}
		return polygonCmd;
	}
	else
	{
		return boost::optional<PolygonCmd>();
	}
	return boost::optional<PolygonCmd>();
}

Helpers::PolygonCmd Helpers::PolygonCmd::invalidCmd = Helpers::PolygonCmd(true);

size_t Helpers::readSizePacket(const std::string& sizePacket)
{
	const auto decode = base64::decode(sizePacket.substr(0, calculatePacketLengthPrefix<size_t>()));
	std::string decoded_packet(begin(decode), end(decode));
	size_t packetSize = 0;
	packetSize |= (((uint8_t)decoded_packet[0]) << 24); //higher value of 16 bit unsigned size
	packetSize |= (((uint8_t)decoded_packet[1]) << 16);
	packetSize |= (((uint8_t)decoded_packet[2]) << 8);
	packetSize |= ((uint8_t)decoded_packet[3]) & 0xFF;
	return packetSize;
}

std::string Helpers::createSizePacket(const size_t user_packet_size)
{
	std::string sizePacket;
	sizePacket.resize(sizeof(size_t));
	const size_t full_packet_size = user_packet_size + calculatePacketLengthPrefix<size_t>();
	sizePacket[0] = (full_packet_size >> 24) & 0xFF;//higher value of 16 bit unsigned size
	sizePacket[1] = (full_packet_size >> 16) & 0xFF;
	sizePacket[2] = (full_packet_size >> 8) & 0xFF;
	sizePacket[3] = (full_packet_size & 0xFF);

	return base64::encode(sizePacket);
}

std::string Helpers::createEncodedPacket(const std::string& user_data)
{
	const std::string encoded_user_data = base64::encode(user_data);
	const std::string message_to_send = createSizePacket(encoded_user_data.length()) + encoded_user_data;
	return message_to_send;
}



std::string Helpers::decodePacket(const std::string& user_packet)
{
	if (user_packet.empty())
	{
		return "";
	}
	std::string packet_size;
	const auto encodedSizePacketLength = calculatePacketLengthPrefix<size_t>();
	const auto packet_load = user_packet.substr(encodedSizePacketLength); //drop off packet length data
	const auto decoded_data = base64::decode(packet_load);
	return std::string(begin(decoded_data), end(decoded_data));
}


void Helpers::sendPacket(const SOCKET ClientSocket, const std::string& data)
{
	//getsockopt with the opt name parameter set to SO_MAX_MSG_SIZE

	//first send packet length
	const std::string data_to_send = createEncodedPacket(data);//createSizePacket(encoded_data.length()) + encoded_data;
	const size_t sendSize = data_to_send.size();
	size_t sentBytesCount = 0;
	size_t totalBytesSent = 0;
	do
	{
		sentBytesCount = ::send(ClientSocket, &data_to_send[totalBytesSent], data_to_send.size() - totalBytesSent, 0);
		totalBytesSent += sentBytesCount;
		if (sentBytesCount == SOCKET_ERROR || WSAGetLastError())
		{
			console_log << "send failed with error: " << WSAGetLastError() << "\n";
			throw std::exception((std::string("socket error ") + std::to_string(WSAGetLastError())).c_str());
			break;
		}
	} while (totalBytesSent < sendSize);

	
	console_log << "Total bytes sent: " << totalBytesSent << "\n";
}

std::string Helpers::receivePacket(const SOCKET ClientSocket)
{
	std::string full_message;
	size_t totalBytesRead = 0;
	size_t packetSize = -1;
	const size_t packet_length_size = calculatePacketLengthPrefix<size_t>();
	const size_t BUFLEN = 4096;
	while (totalBytesRead < packetSize)
	{
		std::string recvbuf;
		recvbuf.resize(BUFLEN);
		const int rcvdBytesCount = ::recv(ClientSocket, &recvbuf[0], /*recvbuf.size()*/BUFLEN, 0);
		if (!rcvdBytesCount)
			continue;

		if (WSAGetLastError() != 0 || rcvdBytesCount == INVALID_SOCKET)
		{
			console_log << "recv failed with error: " << WSAGetLastError();
			int error_code = WSAGetLastError();
			throw std::exception((std::string("socket error ") + std::to_string(WSAGetLastError())).c_str());
		}

		totalBytesRead += rcvdBytesCount;

		if (totalBytesRead >= packet_length_size && packetSize == -1) //we can read packet length
		{
			packetSize = readSizePacket(recvbuf);
		}

		if (rcvdBytesCount > 0)
		{
			console_log << "Bytes received: " << rcvdBytesCount << "\n";
			full_message.append(recvbuf);
		}

	}
	const auto full_message_decoded = decodePacket(full_message);
	return full_message_decoded;
}

std::vector<std::string> Helpers::split(const std::string & s, const char delim)
{
	std::string delimeters;
	delimeters += delim;
	return Helpers::split(s, delimeters);
}

std::vector<std::string> Helpers::split(const std::string & str, const std::string& delimiter)
{
	std::vector<std::string> splitResult;
	boost::split(splitResult, str, boost::is_any_of(delimiter));
	return std::move(splitResult);
}

LogStream::LogStream() : myStream(&std::cout)
{

}

LogStream& Helpers::GetLogStream()
{
	static LogStream logStream;
	return logStream;
}
