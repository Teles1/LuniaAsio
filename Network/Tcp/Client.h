#ifndef Client_GUARD
#define Client_GUARD

#pragma once
#include "Shared.h"
#include "../Crypt/Crypt.h"
#include "FloodChecker.h"
#include <Core/Utils/Buffer.h>

namespace Lunia {
    namespace XRated {
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
                void SendAsync(uint8* data, size_t size) const;
                void ReceivedSome(const error_code& ec, size_t lenght);
                int OnReceived(uint8* buffer, size_t lenght);
                void WroteSome(const error_code& error, size_t size) const;
                virtual uint32 Parse(uint8* buffer, size_t size) = 0;
                FloodChecker            m_floodChecker;
                uint64					m_aliveTime = 0;
            private:
                bool					m_isEncryptKey;
                uint8                   m_buffer[2 << 14];
                Buffer                  m_receiveBuffer;
                mutable tcp::socket	    m_socket;
            };
        }
    }
}

#endif // !Client_GUARD