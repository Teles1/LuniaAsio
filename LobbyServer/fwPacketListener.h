#pragma once
#include "User.h"
#include "Network/NetStream.h"

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

    class fwPacketListener
    {
    private:
        template<class T>
        struct packetFromType
        {
            T value;
        };
    public:
        fwPacketListener(const fwPacketListener&) = delete; //anti creation  of a copy
        fwPacketListener& operator= (const fwPacketListener&) = delete; // anti copy
        ~fwPacketListener() { }
        inline static fwPacketListener& GetInstance() {
            static fwPacketListener m_instance;
            return m_instance;
        }

    public:
        fwPacketListener() {};

        template<typename F>
        void Connect(F f)
        {
            typedef function_traits<decltype(f)> traits;

            packetFromType<std::remove_reference<traits::template arg<1>::type>::type> packet;

            auto lambda = [f](Lobby::UserSharedPtr& user, Net::StreamReader& streamReader)
            {
                //packetFromType<traits::result_type> packet;
                packetFromType<std::remove_reference<traits::template arg<1>::type>::type> packet;
                streamReader.Read(packet.value);

                f(user, packet.value);
            };

            m_callbacks[packet.value.TypeHash] = lambda;
        }

        void Invoke(Lobby::UserSharedPtr& user, const uint16& packetHeaderHash, Net::StreamReader& streamReader)
        {
            auto it = m_callbacks.find(packetHeaderHash);

            if (it != m_callbacks.end())
            {
                it->second(user, streamReader);
            }
            else
                Logger::GetInstance().Warn("fwPacket::Invoke::user{0} unhandled packet [{1:#4x}]", user->GetId(), packetHeaderHash);
            //do something else ?
        }
    private:

        std::map<uint16, std::function<void(Lobby::UserSharedPtr& user, Net::StreamReader&)>> m_callbacks;
    };

}