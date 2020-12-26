#pragma once
#include "SkillInfoManager.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				SkillInfoManager::SkillInfoManager()
				{
				}

				SkillInfoManager::~SkillInfoManager()
				{
				}

				void SkillInfoManager::Init(bool bForceXmlData)
				{
					SkillCategorys.clear();
					SkillInfoLists.clear();

					if (bForceXmlData) {
						LoadXmlData();
					}
					else {
						LoadBinaryData();
					}

					for (SkillInfoVector::iterator i = SkillInfoLists.begin(); i != SkillInfoLists.end(); ++i)
						(*i).SetSkillInfo();

				}

				void SkillInfoManager::LoadXmlData()
				{
					LoadSkillCategoryInfo(L"Database/Skills/SkillCategory.xml");

					std::vector<std::wstring> tempList;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/Skills/SkillList.xml");
					reader->Read(L"SkillList", tempList);

					for (std::vector<std::wstring>::iterator i = tempList.begin(); i != tempList.end(); ++i)
						LoadSkillInfo(i->c_str());
				}

				void SkillInfoManager::LoadBinaryData()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/SkillInfos.b");
					reader->Read(L"SkillInfoManager", *this);
				}

				void SkillInfoManager::LoadSkillInfo(const wchar_t* xml)
				{
					SkillInfoVector tempSkillList;
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"SkillInfoLists", tempSkillList);
					SkillInfoLists.insert(SkillInfoLists.end(), tempSkillList.begin(), tempSkillList.end());

					SkillInfoVector::iterator it = tempSkillList.begin(), endIt = tempSkillList.end();
					for (size_t i = 0; i < tempSkillList.size(); ++i)
					{
						hashs.insert(tempSkillList[i].Hash);
					}
				}

				void SkillInfoManager::LoadSkillCategoryInfo(const wchar_t* xml)
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(xml);
					reader->Read(L"SkillCategorys", SkillCategorys);
				}

				SkillCategoryList::CategoryList& SkillInfoManager::GetCategoryList(uint32 hash)
				{
					SkillCategoryVector::iterator i = SkillCategorys.begin();
					for (; i != SkillCategorys.end(); ++i)
						if ((*i).Hash == hash) return (*i).Categorys;

					Logger::GetInstance().Warn(L"SkillInfoManager::GetCategoryList cannot found : {0}", hash);
					throw ;
				}

				SkillCategoryList::CategoryList& SkillInfoManager::GetCategoryList(const wchar_t* name)
				{
					return GetCategoryList(StringUtil::Hash(name));
				}

				//by wind
				SkillInfoList& SkillInfoManager::GetSkillInfoList(uint32 hash)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();
					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == hash) return (*i);

					Logger::GetInstance().Warn(L"SkillInfoManager::GetCategoryList cannot found : {0}", hash);
					throw;
				}

				SkillInfoList& SkillInfoManager::GetSkillInfoList(const wchar_t* classType)
				{
					return GetSkillInfoList(StringUtil::Hash(classType));
				}

				SkillInfoList::SkillList& SkillInfoManager::GetSkillList(uint32 hash)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();
					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == hash)
							return (*i).Skills;

					Logger::GetInstance().Warn(L"SkillInfoManager::GetSkillList cannot found : {0}", hash);
					throw;
				}

				SkillInfoList::SkillList& SkillInfoManager::GetSkillList(const wchar_t* name)
				{
					return GetSkillList(StringUtil::Hash(name));
				}

				//by wind
				SkillInfoList::SkillGroupList& SkillInfoManager::GetSkillGroupList(const std::wstring& classType)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();
					uint32 hashClassType;
					hashClassType = StringUtil::Hash(classType.c_str());

					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == hashClassType)
							return *((*i).GetSkillGroupList());

					Logger::GetInstance().Warn(L"SkillInfoManager::GetSkillGroupList cannot found : {0}", classType.c_str());
					throw;
				}

				SkillInfoList::SkillGroup* SkillInfoManager::GetSkillGroupInfo(const std::wstring& classType, const std::wstring& skillGroup)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();
					uint32 hashClassType, hashSkillGroup;
					hashClassType = StringUtil::Hash(classType.c_str());
					hashSkillGroup = StringUtil::Hash(skillGroup.c_str());

					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == hashClassType)
							return (*i).GetSkillGroup(hashSkillGroup);

					return NULL;
				}

				SkillInfoList::SkillGroup* SkillInfoManager::GetSkillGroupInfo(uint32 classType, uint32 skillGroup)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();

					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == classType)
							return (*i).GetSkillGroup(skillGroup);

					return NULL;
				}

				SkillInfoList::SkillInfo* SkillInfoManager::GetSkillInfo(const std::wstring& classType, const std::wstring& skillGroup, const std::wstring& name)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();
					uint32 hashClassType, hashSkillGroup;
					hashClassType = StringUtil::Hash(classType.c_str());
					hashSkillGroup = StringUtil::Hash(skillGroup.c_str());

					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == hashClassType)
							return (*i).GetSkillGroup(hashSkillGroup)->GetSkillInfo(name);

					return NULL;
				}

				SkillInfoList::SkillInfo* SkillInfoManager::GetSkillInfo(const std::wstring& classType, const std::wstring& name)
				{
					SkillInfoVector::iterator i = SkillInfoLists.begin();
					uint32 hashClassType;
					hashClassType = StringUtil::Hash(classType.c_str());

					for (; i != SkillInfoLists.end(); ++i)
						if ((*i).Hash == hashClassType)
							return (*i).GetSkillInfo(name);

					return NULL;
				}

				void SkillInfoManager::Serialize(Serializer::IStreamWriter& out) const
				{
					out.Begin(L"XRated::Database::Info::SkillInfoManager");
					out.Write(L"SkillInfoLists", SkillInfoLists);
					out.Write(L"SkillCategorys", SkillCategorys);
				}

				void SkillInfoManager::Deserialize(Serializer::IStreamReader& in)
				{
					in.Begin(L"XRated::Database::Info::SkillInfoManager");
					in.Read(L"SkillInfoLists", SkillInfoLists);
					in.Read(L"SkillCategorys", SkillCategorys);
				}

			}
		}
	}
}