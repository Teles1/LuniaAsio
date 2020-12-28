#include "ClientProxy.h"

#include "Network/NetStream.h"

#include "./Network/Api/Api.h"

ClientProxy::ClientProxy(asio::ip::tcp::socket&& socket) : ClientNetworkIO(std::move(socket)) /* WARN socket is moved a shit ton of times here */
{

};

void ClientProxy::MakeSocketAsyncWriteSerializable(Lunia::Serializer::ISerializable& packet)
{
	Lunia::StaticBuffer<2 << 12> buffer;

	try
	{
		Lunia::Net::StreamWriter streamWriter(buffer);

		streamWriter.Write(packet);

		// unsigned short * packetNameHashed = (char*)streamWriter.buffer.GetData();
		// std::cout << "server -> Client@" << this->GetId() << " 0x" << decltype(packet)::value::TypeHash << std::endl;
	}
	catch (Lunia::Exception&)
	{
		Logger::GetInstance().Error("MakeSocketAsyncWriteSerializable:: Client@{0} couldn't parse packet", this->GetId());
		return;
	}

	this->MakeSocketAsyncWriteSome(reinterpret_cast<uint8_t*>(buffer.GetData()), buffer.GetLength());
}

uint32_t ClientProxy::GetId() const
{
	return this->m_id;
}


void ClientProxy::SetId(const uint32_t& id)
{
	this->m_id = id;
}

void ClientProxy::SetLocale(const std::wstring& locale)
{
	this->m_locale = locale;
}

bool ClientProxy::IsWaitingOnPing() const
{
	return this->m_isWaitingOnPing;
}

std::wstring ClientProxy::GetAccountName() const
{
	return this->m_accountName;
}

void ClientProxy::SetAccountName(const std::wstring& accountName)
{
	this->m_accountName = accountName;
}

bool ClientProxy::HasLicense(Lunia::XRated::Constants::ClassType classType)
{
	return (m_accountLicense & (1 << static_cast<int>(classType))) != 0;
}

void ClientProxy::SetCharacterStateFlags(const Lunia::XRated::CharacterStateFlags& flag)
{
	if (HasSelectedACharacter()) {
		std::scoped_lock<std::mutex> slock(mtx);

		m_characterStateFlags = m_selectedCharacter.StateFlags.EquipmentSet;
		m_characterStateFlags = flag;

		Lunia::Net::Api api("UpdateCharacterStateFlags");

		api << GetAccountName() << GetCharacterName() << (int)m_characterStateFlags;

		if (api.RequestApi().errorCode != 0)
			Logger::GetInstance().Warn(L"Could not update StateFlags for user = {0}, character = {1}", GetAccountName(), GetCharacterName());
	}
	else
	{
		Logger::GetInstance().Warn(L"Account ={0} doesn't have a selected character and there is a call to update flags.", GetAccountName());
	}
}

bool ClientProxy::DoesCharacterExist(std::wstring& characterName)
{
	std::scoped_lock<std::mutex> slock(mtx);

	for (auto& x : m_characters)
	{
		if (x.CharacterName == characterName)
			return true;
	}

	return false;
}

bool ClientProxy::DeleteCharacter(std::wstring& characterName)
{
	std::scoped_lock<std::mutex> slock(mtx);

	for (std::vector<Lunia::XRated::LobbyPlayerInfo>::iterator it = m_characters.begin(); it != m_characters.end(); it++)
	{
		if (it->CharacterName == characterName)
		{
			m_characters.erase(it);

			return true;
		}
	}

	return false;
}

bool ClientProxy::HasSelectedACharacter() const
{
	return m_selectedCharacter.CharacterSerial != -1;
}

std::wstring ClientProxy::GetCharacterName() const
{
	return this->m_selectedCharacter.CharacterName;
}

bool ClientProxy::SetSelectedCharacter(std::wstring& characterName)
{
	std::scoped_lock<std::mutex> slock(mtx);
	for (auto& character : m_characters)
	{
		if (character.CharacterName == characterName)
		{
			this->m_selectedCharacter = character;
			return true;
		}
	}

	return false;
}

void ClientProxy::Error(ErrorLevel error, const std::wstring& message)
{
	switch (error)
	{
	case ErrorLevel::Pvp:
	case ErrorLevel::Curious:
		break;
	case ErrorLevel::Critical:
	case ErrorLevel::Unexpected:
	{
		std::string operation(error == ErrorLevel::Critical ? "critical" : "unexpected");
		std::string characterName;
		//if (m_selectedCharacter.CharacterSerial != -1)
			//characterName = StringUtil::ToASCII(m_selectedCharacter.CharacterName);

		/* LOGGING */
	}
	break;
	}
	//Some sort of disconnecting process going in here. 
}
