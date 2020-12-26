#pragma 
#include "Enchant.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				namespace Enchant {
					struct AlchemyTable : public Serializer::ISerializable
					{
						uint32 Fee;
						float SuccessRate;
						std::vector<RequiredItem> Requirements;

						std::vector< std::vector<RequiredItem> > Results;
						Math::RandomByPortion Portions;

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};
					struct CashEnchantInfo : public Serializer::ISerializable
					{
						uint16	Level;
						uint16	StoredLevel;
						uint16	RebirthCount;

						Info::ActionInfo::StateList States;
						Info::StateBundleInfo::ConditionList StateBundleConditions;

					public: // Serializer::ISerializable impl
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);
					};
					class EnchantTableManager : public Serializer::ISerializable {
					public:
						EnchantTableManager();

						struct EnchantTargets {
							enum Type { Dmg = 0, Str, Dex, Int, Vit, Def, MaxHp, MaxMp, Resist1, Resist2, Resist3, Resist4, Resist5, None };
						};

						struct ProbabilityInfo
						{
							uint32 Hash;
							std::vector<float>	DestroyTable;
							std::vector<float>	SuccessTable;
							std::vector<float>	GradeDownTable;
						public:
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						};

						struct Constants : public Serializer::ISerializable
						{
						public: // Serializer::ISerializable impl
							std::vector<uint32>	HashJewel;
							std::vector<uint32>	HashJewelForAccessory;
							uint32	HashRecoverStone;
							uint32	HashResetScroll;
							uint8	MaxSkillPlus;
							uint32	HashMineral;
							uint32 HashMineralForAccessory;
							uint32	HashCoin;
							uint32	HashCatalyst;

							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} Constant;

						typedef uint32 ItemHashForProduce;
						typedef uint32 ItemCountForProduce;
						typedef std::pair< ItemHashForProduce, ItemCountForProduce > ItemInfoForProduce;
						typedef std::vector< ItemInfoForProduce > ItemInfosForProduce;

					protected:
						struct ReinforcementTable : public Serializer::ISerializable {
							std::vector<ProbabilityInfo>	Probabilities;
							//std::vector< std::vector<float> > Probability;
							std::vector< std::vector< std::vector< float4 > > > Table; //ReinforceTable [Level Limit] [Reinforcement Level] [Enchant Parts] float3(1st, 2nd, 3rd)
							std::vector< std::vector< std::vector< float4 > > > RebirthTable; //ReinforceTable [Sum of INCSTAT value] [Reinforcement Level] [Enchant Parts] float3(1st, 2nd, 3rd)
							std::map< uint16, std::map< uint32, uint8 > > RecoverTable; //[Level][StoneHash][StoneCount]
							int	RecoverFee;

							void Clear();
						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} Reinforcement;

						struct IdentifyTable : public Serializer::ISerializable {
							std::vector<float> StatusTypeProbability;
							std::vector<float> GradeProbability;
							std::vector< std::vector< std::vector<float> > > Table; //float Values[XRated::Constants::MaxLevel/*������ ���ѷ���*/][None/*LastElement*/][8/*��8�ܰ�-3bits*/] ;
							std::vector< std::vector< std::vector<float> > > RebirthTable; //float Values[Item�� ���� ���ΰ��� State�� �հ�][None/*LastElement*/][8/*��8�ܰ�-3bits*/] ;
							std::map< uint16, std::map< uint32, uint8 > > ResetTable;	 //[Level][StoneHash][StoneCount]
							int	ResetFee;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} Identify, AdvancedIdentify, CashIdentify;

						struct LightReinforcementTable : public Serializer::ISerializable {
							//std::vector< std::vector<float> > Probability;
							std::vector<ProbabilityInfo>	Probabilities;
							std::map< uint16, std::vector< std::vector< std::vector<float> > > > Table; //LightReinforceTableinforceme [Level Limit] [Rent Level] [Enchant Parts] [Stat]
							std::map< uint16, std::vector< std::vector< std::vector<float> > > > RebirthTable; //LightReinforceTableinforceme [Level Limit] [Rent Level] [Enchant Parts] [Stat]
							std::map< uint16, std::map< uint32, uint8 > > RecoverTable; //[Level][StoneHash][StoneCount]
							int	RecoverFee;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} LightReinforcement;

						struct MagicalAttachInfo : public Serializer::ISerializable {
							uint32 Hash;
							std::vector<RequiredItem> Requirements;
							std::vector<Info::StateBundleInfo> StateBundles;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						};

						struct MagicalAttachTable : public Serializer::ISerializable {
							std::vector<MagicalAttachInfo> ScrollList;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} MagicalAttach;

						struct ExtractionTable : public Serializer::ISerializable {
							std::vector<float> Probability;	// ������ ���� Ȯ��, �����ܰ躸�� Ȯ��, ��ȭ���� Ȯ��
							std::vector< std::vector<uint16> > CountPerLevel;//������ ������ ���� ����Ǵ� ������ ����.
							std::vector< std::vector<uint16> > CountPerReinforcement;	//�������� ��ȭ �ܰ迡 ���� ����Ǵ� ������ ����.

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} Extraction;

						struct CatalystTable : public Serializer::ISerializable {
							uint32 ItemHash;						///< �˸� ������ �ؽ�(�˸� ���̺��� ����� �������� �ؽ�).
							uint32 ProbabilityTableHash;			///< �˸� ���ÿ� ���� Ȯ�� ���̺� hash
							std::vector<float> ProbabilityPerCnt; //������ ���� Ȯ��
							std::vector<float> ProbabilityPerLevel; //���� ���� ��ġ
							std::vector<uint8> ReinforceMinCounts;	//�˸� ȿ���� �ֱ⿡ �ʿ��� �ּ� ����
							std::vector<uint8> LightReinforceMinCounts;	//�˸� ȿ���� �ֱ⿡ �ʿ��� �ּ� ����

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						};

						struct TempConstants {
							static uint32 HashRecoverStone;
							static uint32 HashResetScroll;
						};


						std::vector<CatalystTable> Catalysts;

						std::vector<CatalystTable> CatalystsForAccessory;

						struct RequirementTable : public Serializer::ISerializable {
							std::vector<uint32> GradeFactor;			// �������� ��޿� ���� ���. ItemInfo::EquipmentLevel::Type�� ���� ���.
							std::vector<uint32> ReinforcementFactor; // ��ȭ �ܰ� ���
							std::vector<uint32> LightReinforcementFactor; // ����ȭ �ܰ� ���

							std::vector<uint16> MineralCntPerLevel;	// ������ ��ȭ�� �ʿ� ����.
							std::vector<uint16> JewelCntPerLevel;	// ������ ���� �ʿ� ����.

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} Requirement;

						std::map< uint32/*item hash*/, AlchemyTable > alchemy;
						std::vector< std::map< uint32/* serial */, CashEnchantInfo> > CashEnchant;
						std::map< uint32/* serial */, CashEnchantInfo> CashEnchant3;
						std::map< uint32/* serial */, CashEnchantInfo> CashEnchant4;
						uint16 RebirthCountEquipLimit;

						struct MakingSkillTable : public Serializer::ISerializable
						{
							typedef std::wstring SkillGroupName;

							typedef std::vector< ItemInfosForProduce > MadeItemInfos;

							typedef std::map< SkillGroupName, MadeItemInfos > MakingSkillGroups;

							MakingSkillGroups makingSkillGroups;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						} MakingSkillTable;

						struct ComposeItemTypes : public Serializer::ISerializable
						{
							typedef std::vector < std::pair < std::pair < uint8, uint8 >, std::vector < float > >/* , std::set<uint8> */ > ItemTypePointMap;
							ItemTypePointMap ItemTypePoint1Table;
							ItemTypePointMap ItemTypePoint2Table;
							ItemTypePointMap ItemTypePoint3Table;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						};
						std::map < uint8, ComposeItemTypes > ComposeItemTypes;

						struct ComposeGradePointTable : public Serializer::ISerializable
						{
							typedef std::vector < std::pair < std::pair < uint16, uint16 >, std::vector < float > >/* , std::set<uint8> */ > GradePointTablesMap;
							GradePointTablesMap GradePointTables;

						public: // Serializer::ISerializable impl
							void Serialize(Serializer::IStreamWriter& out) const;
							void Deserialize(Serializer::IStreamReader& in);
						};
						std::map < uint8, ComposeGradePointTable > ComposeGradePoints;
						void LoadXmlData();
						void LoadBinaryData();
					public: //Compose related Functions
						Lunia::XRated::Database::Info::ItemInfo* GetResult(std::pair<Lunia::XRated::Database::Info::ItemInfo*, Lunia::XRated::Database::Info::ItemInfo*> input);

					public:
						const Constants& GetConstants() const;
						void Init(bool bForceXml);
						//���� ���¿� ���� ����
						float RetrieveIdentifyValue(uint16 level, EnchantTargets::Type type, uint8 value) const;
						float RetrieveIdentifyRebirthedItemValue(uint16 sumOfValue, EnchantTargets::Type type, uint8 value, uint16 rebirthCount) const;
						//��ȭ ���¿� ���� ����
						const float4& RetrieveReinforceValue(uint16 level, uint8 reinforcementLv, XRated::Constants::EquipParts parts) const;
						const float4& RetrieveReinforceRebirthedItemValue(uint16 level, uint8 reinforcementLv, XRated::Constants::EquipParts parts, uint16 itemRebirthLimit) const;
						float RetrieveLightReinforceValue(uint16 level, uint8 lightReinforcementLv, XRated::Constants::EquipParts parts, int stat) const;
						float RetrieveLightReinforceValueForRebirth(uint16 level, uint8 lightReinforcementLv, XRated::Constants::EquipParts parts, int stat) const;
						const MagicalAttachInfo* RetrieveMagicalAttachValue(uint16 index) const;

						const bool IsProduceSkill(const std::wstring& skillGroupName) const;
						const bool RetrieveMakingSkillMadeItems(const std::wstring& skillGroupName, const uint32 skillLevel, ItemInfosForProduce& result) const;

						//���� ���� �������� �Լ���
						uint16 RetrieveJewelCntPerLevel(uint16 lv) const; //���� ���� ����ܰ躸���� ����
						uint16 RetrieveNextJewelCntPerLevel(uint16 lv) const; //���� ���� �����ܰ躸���� ����
						uint16 RetrieveMineralCntPerLevel(uint16 lv) const; //���� ���� ��ȭ���� ����
						uint16 RetrieveCatalystCntPerLevel(uint16 lv) const; //���� ���� �˸��� ����
						uint16 RetrieveJewelCntPerReinforcement(uint8 lv) const; //��ȭ�ܰ迡 ���� ����ܰ躸���� ����
						uint16 RetrieveNextJewelCntPerReinforcement(uint8 lv) const; //��ȭ�ܰ迡 ���� �����ܰ躸���� ����
						uint16 RetrieveMineralCntPerReinforcement(uint8 lv) const; //��ȭ�ܰ迡 ���� ��ȭ���� ����
						uint16 RetrieveCatalystCntPerReinforcement(uint8 lv) const; //��ȭ�ܰ迡 ���� �˸��� ����

						int GetReinforceRecoverFee() const;
						int GetLightReinforceRecoverFee() const;
						int GetResetIdentificationFee() const;
						int GetReinforceRecoverStoneCnt(uint16 lv, uint32 stoneHash = TempConstants::HashRecoverStone) const; // -1 : �� ������ �ش��ϴ� ������ �ؽð� �ƴϴ�.
						int GetLightReinforceRecoverStoneCnt(uint16 lv, uint32 stoneHash = TempConstants::HashRecoverStone) const; // -1 : �� ������ �ش��ϴ� ������ �ؽð� �ƴϴ�.
						int GetResetIdentificationScrollCnt(uint16 lv, uint32 scrollHash = TempConstants::HashResetScroll) const; // -1 : �� ������ �ش��ϴ� �簨���� �ؽð� �ƴϴ�.

						//���� Ȯ�� ���̺�
						const ProbabilityInfo* GetReinforcementProb(uint32 catalyst) const
						{
							uint32 hash = 0;
							if (catalyst != 0)
							{
								for (std::vector<CatalystTable>::const_iterator iter = Catalysts.begin(); iter != Catalysts.end(); ++iter)
								{
									if (iter->ItemHash == catalyst)
									{
										hash = iter->ProbabilityTableHash;

										break;
									}
								}

								if (0 == hash)
								{
									for (std::vector<CatalystTable>::const_iterator iter = CatalystsForAccessory.begin(); iter != CatalystsForAccessory.end(); ++iter)
									{
										if (iter->ItemHash == catalyst)
										{
											hash = iter->ProbabilityTableHash;

											break;
										}
									}

									if (0 == hash)
									{
										Logger::GetInstance().Error(L"There is no catalystInfo.");
									}
								}
							}
							std::vector<ProbabilityInfo>::const_iterator iter = Reinforcement.Probabilities.begin();
							std::vector<ProbabilityInfo>::const_iterator end = Reinforcement.Probabilities.end();
							while (iter != end) {
								if ((*iter).Hash == hash) {
									return &(*iter);
								}
								++iter;
							}
							return NULL;
						}

						/*
						const std::vector<float>* GetReinforcementDestroyProb(uint32 catalyst) const { return Reinforcement.Probability[0]; } //��ȭ - index ; ��ȭ �ܰ�
						const std::vector<float>* GetReinforcementSuccessProb(uint32 catalyst) const { return Reinforcement.Probability[1]; } //��ȭ - index ; ��ȭ �ܰ�
						const std::vector<float>* GetReinforcementGradeDownProb(uint32 catalyst) const { return Reinforcement.Probability[2]; } //��ȭ - index ; ��ȭ �ܰ�
						*/

						const std::vector<float>& GetIdentifyStatusTypeProb() const { return Identify.StatusTypeProbability; } // ���� - index ; ���� ����
						const std::vector<float>& GetIdentifyGradeProb() const { return Identify.GradeProbability; } // ���� - index ; �ܰ�

						const std::vector<float>& GetAdvancedIdentifyStatusTypeProb() const { return AdvancedIdentify.StatusTypeProbability; } // �ι�° �̻��� ���� - index ; ���� ����
						const std::vector<float>& GetAdvancedIdentifyGradeProb() const { return AdvancedIdentify.GradeProbability; } // �ι�° �̻��� ���� - index ; �ܰ�

						const std::vector<float>& GetCashIdentifyStatusTypeProb() const { return CashIdentify.StatusTypeProbability; } // �ι�° �̻��� ���� - index ; ���� ����
						const std::vector<float>& GetCashIdentifyGradeProb() const { return CashIdentify.GradeProbability; } // �ι�° �̻��� ���� - index ; �ܰ�

						const ProbabilityInfo* GetLightReinforcementProb(uint32 catalyst) const {
							uint32 hash = 0;
							if (catalyst != 0)
							{
								for (std::vector<CatalystTable>::const_iterator iter = Catalysts.begin(); iter != Catalysts.end(); ++iter)
								{
									if (iter->ItemHash == catalyst)
									{
										hash = iter->ProbabilityTableHash;

										break;
									}
								}

								if (0 == hash)
								{
									for (std::vector<CatalystTable>::const_iterator iter = CatalystsForAccessory.begin(); iter != CatalystsForAccessory.end(); ++iter)
									{
										if (iter->ItemHash == catalyst)
										{
											hash = iter->ProbabilityTableHash;

											break;
										}
									}

									if (0 == hash)
									{
										Logger::GetInstance().Error(L"There is no catalystInfo.");
									}
								}
							}
							std::vector<ProbabilityInfo>::const_iterator iter = LightReinforcement.Probabilities.begin();
							std::vector<ProbabilityInfo>::const_iterator end = LightReinforcement.Probabilities.end();
							while (iter != end) {
								if ((*iter).Hash == hash) {
									return &(*iter);
								}
								++iter;
							}
							return NULL;
						}
						/*
						const std::vector<float>* GetLightDestroyProb(uint32 catalyst) const { return LightReinforcement.Probability[0]; } //����ȭ - index ; ����ȭ �ܰ�
						const std::vector<float>* GetLightSuccessProb(uint32 catalyst) const { return LightReinforcement.Probability[1]; } //����ȭ - index ; ����ȭ �ܰ�
						const std::vector<float>* GetLightGradeDownProb(uint32 catalyst) const { return LightReinforcement.Probability[2]; } //����ȭ - index ; ����ȭ �ܰ�
						*/

						float GetExtractionJewelProb() const { return Extraction.Probability[0]; } //���� - ������ ���� Ȯ��
						float GetExtractionNextJewelProb() const { return Extraction.Probability[1]; } //���� - �����ܰ� ������ ���� Ȯ��
						float GetExtractionMineralProb() const { return Extraction.Probability[2]; } //���� - ������ ���� Ȯ��

						// �˸� Ȯ��
						float GetCatalystProbPerCnt(uint32 hash, int cnt) const {
							std::vector<CatalystTable>::const_iterator iter = Catalysts.begin();
							std::vector<CatalystTable>::const_iterator end = Catalysts.end();
							while (iter != end) {
								if ((*iter).ItemHash == hash) {
									const CatalystTable& Catalyst = *iter;
									if (cnt == 0 || Catalyst.ProbabilityPerCnt.empty()) return 0.0f; /* no catalyst, no additional probability */
									if (cnt >= static_cast<int>(Catalyst.ProbabilityPerCnt.size())) /* force to be last element */
										cnt = Catalyst.ProbabilityPerCnt.size();

									return Catalyst.ProbabilityPerCnt[cnt - 1];
								}
								++iter;
							}
							return 0.0f;
						}
						float GetCatalystProbPerLevel(uint32 hash, uint16 lv) const {
							std::vector<CatalystTable>::const_iterator iter = Catalysts.begin();
							std::vector<CatalystTable>::const_iterator end = Catalysts.end();
							while (iter != end) {
								if ((*iter).ItemHash == hash) {
									const CatalystTable& Catalyst = *iter;
									if (Catalyst.ProbabilityPerLevel.empty()) return 0.0f; /* unable to be or no data */
									if (lv >= Catalyst.ProbabilityPerLevel.size()) /* force to be last element */
										lv = static_cast<uint16>(static_cast<int>(Catalyst.ProbabilityPerLevel.size()) - 1);

									return Catalyst.ProbabilityPerLevel[lv];
								}
								++iter;
							}
							return 0.0f;
						}

						uint32 GetCatalystLimitCount(uint32 hash) const {
							std::vector<CatalystTable>::const_iterator iter = Catalysts.begin();
							std::vector<CatalystTable>::const_iterator end = Catalysts.end();
							while (iter != end) {
								if ((*iter).ItemHash == hash) {
									return (*iter).ProbabilityPerCnt.size();
								}
								++iter;
							}
							return 0;
						}

						uint32 GetCatalystMinCountForReinforce(uint32 hash, uint16 lv) const;
						uint32 GetCatalystMinCountForLightReinforce(uint32 hash, uint16 lv) const;

						uint32 GetCatalystMinCountForReinforceOfAccessory(uint32 hash, uint16 lv) const;
						uint32 GetCatalystMinCountForLightReinforceOfAccessory(uint32 hash, uint16 lv) const;

						// ������ ��꿡 �ʿ��� ���� ��ġ��
						uint32 GetRequirementFactorPerGrade(uint8 grade) const; //������ ��޿� ���� �䱸ġ
						uint32 GetRequirementFactorPerReinforcement(uint8 reinforceLv) const; //��ȭ ��޿� ���� �䱸ġ
						uint32 GetRequirementFactorPerLightReinforcement(uint8 lightLv) const; //����ȭ ��޿� ���� �䱸ġ

						//���ÿ� �ʿ��� �ʿ� ������ ����
						uint16 GetMineralCntPerLevel(uint16 lv) const;
						uint16 GetJewelCntPerLevel(uint16 lv) const;


						/* functions for alchemy */
						const AlchemyTable* GetAlchemyTable(const Info::ItemInfo* info/* source */) const; ///< @return NULL if the information doesn't exist

						/* Cash enchant */
						void SetRebirthCountEquipLimit(uint16 RebirthCountEquipLimit);
						bool IsCashEnchantAvailable(const CashEnchantInfo* info, uint16 level, uint16 storedLevel, uint16 rebirthCount, uint16 rebirthCountEquipLimit = 0);
						std::pair< CashEnchantInfo*, CashEnchantInfo* > GetCashEnchantInfo(uint32 id1, uint32 id2);
						void GetCashEnchantList(std::vector<uint32>& l) const;

					public: // Serializer::ISerializable impl
						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);


					};
				}
			}	
		}	
	}	
}

