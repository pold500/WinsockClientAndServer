#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"
#include <sstream>
#include <boost/optional.hpp>
#include <boost/algorithm/string.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <iomanip>

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
	bool is_cmd_spelled_right = command_tokens[0] == "get" && (command_tokens[1] == "polygons" || command_tokens[1] == "polygons_v2");
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

std::unique_ptr<Helpers::CPacket> Helpers::createPacketByType(const std::string& rawData)
{
	int type = std::stoi(rawData.substr(0, 1));
	switch (type)
	{
	case CPacket::PacketType::StringResponse:
	{
		auto packet = std::make_unique<CStringPacket>();
		packet->deserialize(rawData);
		return packet;
	}
	break;
	case CPacket::PacketType::BinaryVertexData:
	{
		auto packet = std::make_unique<CBinaryVertexDataPacket>();
		packet->deserialize(rawData);
		return packet;
	}
	break;
	case CPacket::PacketType::BinaryVertexData_V2:
	{
		auto packet = std::make_unique<CBinaryVertexDataPacket_V2>();
		packet->deserialize(rawData);
		return packet;
	}
	break;
	default:
		throw std::logic_error("no such cmd type");
	}
	return nullptr;
}

void Helpers::writeObjToFile(const std::string& file_name, const std::vector<Point3D<float>>& vertices, const std::vector<std::vector<int>>& faces)
{
	std::fstream fs(file_name, std::ios_base::in | std::ios_base::out | std::ios_base::trunc);
	fs << std::setprecision(6);
	//Write all vertices to the file
	for (const auto& vertex : vertices)
	{
		fs << "v " << vertex.getX() << " " << vertex.getY() << " " << vertex.getZ() << "\n";
	}
	fs << "\n";
	//Write all indexes for faces
	for (const auto& face : faces)
	{
		fs << "f ";
		for(const auto& face_index : face)
		{
			fs << face_index << " ";
		}
		fs << "\n";
	}
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


bool Helpers::sendPacket(const SOCKET ClientSocket, const CPacket& dataPacket)
{
	//getsockopt with the opt name parameter set to SO_MAX_MSG_SIZE

	//first send packet length
	const std::string data_to_send = createEncodedPacket(dataPacket.serialize());//createSizePacket(encoded_data.length()) + encoded_data;
	const size_t sendSize = data_to_send.size();
	size_t sentBytesCount = 0;
	size_t totalBytesSent = 0;
	const size_t maxFailedAttempts = 3;
	size_t failedAttempts = 0;
	do
	{
		sentBytesCount = ::send(ClientSocket, &data_to_send[totalBytesSent], data_to_send.size() - totalBytesSent, 0);
		totalBytesSent += sentBytesCount;
		if (!sentBytesCount)
		{
			failedAttempts++;
		}
		if (sentBytesCount == SOCKET_ERROR || WSAGetLastError() || failedAttempts>= maxFailedAttempts)
		{
			console_log << "send failed with error: " << WSAGetLastError() << "\n";
			//throw std::exception((std::string("socket error ") + std::to_string(WSAGetLastError())).c_str());
			return false;
		}
	} while (totalBytesSent < sendSize);
		
	console_log << "Total bytes sent: " << totalBytesSent << "\n";
	return true;
}


std::unique_ptr<CPacket> Helpers::receivePacket(const SOCKET ClientSocket)
{
	std::string full_message;
	size_t totalBytesRead = 0;
	size_t packetSize = -1;
	const size_t packet_length_size = calculatePacketLengthPrefix<size_t>();
	const size_t BUFLEN = 4096;
	size_t rcvFailed = 0;
	const size_t rcvTries = 3;
	while (totalBytesRead < packetSize)
	{
		std::string recvbuf;
		recvbuf.resize(BUFLEN);
		const int rcvdBytesCount = ::recv(ClientSocket, &recvbuf[0], /*recvbuf.size()*/BUFLEN, 0);
		if (!rcvdBytesCount && rcvFailed < rcvTries)
		{
			rcvFailed++;
			continue;
		}

		if (WSAGetLastError() != 0 || rcvdBytesCount == INVALID_SOCKET || rcvFailed >= rcvTries)
		{
			console_log << "recv failed with error: " << WSAGetLastError() << "\n";
			return nullptr;
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
	return Helpers::createPacketByType(full_message_decoded);
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



Helpers::CStringPacket::CStringPacket(const std::string& strData) :
	CPacket(CPacket::PacketType::StringResponse),
	m_stringData(strData)
{

}

Helpers::CStringPacket::CStringPacket(): CPacket(PacketType::StringResponse)
{

}

std::string Helpers::CStringPacket::serialize() const
{
	std::string serializedResult;
	serializedResult.append(std::to_string(static_cast<int>(m_packetType)));
	serializedResult.append(m_stringData);
	return serializedResult;
}

void Helpers::CStringPacket::deserialize(const std::string& dataString)
{
	this->m_packetType = static_cast<Helpers::CPacket::PacketType>(std::stoi(dataString.substr(0,1)));
	this->m_stringData = dataString.substr(1);
}

const std::string& Helpers::CStringPacket::GetStringData() const
{
	return m_stringData;
}

Helpers::CBinaryVertexDataPacket::CBinaryVertexDataPacket(const std::vector<Polygon3D>& polygonsData)
	: CPacket(PacketType::BinaryVertexData)
		
{
	m_objectData.m_polygonsData = polygonsData;
}

std::string Helpers::CBinaryVertexDataPacket::serialize() const
{
	//Serializing to binary archive
	std::stringstream ss;
	ss << std::to_string(m_packetType);
	cereal::BinaryOutputArchive binaryOutput(ss);
	m_objectData.save(binaryOutput);
	return ss.str();
}

void Helpers::CBinaryVertexDataPacket::deserialize(const std::string& dataString)
{
	std::stringstream ss;
	ss << dataString.substr(1);
	cereal::BinaryInputArchive binaryInput(ss);
	m_objectData.load(binaryInput);
}

Helpers::CPacket::CPacket(const PacketType& packetType) : m_packetType(packetType)
{

}

Helpers::CBinaryVertexDataPacket_V2::CBinaryVertexDataPacket_V2(ObjFileData_v2* objectData) :
	CPacket(PacketType::BinaryVertexData_V2),
	m_objectData(objectData)
{

}

Helpers::CBinaryVertexDataPacket_V2::CBinaryVertexDataPacket_V2() : CPacket(PacketType::BinaryVertexData_V2), m_objectData(new ObjFileData_v2()),
	m_objectDataClient(m_objectData)
{
	
}

std::string Helpers::CBinaryVertexDataPacket_V2::serialize() const
{
	std::stringstream ss;
	ss << std::to_string(m_packetType);
	cereal::BinaryOutputArchive binaryOutput(ss);
	m_objectData->save(binaryOutput);
	return ss.str();
}

void Helpers::CBinaryVertexDataPacket_V2::deserialize(const std::string& dataString)
{
	std::stringstream ss;
	ss << dataString.substr(1);
	cereal::BinaryInputArchive binaryInput(ss);
	m_objectData->load(binaryInput);
}
