#pragma once
#include <Core/Serializer/Serializer.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				struct SwapSetInfo
					: public Serializer::ISerializable
				{
					uint8 SetIndex;
					uint8 BagNumber;
					std::wstring	EquipSetName;

					struct FindByIndex
					{
						uint8 Index;
						FindByIndex(uint8 i) :Index(i) {}
						bool operator()(const SwapSetInfo& info) const { return info.SetIndex == Index; }
					};

					struct FindByBagNumber
					{
						uint8 Bag;
						FindByBagNumber(uint8 i) :Bag(i) {}
						bool operator()(const SwapSetInfo& info) const { return info.BagNumber == Bag; }
					};

					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}