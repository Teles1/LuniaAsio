#pragma once
#include <map>
#include <Network/NetStream.h>
namespace Lunia {
	namespace XRated {
		namespace StageServer {
            template<typename SenderType>
            struct DynamicParser {
            public:
                template <typename PacketType, typename ListenerType>
                void Bind(ListenerType& listener, void (ListenerType::* fp)(const SenderType, PacketType&)) {
                    AutoLock lock(cs);
                    auto i = events.find(PacketType::TypeHash);
                    if (i == events.end()) {
                        events.emplace(PacketType::TypeHash, PacketProcessor());
                        i = events.find(PacketType::TypeHash);
                        i->second.PacketBuffer = std::make_shared<PacketType>();
                    }
                    /* add a packet processor */
                    std::vector<ICallerSharedPtr>& procedures = i->second.Procedures;
                    procedures.push_back(std::make_shared<Caller<PacketType, ListenerType>>(&listener, fp));
                }
                template <typename PacketType, typename ListenerType>
                void Unbind(ListenerType& listener, void (ListenerType::* fp)(const SenderType, PacketType&)) {
                    AutoLock lock(cs);
                    auto i = events.find(PacketType::TypeHash);
                    if (i == events.end()) return;  // nothing to be bound for the packet

                    std::vector<ICallerSharedPtr>& procedures = i->second.Procedures;
                    for (auto del_iter = procedures.begin(); del_iter != procedures.end(); ++del_iter) {
                        if ((*del_iter)->IsEqual(&listener, &fp)) {
                            procedures.erase(del_iter);
                            return;
                        }
                    }
                }
                void Invoke(const SenderType user, Net::StreamReader& reader) {
                    AutoLock lock(cs);
                    auto itr = events.find(reader.GetSerializedTypeHash());
                    if (itr == events.end()) {
                        LoggerInstance().Error("Unhandled Packet [{0:#4x}]", reader.GetSerializedTypeHash());
                        return;
                    }
                    reader.Read(*itr->second.PacketBuffer); // Deserializing the packet
                    auto& procedure = itr->second.Procedures;
                    for (auto& element : procedure) {
                        element->Call(user, *itr->second.PacketBuffer); // calling the listeners binded to this packet. can be multiples.
                    }
                }
            private:
                class ICaller {
                public:
                    virtual ~ICaller() {}
                    virtual void Call(const SenderType SenderType, Serializer::ISerializable& param) = 0;
                    virtual bool IsEqual(void* instance, void* fp) const = 0;
                };

                template <typename PacketType, typename Listener>
                class Caller : public ICaller {
                public:
                    typedef void (Listener::* BoundFunction)(const SenderType, PacketType&);

                    Caller(Listener* listener, BoundFunction fp) : listener(listener), fp(fp) {}
                    void Call(const SenderType SenderType, Serializer::ISerializable& param) {
                        (*listener.*fp)(SenderType, reinterpret_cast<PacketType&>(param));
                    }
                    bool IsEqual(void* instance, void* fp) const {
                        return (instance == listener && (*reinterpret_cast<const BoundFunction*>(fp)) == (this->fp));
                    }

                private:
                    Listener* listener;
                    BoundFunction fp;
                };

                typedef std::shared_ptr<Serializer::ISerializable> ISerializableSharedPtr;
                typedef std::shared_ptr<ICaller> ICallerSharedPtr;

                struct PacketProcessor {
                    ISerializableSharedPtr PacketBuffer;
                    std::vector<ICallerSharedPtr> Procedures;
                };
                typedef std::map<HashType, PacketProcessor> EventCollection;

                EventCollection events;
                std::mutex cs;
            };

		}
	}
}