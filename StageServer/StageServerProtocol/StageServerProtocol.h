#pragma once
#include <Core/GameConstants.h>
namespace Lunia {
	namespace StageServer {
		namespace Protocol {
			/*
				We shall see the reason why they did this.
			*/
			class IPacketSerializable : public Serializer::ISerializable {
			public:
				virtual void Serialize(Serializer::IStreamWriter& out) const = 0;
				virtual void Deserialize(Serializer::IStreamReader& in) = 0;

				virtual const wchar_t* GetTypeName() = 0;

				virtual ~IPacketSerializable() {}
			};

			struct Error : public IPacketSerializable
			{
				int			 errorcode;
				std::wstring errorstring;

				NET_SERIALIZABLE;
			};

			struct Way : public IPacketSerializable
			{
				uint32 EncryptKey;

				NET_SERIALIZABLE;
			};

			struct Stage : public IPacketSerializable
			{
				//To Client
				std::wstring charactername;
				XRated::StageLicense targetStage;
				//From Client
				std::wstring secukey;
				uint32 Version;
				std::wstring Locale;

				NET_SERIALIZABLE;
			};

			struct Alive : public IPacketSerializable
			{
				uint64	RequestTime;
				uint32 EncryptKey;

				NET_SERIALIZABLE;
			};
		}
	}
}

