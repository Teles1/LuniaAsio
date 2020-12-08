#include "User.h"
#include "fwPacketListener.h"

namespace Lobby 
{
	void User::Parse(uint8* buffer, size_t size)
	{
		std::cout << "Parser called" << std::endl;
		/*
			We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies. 
			That's why I want to keep this call in here
			Parse only known packets. At least the ones which follow NetStream Standarts.
		*/

		Serializer::StreamReader sReader(buffer, size);

		uint16 packetHeaderHash;
		sReader.Read(packetHeaderHash);

		uint32 a = 0;
		this->SetCryptoKey(a);

		/*
			Move 'fwPacketListener::instance' to someplace it can be globally accessed;
		*/
		fwPacketListener::instance.Connect([&](/*UserSharedPtr& user, */Lobby::Protocol::Head& packet)
		{
			packet.Checksums[0] = 1;

			packet.Result = Lobby::Protocol::Head::Results::Ok;

			packet.EncryptKey = 0;
			packet.ServerTime = DateTime::Now();
			packet.UserIP = "127.0.0.1";

			StaticBuffer< 4096 > buffer;
			try
			{
				Serializer::StreamWriter writer(buffer);
				writer.Write(packet);
			}
			catch (Exception&) // catch something like buffer overflow
			{
				ERROR_LOG("unable to send", 0);
				return;
			}

			this->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
		});

		fwPacketListener::instance.Connect([](/*UserSharedPtr& user, */Lobby::Protocol::Auth& packet)
		{
			INFO_LOG("Evento CHAMADO AUTH!!!! {0}", packet.EncryptedPassword);
		});

		fwPacketListener::instance(packetHeaderHash, sReader);

		HandleRead();
	}
}
