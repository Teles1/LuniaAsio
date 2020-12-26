#pragma once
#include "GeneralInfo.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void GeneralInfo::CharacterInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::GeneralInfo::CharacterInfo");
					out.Write(L"Radius", Radius);
					out.Write(L"MaxSp", MaxSp);
					out.Write(L"RegenHp", RegenHp);
					out.Write(L"RegenMp", RegenMp);
					out.Write(L"RegenSp", RegenSp);
					out.Write(L"Resource", Resource);
				}

				void GeneralInfo::CharacterInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::GeneralInfo::CharacterInfo");
					in.Read(L"Radius", Radius);
					in.Read(L"MaxSp", MaxSp);
					in.Read(L"RegenHp", RegenHp);
					in.Read(L"RegenMp", RegenMp);
					in.Read(L"RegenSp", RegenSp);
					in.Read(L"Resource", Resource, std::wstring());
				}

				void GeneralInfo::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::GeneralInfo");
					out.Write(L"PhysicalCri", PhysicalCri);
					out.Write(L"MagicCri", MagicCri);
					out.Write(L"MaxNPCCnt", MaxNPCCnt);
					out.Write(L"CharacterInfos", CharacterInfos);
					out.Write(L"PoolSize", PoolSize);
					out.Write(L"MultiHitTimeInMSec", MultiHitTimeInMSec);
					out.Write(L"MultiHitCount", MultiHitCount);
				}

				void GeneralInfo::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::GeneralInfo");
					in.Read(L"PhysicalCri", PhysicalCri, 2.0);
					in.Read(L"MagicCri", MagicCri, 2.0);
					in.Read(L"MaxNPCCnt", MaxNPCCnt, 100);
					in.Read(L"CharacterInfos", CharacterInfos);
					in.Read(L"PoolSize", PoolSize, std::vector< std::vector<int> >());
					in.Read(L"MultiHitTimeInMSec", MultiHitTimeInMSec, 1000);
					in.Read(L"MultiHitCount", MultiHitCount, 20);
				}
			}
		}
	}
}
