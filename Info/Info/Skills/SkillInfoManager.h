#pragma once
#include "Skills.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class SkillInfoManager : public Serializer::ISerializable
				{
				public:
					typedef std::vector<SkillInfoList> SkillInfoVector;
					typedef std::vector<SkillCategoryList> SkillCategoryVector;

				protected:
					std::set<uint32, Less<uint32> > hashs;
					SkillInfoVector SkillInfoLists;
					SkillCategoryVector SkillCategorys;

					void LoadSkillInfo(const wchar_t* xml);
					void LoadSkillCategoryInfo(const wchar_t* xml);

				public:
					SkillInfoManager();
					virtual ~SkillInfoManager();

					void Init(bool bForceXml);
					void LoadXmlData();
					void LoadBinaryData();

					SkillCategoryList::CategoryList& GetCategoryList(uint32 hash);
					SkillCategoryList::CategoryList& GetCategoryList(const wchar_t* name);
					SkillInfoList& GetSkillInfoList(uint32 hash);
					SkillInfoList& GetSkillInfoList(const wchar_t* classType);
					SkillInfoList::SkillList& GetSkillList(uint32 hash);
					SkillInfoList::SkillList& GetSkillList(const wchar_t* name);
					SkillInfoList::SkillGroup* GetSkillGroupInfo(const std::wstring& classType, const std::wstring& skillGroup);
					SkillInfoList::SkillGroup* GetSkillGroupInfo(uint32 classType, uint32 skillGroup);
					SkillInfoList::SkillGroupList& GetSkillGroupList(const std::wstring& classType);
					SkillInfoList::SkillInfo* GetSkillInfo(const std::wstring& classType, const std::wstring& skillGroup, const std::wstring& name);
					SkillInfoList::SkillInfo* GetSkillInfo(const std::wstring& classType, const std::wstring& name);

				public:
					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}