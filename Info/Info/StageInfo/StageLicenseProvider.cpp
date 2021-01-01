#pragma once
#include "StageLicenseProvider.h"
#include <Core/Resource/Resource.h>
namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				StageLicenseProvider::AcquireLicenseCondition::AcquireLicenseCondition() : Level(1), RebirthCount(0) {}
				StageLicenseProvider::AcquireLicenseCondition::AcquireLicenseCondition(uint16 level, uint16 rebirthCount)
					: Level(level), RebirthCount(rebirthCount) {}
				bool StageLicenseProvider::AcquireLicenseCondition::operator<(const AcquireLicenseCondition& rhs) const
				{
					if (this->Level < rhs.Level)
						return true;

					if (this->Level == rhs.Level && this->RebirthCount < rhs.RebirthCount)
						return true;

					return false;
				}
				bool StageLicenseProvider::AcquireLicenseCondition::operator==(const AcquireLicenseCondition& rhs) const
				{
					if (this->Level == rhs.Level
						&& this->RebirthCount == rhs.RebirthCount) return true;
					else return false;
				}
				bool StageLicenseProvider::AcquireLicenseCondition::operator!=(const AcquireLicenseCondition& rhs) const
				{
					return !(*this == rhs);
				}

				typedef std::vector<uint32> StageGroups;
				typedef std::map<uint32, StageGroups> StageLicenseMap;

				void StageLicenseProvider::AcquireLicenseCondition::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StageInfoManager::StageLicenseProvider::AcquireLicenseCondition");
					out.Write(L"Level", Level);
					out.Write(L"RebirthCount", RebirthCount);
				}

				void StageLicenseProvider::AcquireLicenseCondition::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StageInfoManager::StageLicenseProvider::AcquireLicenseCondition");
					in.Read(L"Level", Level);
					in.Read(L"RebirthCount", RebirthCount);
				}
				void StageLicenseProvider::Init(bool bForceXmlData)
				{
					if (bForceXmlData) //Xml loading
						LoadXmlData();
					else //Binary loading
						LoadBinaryData();
				}

				void StageLicenseProvider::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StageLicenseProvider.b");
					reader->Read(L"StageLicenseProvider", *this);

					PostProcess(0);
				}

				void StageLicenseProvider::PostProcess(bool bForceXml)
				{
					for (StageLicenseMap::iterator it = licenseMap.begin(), endIt = licenseMap.end(); it != endIt; ++it)
					{
						for (StageLicenseList::iterator licenseIt = it->second.begin(), endLicenseIt = it->second.end(); licenseIt != endLicenseIt; ++licenseIt)
						{
							levelMap[*licenseIt] = it->first;
						}
					}

					if (bForceXml)
					{
						StageLicenseMap						templicenseMap;
						std::vector<XRated::StageLicense>	lastlicenses;
						uint16 maxRebirthCount = 100;

						{
							for (int i = 1; i <= XRated::Constants::PlayerMaxLevel; ++i)
							{
								StageLicenseMap::iterator itr = licenseMap.find(AcquireLicenseCondition(i, 0));
								if (itr != licenseMap.end())
								{
									for (StageLicenseList::iterator licenseIt = itr->second.begin(), endLicenseIt = itr->second.end(); licenseIt != endLicenseIt; ++licenseIt)
									{
										std::vector<XRated::StageLicense>::iterator lastlicenseItr = std::find_if(lastlicenses.begin(), lastlicenses.end(), StageLocation::FinderOnlyStageGroupHash(licenseIt->StageGroupHash));
										if (lastlicenseItr != lastlicenses.end())
										{
											if (lastlicenseItr->Level < licenseIt->Level) lastlicenseItr->Level = licenseIt->Level;
										}
										else
										{
											lastlicenses.push_back(*licenseIt);
										}
									}
								}

								templicenseMap[AcquireLicenseCondition(i, 0)] = lastlicenses;
							}
						}

						{
							lastlicenses.clear();
							StageLicenseMap::iterator tempItr = templicenseMap.find(AcquireLicenseCondition(1, 0));
							if (tempItr != templicenseMap.end())
								lastlicenses = tempItr->second;

							for (int k = 1; k <= maxRebirthCount; ++k)
							{
								StageLicenseMap::iterator itr = licenseMap.find(AcquireLicenseCondition(1, k));
								if (itr != licenseMap.end())
								{
									for (StageLicenseList::iterator licenseIt = itr->second.begin(), endLicenseIt = itr->second.end(); licenseIt != endLicenseIt; ++licenseIt)
									{
										std::vector<XRated::StageLicense>::iterator lastlicenseItr = std::find_if(lastlicenses.begin(), lastlicenses.end(), StageLocation::FinderOnlyStageGroupHash(licenseIt->StageGroupHash));
										if (lastlicenseItr != lastlicenses.end())
										{
											if (lastlicenseItr->Level < licenseIt->Level) lastlicenseItr->Level = licenseIt->Level;
										}
										else
										{
											lastlicenses.push_back(*licenseIt);
										}
									}
								}

								templicenseMap[AcquireLicenseCondition(1, k)] = lastlicenses;
							}
						}


						{
							for (int i = 2; i <= XRated::Constants::PlayerMaxLevel; ++i)
							{
								for (int k = 1; k <= maxRebirthCount; ++k)
								{
									lastlicenses.clear();
									StageLicenseMap::iterator tempItr = templicenseMap.find(AcquireLicenseCondition(i - 1, k));
									if (tempItr != templicenseMap.end())
									{
										lastlicenses = tempItr->second;
									}
									tempItr = templicenseMap.find(AcquireLicenseCondition(i, k - 1));
									if (tempItr != templicenseMap.end())
									{
										for (StageLicenseList::iterator licenseIt = tempItr->second.begin(), endLicenseIt = tempItr->second.end(); licenseIt != endLicenseIt; ++licenseIt)
										{
											std::vector<XRated::StageLicense>::iterator lastlicenseItr = std::find_if(lastlicenses.begin(), lastlicenses.end(), StageLocation::FinderOnlyStageGroupHash(licenseIt->StageGroupHash));
											if (lastlicenseItr != lastlicenses.end())
											{
												if (lastlicenseItr->Level < licenseIt->Level) lastlicenseItr->Level = licenseIt->Level;
											}
											else
											{
												lastlicenses.push_back(*licenseIt);
											}
										}
									}

									StageLicenseMap::iterator itr = licenseMap.find(AcquireLicenseCondition(i, k));
									if (itr != licenseMap.end())
									{
										for (StageLicenseList::iterator licenseIt = itr->second.begin(), endLicenseIt = itr->second.end(); licenseIt != endLicenseIt; ++licenseIt)
										{
											std::vector<XRated::StageLicense>::iterator lastlicenseItr = std::find_if(lastlicenses.begin(), lastlicenses.end(), StageLocation::FinderOnlyStageGroupHash(licenseIt->StageGroupHash));
											if (lastlicenseItr != lastlicenses.end())
											{
												if (lastlicenseItr->Level < licenseIt->Level) lastlicenseItr->Level = licenseIt->Level;
											}
											else
											{
												lastlicenses.push_back(*licenseIt);
											}
										}
									}

									templicenseMap[AcquireLicenseCondition(i, k)] = lastlicenses;
								}
							}
						}

						licenseMap.clear();
						licenseMap = templicenseMap;
					}

				}

				const StageLicenseProvider::AcquireLicenseCondition* StageLicenseProvider::GetAcceptLicenseLevel(const StageLicense& license)
				{
					StageLicenseLevelMap::const_iterator itr = levelMap.find(license);
					if (itr == levelMap.end())
					{
						return NULL;
					}

					return &itr->second;
				}

				void StageLicenseProvider::LoadXmlData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/StageLicenseProvider.xml");
					reader->Read(L"LicenseMap", licenseMap);

					PostProcess(1);
				}

				void StageLicenseProvider::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"AllM::XRated::Database::Info::StageInfoManager::StageLicenseProvider");
					out.Write(L"LicenseMap", licenseMap);
				}

				void StageLicenseProvider::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"AllM::XRated::Database::Info::StageInfoManager::StageLicenseProvider");
					in.Read(L"LicenseMap", licenseMap);
				}

				const StageLicenseProvider::StageLicenseList* StageLicenseProvider::GetStageLicenseList(uint16 level, uint16 rebirthCount)
				{
					StageLicenseMap::const_iterator itr = licenseMap.find(AcquireLicenseCondition(level, rebirthCount));
					if (itr == licenseMap.end())
					{
						return 0;
					}

					return &(itr->second);
				}

				bool StageLicenseProvider::IsEnableJoinToStage(uint16 level, uint16 rebirthCount, const StageLicense& targetStage) const
				{
					StageLicenseMap::const_iterator itr = licenseMap.find(AcquireLicenseCondition(level, rebirthCount));
					if (itr != licenseMap.end())
					{
						std::vector<XRated::StageLicense>::const_iterator licenseItr = std::find_if(itr->second.begin(), itr->second.end(), StageLocation::Finder(targetStage.StageGroupHash, targetStage.Level));
						if (licenseItr != itr->second.end())
							return true;
					}

					return false;
				}

				bool StageLicenseProvider::IsExistLicense(const StageLicense& targetStage) const
				{
					uint16 maxRebirthCount = 100;
					for (int k = 0; k <= maxRebirthCount; ++k)
					{
						StageLicenseMap::const_iterator itr = licenseMap.find(AcquireLicenseCondition(XRated::Constants::PlayerMaxLevel, k));
						if (itr != licenseMap.end())
						{
							std::vector<XRated::StageLicense>::const_iterator licenseItr = std::find_if(itr->second.begin(), itr->second.end(), StageLocation::Finder(targetStage.StageGroupHash, targetStage.Level));
							if (licenseItr != itr->second.end())
								return true;
						}
					}

					return false;
				}

				std::map<StageLicenseProvider::AcquireLicenseCondition, StageLicenseProvider::StageLicenseList>& StageLicenseProvider::GetStageLicenseMap()
				{
					return licenseMap;
				}
			}
		}
	}
}
