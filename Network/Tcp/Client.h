#ifndef Client_GUARD
#define Client_GUARD

#pragma once
#include "Shared.h"
#include "../Crypt/Crypt.h"
namespace Lunia {
    namespace Net {
        struct ClientTcp : public std::enable_shared_from_this<ClientTcp> {
        public:
            ClientTcp(tcp::socket&& s);
            void HandleRead();
            ~ClientTcp();
            void CloseSocket();
            void SetCryptoKey(uint32& newKey);
            uint32 GetCryptoKey();
            const std::string GetPeerAddress() const;

            Crypt::Box		m_decryptor;

        protected:
            void SendAsync(uint8* data, size_t size);
            void ReceivedSome(const error_code& ec, size_t size);
            void WroteSome(const error_code& error, size_t size);
            virtual uint32 Parse(uint8* buffer, size_t size) = 0;
        private:
            bool					m_isEncryptKey;
            uint8					m_buffer[READ_DATA_BUFFER_LENGTH] = {};
            tcp::socket				m_socket;
        };
    }
}

#endif // !Client_GUARD