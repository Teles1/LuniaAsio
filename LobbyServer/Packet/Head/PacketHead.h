#pragma once

#include "../PacketBase.h"

#include <Core/Utils/DateTime.h>

uint16_t asHash = 0xAA41;

struct PacketHeadFromClient : public PacketBase
{
	uint16_t ProtocolVersion;
	std::vector<std::string> Checksums;

	enum Results
	{
		Ok = 0,
		VersionMismatched,
		NoResponse = 0xff,
	} Result;

	Lunia::DateTime ServerTime;
	std::string UserIP;
	uint32_t EncryptKey;

	int Hashcode;

public:
	inline static uint16_t asHash = asHash;

	void Serialize(Lunia::Serializer::IStreamWriter& out);
	void Deserialize(Lunia::Serializer::IStreamReader& out);
};

struct PacketHeadToClient : public PacketBase
{
	inline static uint16_t asHash = asHash;

};