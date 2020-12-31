#ifndef fwPacketHandler_H
#define fwPacketHandler_H
#pragma once
#include <Network/NetStream.h>
namespace Lunia {
    template <typename T>
    struct function_traits
        : public function_traits<decltype(&T::operator())>
    {};
    // For generic types, directly use the result of the signature of its 'operator()'

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const>
        // we specialize for pointers to member function
    {
        enum { arity = sizeof...(Args) };
        // arity is the number of arguments.

        typedef ReturnType result_type;

        template <size_t i>
        struct arg
        {
            typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
            // the i-th argument is equivalent to the i-th tuple element of a tuple
            // composed of those arguments.
        };
    };

#include <iostream>

    template<typename TClientProxySharedPtr>
    class fwPacketHandler
    {
    private:
        template<class T>
        struct PacketFromType
        {
            T value;
        };
    public:
        fwPacketHandler() {}

        template<typename F>
        void Connect(F f)
        {
            typedef function_traits<decltype(f)> traits;

            PacketFromType<std::remove_reference<traits::template arg<1>::type>::type> packet;

            auto lambda = [f](TClientProxySharedPtr& client, Lunia::Net::StreamReader& streamReader)
            {
                /*
                    FIXME! These streamReader members shouldnt be exposed!
                */
                PacketFromType<std::remove_reference<traits::template arg<1>::type>::type> packet;

                /*
                    streamReader.working = (char*)streamReader.buffer;
                    streamReader.working += 4;

                    packet.value.Deserialize(streamReader);
                */

                streamReader.Read(packet.value);

                f(client, packet.value);
            };

            /*
                FIXME! We should need to create a new packet here, we should instead use type::asHash, as its static.
            */
            // m_callbacks[packet.value.asHash] = lambda;
            m_callbacks[(uint16_t)packet.value.TypeHash] = lambda;
        }

        bool /* operator() */ Invoke(TClientProxySharedPtr& client, const uint16_t& packetHeaderHash, Lunia::Net::StreamReader streamReader)
        {
            auto it = m_callbacks.find(packetHeaderHash);

            if (it != m_callbacks.end())
            {
                it->second(client, streamReader);
            }
            else
            {
                Logger::GetInstance().Warn("fwPacket::Invoke::client unhandled packet [{0:#4x}]", packetHeaderHash);

                //streamReader.PrintOutData();

                return false;
            }
            //do something else ?

            return true;
        }
    private:

        std::map<uint16_t, std::function<void(TClientProxySharedPtr&, Lunia::Net::StreamReader&)>> m_callbacks;
    };
}

#endif // !fwPacketHandler_H