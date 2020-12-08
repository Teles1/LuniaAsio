#pragma once
#include "User.h"
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

struct fwPacketListener
{
public:

    template<class T>
    struct packetFromType
    {
        T value;
    };

    template<typename F>
    void Connect(F f)
    {
        typedef function_traits<decltype(f)> traits;

        packetFromType<std::remove_reference<traits::template arg<1>::type>::type> packet;

        auto lambda = [f](Lobby::UserSharedPtr& user, Serializer::StreamReader& streamReader)
        {
            //packetFromType<traits::result_type> packet;
            packetFromType<std::remove_reference<traits::template arg<1>::type>::type> packet;

            packet.value.Deserialize(streamReader);

            f(user, packet.value);
        };

        m_callbacks[packet.value.TypeHash] = lambda;
    }

    void operator()(Lobby::UserSharedPtr& user, uint16& packetHeaderHash, Serializer::StreamReader& streamReader)
    {
        auto it = m_callbacks.find(packetHeaderHash);

        if (it != m_callbacks.end())
        {
            it->second(user, streamReader);

            std::cout << "[fwPacketListener] Handler de " << packetHeaderHash << " foi encontrado e invocado!" << std::endl;
        }
    }

    static fwPacketListener instance;

private:
    std::map<uint16, std::function<void(Lobby::UserSharedPtr& user, Serializer::StreamReader&)>> m_callbacks;
};
