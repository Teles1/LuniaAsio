#ifndef Client_GUARD
#define Client_GUARD

#pragma once
#include "Shared.h"
#include "../Crypt/Crypt.h"

namespace net {
    struct ClientTcp : std::enable_shared_from_this<ClientTcp> {
    public:
        ClientTcp(tcp::socket&& s);
        void SendAsync(uint8* data, size_t& size);
        void HandleRead();
        ~ClientTcp();
    protected:
        void ReceivedSome(const error_code& ec, size_t size);
        void SetCryptyoKey(uint32& newKey);
        uint32 GetCryptoKey();
        void WroteSome(const error_code& error, size_t size);
        virtual void Parse(uint8* buffer, size_t size) = 0;
	protected:
		Network::Crypt::Box		m_decryptor;
		bool					m_isEncryptKey;
    private:
		uint8					m_buffer[READ_DATA_BUFFER_LENGTH] = {};
        tcp::socket				m_socket;
    };
}

#endif // !Client_GUARD