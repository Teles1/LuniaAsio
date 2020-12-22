#pragma once

// #include "../fwPacketListener.h"

#include <Core/Serializer/Serializer.h>
#include <Core/Utils/InitFunction.h>

#include <LobbyServer/fwPacketListener.h>

#include <LobbyServer/LobbyServer.h>

struct PacketBase
{

	PacketBase() { };

	~PacketBase() { };

	virtual void Serialize  (Lunia::Serializer::IStreamWriter& out) = 0;
	virtual void Deserialize(Lunia::Serializer::IStreamReader& out) = 0;
};

// inline Lunia::fwPacketListener& j_fwPacketListener = g_gameServer->Proxy->PacketListener;

/*
	template<const wchar_t* str>
	static inline unsigned int Hash()
	{
		const wchar_t* p = str;
		unsigned int hashcode = 0;
		wchar_t c = 0;
		while (*p)
		{
			c = *p; hashcode = (31 * hashcode + (c % 31)) % 64000000; ++p;
		}
		return hashcode;
	}
*/