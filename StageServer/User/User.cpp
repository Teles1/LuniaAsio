#include <StageServer/fwPacketListener.h>
#include <StageServer/User/UserRegistry.h>
#include <Network/NetStream.h>
#include <Network/CommonProtocol/Protocol.h>
#include <Network/Api/Api.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			void User::Send(Serializer::ISerializable& packet)
			{
				StaticBuffer<2 << 12> buffer;
				try
				{
					Net::StreamWriter writer(buffer);
					writer.Write(packet);
				}
				catch (Exception&) // catch something like buffer overflow
				{
					Logger::GetInstance().Error("User@{0} Unable to Parse packet", this->GetId());
					return;
				}

				Logger::GetInstance().Info("Sending packet[{0:#04x}] to User@{1}", *((HashType*)buffer.GetData() + 2), this->GetId());
				this->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
			}
			uint32 User::Parse(uint8* buffer, size_t size)
			{
				/*
					We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies.
					That's why I want to keep this call in here
					Parse only known packets. At least the ones which follow NetStream Standarts.
				*/

				Net::StreamReader sReader(buffer);

				auto userPtr = UserRegistry::GetInstance().GetUserByUserId(this->GetId());

				fwPacketListener::GetInstance().Invoke(userPtr, sReader.GetSerializedTypeHash(), sReader);

				HandleRead();
				return (uint32)size;
			}

			void User::Init() {
				{
					AutoLock _l(mtx);
					Protocol::Way way;
					way.EncryptKey = Math::Random();
					SetCryptoKey(way.EncryptKey);
					Send(way);
				}
			}

			uint32 User::GetId() const {
				return m_userId;
			}

			void User::SetId(const uint32& userId)
			{
				AutoLock _l(mtx);
				m_userId = userId;
			}
			bool User::IsAuthenticated() const {
				return m_IsAuthenticated;
			}

			void User::SetIsAuthenticated()
			{
				AutoLock _l(mtx);
				this->m_IsAuthenticated = true;
			}

			void User::SetCharacterName(const String& name) {
				AutoLock _l(mtx);
				this->m_CharacterName = name;
			}

			String User::GetCharacterName() const {
				return m_CharacterName;
			}

			void User::Update(const float& dt)
			{
				AutoLock l_(mtx);
				//Update the user with the time.
			}

			StageLicense& User::GetCurrentStage()
			{
				return m_CurrentStage;
			}

			void User::Terminate()
			{
				Logger::GetInstance().Warn("Terminate has to be implemented.");
				AutoLock _l(mtx);
				Net::Api api("Terminate");
				api << GetCharacterName();
			}
		}
	}
}