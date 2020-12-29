#include <StageServer/fwPacketListener.h>
#include <StageServer/User/UserManager.h>
#include <Network/NetStream.h>
#include <Network/CommonProtocol/Protocol.h>
#include <StageServer/StageServerProtocol/StageServerProtocol.h>
#include <Network/Api/Api.h>

namespace Lunia {
	namespace XRated {
		namespace StageServer
		{
			void User::Send(Serializer::ISerializable& packet){
				StaticBuffer<2 << 12> buffer;
				try
				{
					Net::StreamWriter writer(buffer);
					writer.Write(packet);
				}
				catch (Exception&) // catch something like buffer overflow
				{
					Logger::GetInstance().Error("User@{0} Unable to Parse packet", this->GetSerial());
					return;
				}

				Logger::GetInstance().Info("Sending packet[{0:#04x}] to User@{1}", *((HashType*)buffer.GetData() + 2), this->GetSerial());
				this->SendAsync(reinterpret_cast<uint8*>(buffer.GetData()), buffer.GetLength());
			}
			uint32 User::Parse(uint8* buffer, size_t size){
				/*
					We should technically see but i think that if the server doesn't recognize the packet I say we let the client connection dies.
					That's why I want to keep this call in here
					Parse only known packets. At least the ones which follow NetStream Standarts.
				*/

				Net::StreamReader sReader(buffer);
				if(!IsAuthenticated())
					fwPacketListener::GetInstance().Invoke(UserManagerInstance().GetUserByConnectionId(m_UserId), sReader.GetSerializedTypeHash(), sReader);
				else
					fwPacketListener::GetInstance().Invoke(UserManagerInstance().GetUserBySerial(m_UserSerial), sReader.GetSerializedTypeHash(), sReader);
				HandleRead();
				return (uint32)size;
			}

			void User::Init() {
				AutoLock _l(mtx);
				Protocol::Way way;
				way.EncryptKey = Math::Random();
				SetCryptoKey(way.EncryptKey);
				Send(way);
			}

			uint32 User::GetId() const {
				return m_UserId;
			}

			void User::SetSerial(const uint64& userSerial){
				m_UserSerial = userSerial;
			}

			const uint64& User::GetSerial() const
			{
				return m_UserSerial;
			}

			bool User::IsAuthenticated() const
			{
				return m_UserSerial == 0 ? false : true;
			}

			void User::SetCharacterName(const String& name) {
				this->m_CharacterName = name;
			}

			String User::GetCharacterName() const {
				return m_CharacterName;
			}

			void User::Update(const float& dt){
				AutoLock l_(mtx);
				//Update the user with the time.
			}

			StageLicense& User::GetCurrentStage(){
				return m_CurrentStage;
			}

			void User::Terminate(){
				Logger::GetInstance().Warn("Terminate has to be implemented.");
				AutoLock _l(mtx);
				Net::Api api("Terminate");
				api << GetCharacterName();
			}
		}
	}
}