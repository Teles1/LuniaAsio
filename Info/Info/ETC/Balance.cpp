#pragma once
#include "Balance.h"
namespace Lunia {
	namespace XRated {
		namespace Database {

			struct	BalanceTable : public Serializer::ISerializable
			{
			public:
				std::vector<float>	Vit;
				std::vector<float>	Hp;
				std::vector<float>	DamageFactor;
				std::vector<float>	HealingFactor;

				float	AverageHp;
				float	AverageDamage;
			public:
				virtual void Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Logic::BalanceTable");
					out.Write(L"Vit", Vit);
					out.Write(L"Hp", Hp);
					out.Write(L"DamageFactor", DamageFactor);
					out.Write(L"HealingFactor", HealingFactor);
					out.Write(L"AverageHp", AverageHp);
					out.Write(L"AverageDamage", AverageDamage);
				}

				virtual void Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Logic::BalanceTable");
					in.Read(L"Vit", Vit);
					in.Read(L"Hp", Hp);
					in.Read(L"DamageFactor", DamageFactor);
					in.Read(L"HealingFactor", HealingFactor);
					in.Read(L"AverageHp", AverageHp);
					in.Read(L"AverageDamage", AverageDamage);
				}
			};

			class	Balancer
				: public	IBalancer
			{
			private:
				std::vector<BalanceTable>	balanceTable;

			public:
				Balancer()
				{
					balanceTable.resize(static_cast<int>(XRated::Constants::ClassType::NumberOfClassTypes));
				}

				virtual	~Balancer()
				{
				}
				virtual void	LoadData(bool bForceXml)
				{
					if (bForceXml)
						LoadXmlData();
					else
						LoadBinaryData();
				}
				virtual void	SaveData(bool bForceXml) const
				{
					if (bForceXml)
						SaveXmlData();
					else
						SaveBinaryData();
				}


				void	LoadXmlData()
				{
					Resource::SerializerStreamReader		balanceDataReader = Resource::ResourceSystemInstance().CreateSerializerXmlStreamReader(L"Database/PvP/Balance.xml");

					balanceDataReader->Read(L"BalanceTable", balanceTable);
				}
				void	LoadBinaryData()
				{
					Resource::SerializerStreamReader		balanceDataReader = Resource::ResourceSystemInstance().CreateSerializerBinaryStreamReader(L"Database/Balance.b");

					balanceDataReader->Read(L"BalanceTable", balanceTable);
				}

				void SaveXmlData() const
				{
					Resource::SerializerStreamWriter		balanceDataWriter = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"Database/Pvp/Balance.xml");

					balanceDataWriter->Write(L"BalanceTable", balanceTable);
				}
				void SaveBinaryData() const
				{
					Resource::SerializerStreamWriter		balanceDataWriter = Resource::ResourceSystemInstance().CreateSerializerBinaryStreamWriter(L"Database/Balance.b");

					balanceDataWriter->Write(L"BalanceTable", balanceTable);
				}

				virtual	float	GetValue(XRated::Constants::ClassType classType, IBalancer::ValueType valueType, uint16 level = 0)
				{
					if (classType < XRated::Constants::ClassType::Healer || classType >= XRated::Constants::ClassType::NumberOfClassTypes)
						return 1.0f;
					if (level > XRated::Constants::PlayerMaxLevel)
						return	1.0f;

					switch (valueType)
					{
					case	IBalancer::Hp:
						return	balanceTable[static_cast<int>(classType)].Hp[level - 1];
					case	IBalancer::Vit:
						return	balanceTable[static_cast<int>(classType)].Vit[level - 1];
					case	IBalancer::DamageFactor:
						return	balanceTable[static_cast<int>(classType)].DamageFactor[level - 1];
					case	IBalancer::HealingFactor:
						return	balanceTable[static_cast<int>(classType)].HealingFactor[level - 1];
					case	IBalancer::AverageDamage:
						return	balanceTable[static_cast<int>(classType)].AverageDamage;
					case	IBalancer::AverageHp:
						return	balanceTable[static_cast<int>(classType)].AverageHp;
					default:
						break;
					}

					return	1.0f;
				}
			};

			IBalancer& BalancerInstance()
			{
				static	Balancer	instance;

				return	instance;
			}

		}
	}
}