#ifndef StageLicenseProvider_H
#define StageLicenseProvider_H
#pragma once
#ifndef Serializer_H
#include <Core/Serializer/Serializer.h>
#endif // !Serializer_H
#ifndef GameConstants_H
#include <Core/GameConstants.h>
#endif // !GameConstants_H

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				class StageLicenseProvider : public Serializer::ISerializable {
				public:
					struct AcquireLicenseCondition
					{
						uint16 Level;
						uint16 RebirthCount;

						AcquireLicenseCondition();
						AcquireLicenseCondition(uint16 level, uint16 rebirthCount);

						void Serialize(Serializer::IStreamWriter& out) const;
						void Deserialize(Serializer::IStreamReader& in);

						bool operator<(const AcquireLicenseCondition& rhs) const;

						bool operator==(const AcquireLicenseCondition& rhs) const;

						bool operator!=(const AcquireLicenseCondition& rhs) const;
					};

				public:
					typedef std::vector<StageLicense> StageLicenseList;
					typedef std::map<AcquireLicenseCondition, StageLicenseList> StageLicenseMap;
					typedef std::map<StageLicense, AcquireLicenseCondition> StageLicenseLevelMap;

					void Init(bool bForceXml);
					void LoadXmlData();
					virtual void LoadBinaryData();
					void PostProcess(bool bForceXml);

					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);

					const StageLicenseList* GetStageLicenseList(uint16 level, uint16 rebirthCount);
					const AcquireLicenseCondition* GetAcceptLicenseLevel(const StageLicense& license);

					bool IsEnableJoinToStage(uint16 level, uint16 rebirthCount, const StageLicense& targetStage) const;
					bool IsExistLicense(const StageLicense& targetStage) const;
					std::map<AcquireLicenseCondition, StageLicenseList>& GetStageLicenseMap();

				protected:
					StageLicenseMap licenseMap;
					StageLicenseLevelMap levelMap;
				};
			}
		}
	}
}
#endif // !StageLicenseProvider_H