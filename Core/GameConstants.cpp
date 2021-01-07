#include <Core/GameConstants.h>


namespace Lunia {
	namespace XRated {
		const int Constants::LocaleSpecificSetting::COUNTRY_CODE = Constants::LocaleSpecificSetting::CountryCode::KR;
		const int Constants::LocaleSpecificSetting::WRAPTYPE = WrapType::CharacterWrap;
		const float Mail::FeeFactor = 0.05f;
		const uint32 Mail::Limit::GoldWithoutStamp = 1000000;
		const uint8 Mail::Limit::ItemWithoutStamp = 1;
		const int Mail::Limit::TARGETLEN_MAX = 24;
		const float Constants::MapTileSizeWidth = 3.2f;
		const float Constants::MapTileSizeHeight = 4.525f;
		const float Constants::PIXELPERMAYA = 7.4f;
		const float Constants::CampfireDelayCooperative = 10.0f;
		const float Constants::CampfireDelayPVP = 10.0f;
		const float Constants::Tick = 0.5f;
		const float Constants::PvPLimitTime = 1.5f;
		const float3 Constants::DirectionF::LeftDown(-0.7071f, 0, -0.7071f);
		const float3 Constants::DirectionF::Down(0, 0, -1);
		const float3 Constants::DirectionF::RightDown(0.7071f, 0, -0.7071f);
		const float3 Constants::DirectionF::Left(-1, 0, 0);
		const float3 Constants::DirectionF::Right(1, 0, 0);
		const float3 Constants::DirectionF::LeftUp(-0.7071f, 0, 0.7071f);
		const float3 Constants::DirectionF::Up(0, 0, 1);
		const float3 Constants::DirectionF::RightUp(0.7071f, 0, 0.7071f);
		const float Constants::GhostRules::tGhostWhenInstantPop = 3.0f;
		const float Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec10 = 10.0f;
		const float Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec5 = 5.0f;
		const float Constants::GhostRules::tGhostWhenAutoTimeRevive_Sec3 = 3.0f;
		const DateTime::Date Quest::NotUsedDate = DateTime::Date(2001, 1, 1);
		const float Gamble::SlimeRace::BettingLimitInSec = 10.0f;
		const float Gamble::SlimeRace::Constants::Chip2MoneyConversionTax = 0.05f;

		const float3 Constants::Familiar::FormationPosition[Constants::Familiar::Formation::FormationCount][Constants::Familiar::MaxCount] = {

			{float3(-15,0,-15),float3(15,0,-15),float3(6,0,-6)},
			{float3(0,0,-15),float3(0,0,15),float3(6,0,-6)},
			{float3(-15,0,0),float3(15,0,0),float3(6,0,-6)},
			{float3(-15,0,15),float3(15,0,15),float3(6,0,-6)}
		};
		const int    Constants::Familiar::MaxCountForEachType[TypeCount] = { 0, 1, 1, 2 };
	
		const float Fishing::CorkRelativePosition = 30.f;

		const Constants::Direction Constants::DirectionF::GetDirectionEnum(const float3& dir)
		{
			if (dir.x == 0) {
				if (dir.z < 0)
					return Constants::Direction::Down;
				else
					return Constants::Direction::Up;
			}
			else if (dir.x < 0) {
				if (dir.z == 0)
					return Constants::Direction::Left;
				else if (dir.z < 0)
					return Constants::Direction::LeftDown;
				else
					return Constants::Direction::LeftUp;
			}
			else {
				if (dir.z == 0)
					return Constants::Direction::Right;
				else if (dir.z < 0)
					return Constants::Direction::RightDown;
				else
					return Constants::Direction::RightUp;
			}
		}

		const float3& Constants::DirectionF::GetDirection(Constants::Direction dir)
		{
			switch (dir)
			{
			case Constants::Direction::LeftDown:
				return LeftDown;
			case Constants::Direction::Down:
				return Down;
			case Constants::Direction::RightDown:
				return RightDown;
			case Constants::Direction::Left:
				return Left;
			case Constants::Direction::Right:
				return Right;
			case Constants::Direction::LeftUp:
				return LeftUp;
			case Constants::Direction::Up:
				return Up;
			case Constants::Direction::RightUp:
				return RightUp;
			case Constants::Direction::None:
				break;
			}
			return Down;
		}

		const float3 Constants::DirectionF::GetDirection(const float3& dir)
		{
			if (dir.x == 0) {
				if (dir.z < 0)
					return Constants::DirectionF::Down;
				else
					return Constants::DirectionF::Up;
			}
			else if (dir.x < 0) {
				if (dir.z == 0)
					return Constants::DirectionF::Left;
				else if (dir.z < 0)
					return Constants::DirectionF::LeftDown;
				else
					return Constants::DirectionF::LeftUp;
			}
			else {
				if (dir.z == 0)
					return Constants::DirectionF::Right;
				else if (dir.z < 0)
					return Constants::DirectionF::RightDown;
				else
					return Constants::DirectionF::RightUp;
			}
		}

		const Constants::Direction Constants::DirectionF::GetRealDirection(Constants::Direction myDir, Constants::Direction relativeD)
		{
			//��ġ� �ũ� �ΰ�� ǽ��ġ��� ��ġ� �� ��´.
			//��ġ�� �ε���ȭ �ؼ� �� ��ϸ ��� �� ���ϰ��, ���� �̺�� ���� ϼ ���.
			switch (myDir) {
			case Constants::Direction::Up:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::Up;
				case Constants::Direction::LeftUp: return Constants::Direction::LeftUp;
				case Constants::Direction::Left: return Constants::Direction::Left;
				case Constants::Direction::LeftDown: return Constants::Direction::LeftDown;
				case Constants::Direction::Down: return Constants::Direction::Down;
				case Constants::Direction::RightDown: return Constants::Direction::RightDown;
				case Constants::Direction::Right: return Constants::Direction::Right;
				case Constants::Direction::RightUp: return Constants::Direction::RightUp;
				default: return Constants::Direction::Up;
				}
				break;
			case Constants::Direction::LeftUp:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::LeftUp;
				case Constants::Direction::LeftUp: return Constants::Direction::Left;
				case Constants::Direction::Left: return Constants::Direction::LeftDown;
				case Constants::Direction::LeftDown: return Constants::Direction::Down;
				case Constants::Direction::Down: return Constants::Direction::RightDown;
				case Constants::Direction::RightDown: return Constants::Direction::Right;
				case Constants::Direction::Right: return Constants::Direction::RightUp;
				case Constants::Direction::RightUp: return Constants::Direction::Up;
				default: return Constants::Direction::LeftUp;
				}
				break;
			case Constants::Direction::Left:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::Left;
				case Constants::Direction::LeftUp: return Constants::Direction::LeftDown;
				case Constants::Direction::Left: return Constants::Direction::Down;
				case Constants::Direction::LeftDown: return Constants::Direction::RightDown;
				case Constants::Direction::Down: return Constants::Direction::Right;
				case Constants::Direction::RightDown: return Constants::Direction::RightUp;
				case Constants::Direction::Right: return Constants::Direction::Up;
				case Constants::Direction::RightUp: return Constants::Direction::LeftUp;
				default: return Constants::Direction::Left;
				}
				break;
			case Constants::Direction::LeftDown:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::LeftDown;
				case Constants::Direction::LeftUp: return Constants::Direction::Down;
				case Constants::Direction::Left: return Constants::Direction::RightDown;
				case Constants::Direction::LeftDown: return Constants::Direction::Right;
				case Constants::Direction::Down: return Constants::Direction::RightUp;
				case Constants::Direction::RightDown: return Constants::Direction::Up;
				case Constants::Direction::Right: return Constants::Direction::LeftUp;
				case Constants::Direction::RightUp: return Constants::Direction::Left;
				default: return Constants::Direction::LeftDown;
				}
				break;
			case Constants::Direction::Down:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::Down;
				case Constants::Direction::LeftUp: return Constants::Direction::RightDown;
				case Constants::Direction::Left: return Constants::Direction::Right;
				case Constants::Direction::LeftDown: return Constants::Direction::RightUp;
				case Constants::Direction::Down: return Constants::Direction::Up;
				case Constants::Direction::RightDown: return Constants::Direction::LeftUp;
				case Constants::Direction::Right: return Constants::Direction::Left;
				case Constants::Direction::RightUp: return Constants::Direction::LeftDown;
				default: return Constants::Direction::Down;
				}
				break;
			case Constants::Direction::RightDown:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::RightDown;
				case Constants::Direction::LeftUp: return Constants::Direction::Right;
				case Constants::Direction::Left: return Constants::Direction::RightUp;
				case Constants::Direction::LeftDown: return Constants::Direction::Up;
				case Constants::Direction::Down: return Constants::Direction::LeftUp;
				case Constants::Direction::RightDown: return Constants::Direction::Left;
				case Constants::Direction::Right: return Constants::Direction::LeftDown;
				case Constants::Direction::RightUp: return Constants::Direction::Down;
				default: return Constants::Direction::RightDown;
				}
				break;
			case Constants::Direction::Right:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::Right;
				case Constants::Direction::LeftUp: return Constants::Direction::RightUp;
				case Constants::Direction::Left: return Constants::Direction::Up;
				case Constants::Direction::LeftDown: return Constants::Direction::LeftUp;
				case Constants::Direction::Down: return Constants::Direction::Left;
				case Constants::Direction::RightDown: return Constants::Direction::LeftDown;
				case Constants::Direction::Right: return Constants::Direction::Down;
				case Constants::Direction::RightUp: return Constants::Direction::RightDown;
				default: return Constants::Direction::Right;
				}
				break;
			case Constants::Direction::RightUp:
				switch (relativeD) {
				case Constants::Direction::Up: return Constants::Direction::RightUp;
				case Constants::Direction::LeftUp: return Constants::Direction::Up;
				case Constants::Direction::Left: return Constants::Direction::LeftUp;
				case Constants::Direction::LeftDown: return Constants::Direction::Left;
				case Constants::Direction::Down: return Constants::Direction::LeftDown;
				case Constants::Direction::RightDown: return Constants::Direction::Down;
				case Constants::Direction::Right: return Constants::Direction::RightDown;
				case Constants::Direction::RightUp: return Constants::Direction::Right;
				default: return Constants::Direction::RightUp;
				}
				break;
			}
			Logger::GetInstance().Info("[Constants::DirectionF::GetRealDirection] Unknown Direction ({0}/{1})", (int)myDir, (int)relativeD);
			return Constants::Direction::Up;
		}

		Constants::DamageType Constants::StringToDmgType(const std::wstring& str)
		{
			if (str == L"FIRE")
				return Constants::DamageType::FIRE;
			else if (str == L"WATER")
				return Constants::DamageType::WATER;
			else if (str == L"ICE")
				return Constants::DamageType::ICE;
			else if (str == L"LIGHTNING")
				return Constants::DamageType::LIGHTNING;
			else if (str == L"LAND")
				return Constants::DamageType::LAND;
			else if (str == L"WIND")
				return Constants::DamageType::WIND;
			else if (str == L"POISON")
				return Constants::DamageType::POISON;
			else if (str == L"LIGHT")
				return Constants::DamageType::LIGHT;
			else if (str == L"CURSE")
				return Constants::DamageType::CURSE;
			else if (str == L"PHYSICAL")
				return Constants::DamageType::PHYSICAL;
			else if (str == L"INDEPENDENCE")
				return Constants::DamageType::INDEPENDENCE;
			else {
				//throw Exception(ALLM_EXCEPTION((L"Warning : Not Exist Constants::DamageType : %s", str.c_str())));
				Logger::GetInstance().Info("Warning : Not Exist Constants::DamageType : {0}", StringUtil::ToASCII(str.c_str()));
				throw(fmt::format(L"Warning : Not Exist Constants::DamageType : {0}", str.c_str()));
			}
		}

		uint64 XRated::Mail::CalculatePostalCharge(uint64 money, uint16 itemCount)
		{
			return static_cast<uint64>(Mail::MailFee + money * Mail::FeeFactor + itemCount * Mail::ItemDeliverFee);
		}

		uint32 Constants::GetClassHash(ClassType type)
		{
			const static uint32 classHash[17] = {
				7120,		//Eir
				637401,		//Sieg
				5672366,	//Dainn
				21518,		//Tia
				61041505,	//SlimeCharacter
				182804,		//Dacy
				12643174,	//Krieg
				57369345,	//IceGirlCharacter
				46469254,	//ElfArcherCharacter
				4357310,	//BountyHunterCharacter
				58604409,	//BardCharacter
				1307419,	//DualWieldCharacter
				51352876,	//FighterCharacter
				7494098,	//DarkEirCharacter
				18654328,	//ArutaCharacter
				26017521,  //GaonCharacter
				62917719	//IrisCharacter
			};

			if (type < ClassType::Healer || type >= ClassType::NumberOfClassTypes)
			{
				//throw Exception(ALLM_EXCEPTION((L"Warring : Constants::GetClassHash : %d", (int)type)));
				Logger::GetInstance().Info("Warring : Constants::GetClassHash : {0}", (int)type);
				throw(fmt::format(L"Warring : Constants::GetClassHash : {0}", (int)type));
				//return classHash[0];
			}

			return classHash[(int)type];
		}

		float XRated::Gamble::SlimeRace::GetDividendRate(BettingPosition position)
		{
			switch (position)
			{
			case Position::One:
			case Position::Two:
			case Position::Three:
			case Position::Four:
			case Position::Six:
			case Position::Seven:
			case Position::Eight:
			case Position::Nine:
				return 8.f;

			case Position::Red:
			case Position::Black:
			case Position::Low:
			case Position::High:
				return 2.f;

			case Position::StreetBets123:
			case Position::StreetBets789:
				return 2.5f;
			case Position::StreetBets456:
				return 3.5f;
			case Position::InsideFive:
				return 30.f;
			}
			return 0.0f;
		}

		void XRated::Gamble::BettingState::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Gamble::BettingInfo");
			out.Write(L"RecentRate",RecentRate);
			out.Write(L"MyChips",MyChips);
			out.Write(L"TotalChips",TotalChips);
		}
		void XRated::Gamble::BettingState::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Gamble::BettingInfo");
			in.Read(L"RecentRate", RecentRate);
			in.Read(L"MyChips", MyChips);
			in.Read(L"TotalChips", TotalChips);
		}

		bool Constants::IsDefaultClass(ClassType type)
		{
			switch (type)
			{
				case ClassType::Healer: case ClassType::Knight: case ClassType::Wizard: case ClassType::Thief: 
				case ClassType::DollMaster: case ClassType::DarkTemplar: case ClassType::DarkEir: 
				case ClassType::Aruta: case ClassType::Gaon: case ClassType::Iris:
					return true;
			}

			return false;
		}

		const wchar_t* Constants::GetClassStr(ClassType type)
		{
			if (type >= ClassType::Genus_1 && type < ClassType::NumberOfGenusClassTypes)
			{
				const static std::wstring genusClass[] = {
					  L"Genus_1"
					, L"Genus_2"
					, L"Genus_3"
					, L"Genus_4"
					, L"Genus_5"
					, L"Genus_6"
					, L"Genus_7"
					, L"Genus_8"
					, L"Genus_9"
					, L"Genus_10"
					, L"Genus_11"
					, L"Genus_12"
					, L"Genus_13"
					, L"Genus_14"
					, L"Genus_15"
					, L"Genus_16"
					, L"Genus_17"
					, L"Genus_18"
					, L"Genus_19"
					, L"Genus_20"
					, L"AnyGenus"
				};
				return genusClass[static_cast<int>(type) - static_cast<int>(ClassType::Genus_1)].c_str();
			}
			if (type < ClassType::Healer || type >= ClassType::NumberOfClassTypes)
			{
				Logger::GetInstance().Info("Warring : Constants::GetClassStr : {0}", (int)type);
				throw(fmt::format(L"Warring : Constants::GetClassHash : {0}", (int)type));
				//return L"Eir";
			}
			const static std::wstring classStr[17] = {
				L"Eir",
				L"Sieg",
				L"Dainn",
				L"Tia",
				L"SlimeCharacter",
				L"Dacy",L"Krieg",
				L"IceGirlCharacter",
				L"ElfArcherCharacter",
				L"BountyHunterCharacter",
				L"BardCharacter",
				L"DualWieldCharacter",
				L"FighterCharacter",
				L"DarkEirCharacter",
				L"ArutaCharacter",
				L"GaonCharacter",
				L"IrisCharacter"
			};
			return classStr[(int)type].c_str();
		}
		const std::wstring Constants::GetClassLocaleStr(ClassType type)
		{
			std::wstring value = L"$UI.Global.CharacterClass.";
			value += GetClassStr(type);
			value += L"$";
			return value;
		}

		bool Constants::IsEquippable(Equipment e1, EquipParts e2)
		{
			if (e1 == Equipment::Body && e2 == EquipParts::CHEST) return true;
			if (e1 == Equipment::Head && e2 == EquipParts::HEAD) return true;
			if (e1 == Equipment::Weapon && e2 == EquipParts::WEAPON) return true;
			if (e1 == Equipment::Legs && e2 == EquipParts::LEG) return true;
			if (e1 == Equipment::Gloves && e2 == EquipParts::HAND) return true;
			if (e1 == Equipment::Shoes && e2 == EquipParts::FOOT) return true;
			if (e1 == Equipment::Support && e2 == EquipParts::SUPPORT) return true;
			if (e1 == Equipment::Necklace && e2 == EquipParts::NECKLACE) return true;
			if ((e1 == Equipment::Ring1 || e1 == Equipment::Ring2) && e2 == EquipParts::RING) return true;
			if ((e1 == Equipment::Earing1 || e1 == Equipment::Earing2) && e2 == EquipParts::EARING) return true;

			if (e1 == Equipment::CashBody && e2 == EquipParts::CASH_CHEST) return true;
			if (e1 == Equipment::CashHead && e2 == EquipParts::CASH_HEAD) return true;
			if (e1 == Equipment::CashWeapon && e2 == EquipParts::CASH_WEAPON) return true;
			if (e1 == Equipment::CashLegs && e2 == EquipParts::CASH_LEG) return true;
			if (e1 == Equipment::CashGloves && e2 == EquipParts::CASH_HAND) return true;
			if (e1 == Equipment::CashShoes && e2 == EquipParts::CASH_FOOT) return true;
			if (e1 == Equipment::CashSupport && e2 == EquipParts::CASH_SUPPORT) return true;
			if (e1 == Equipment::CashMask && e2 == EquipParts::CASH_MASK) return true;
			if (e1 == Equipment::CashBack && e2 == EquipParts::CASH_BACK) return true;
			if (e1 == Equipment::CashHip && e2 == EquipParts::CASH_HIP) return true;
			if (e1 == Equipment::CashEtc1 && e2 == EquipParts::CASH_ETC1) return true;
			if (e1 == Equipment::CashEtc2 && e2 == EquipParts::CASH_ETC2) return true;

			if (e1 == Equipment::FameAdj && e2 == EquipParts::FAMEADJ) return true;
			if (e1 == Equipment::FameNoun && e2 == EquipParts::FAMENOUN) return true;
			if (e1 == Equipment::Pet && e2 == EquipParts::PET) return true;
			if (e1 == Equipment::Pet_Mask && e2 == EquipParts::PET_MASK) return true;
			if (e1 == Equipment::Pet_Etc1 && e2 == EquipParts::PET_ETC1) return true;
			if (e1 == Equipment::Pet_Etc2 && e2 == EquipParts::PET_ETC2) return true;
			return false;
		}

		bool Constants::IsAccessory(EquipParts part)
		{
			switch (part)
			{
			case EquipParts::SUPPORT:
			case EquipParts::NECKLACE:
			case EquipParts::RING:
			case EquipParts::EARING:
				return true;
			}

			return false;
		}

		Constants::Equipment Constants::GetEquipPosition(EquipParts part)
		{
			switch (part)
			{
			case WEAPON:			return Equipment::Weapon;
			case HEAD:				return Equipment::Head;
			case CHEST:				return Equipment::Body;
			case LEG:				return Equipment::Legs;
			case HAND:				return Equipment::Gloves;
			case FOOT:				return Equipment::Shoes;
			case SUPPORT:			return Equipment::Support;
			case NECKLACE:			return Equipment::Necklace;
			case RING:				return Equipment::Ring1;
			case EARING:			return Equipment::Earing1;
			case CASH_WEAPON:		return Equipment::CashWeapon;
			case CASH_HEAD:			return Equipment::CashHead;
			case CASH_CHEST:		return Equipment::CashBody;
			case CASH_LEG:			return Equipment::CashLegs;
			case CASH_HAND:			return Equipment::CashGloves;
			case CASH_FOOT:			return Equipment::CashShoes;
			case CASH_SUPPORT:		return Equipment::CashSupport;
			case CASH_MASK:			return Equipment::CashMask;
			case CASH_BACK:			return Equipment::CashBack;
			case CASH_HIP:			return Equipment::CashHip;
			case CASH_ETC1:			return Equipment::CashEtc1;
			case CASH_ETC2:			return Equipment::CashEtc2;
			case FAMEADJ:			return Equipment::FameAdj;
			case FAMENOUN:			return Equipment::FameNoun;
			case PET:			return Equipment::Pet;
			case PET_MASK:		return Equipment::Pet_Mask;
			case PET_ETC1:		return Equipment::Pet_Etc1;
			case PET_ETC2:		return Equipment::Pet_Etc2;
			}
			Logger::GetInstance().Info("can not found Equip position: {0}", static_cast<int>(part));
			throw(fmt::format(L"can not found Equip position: {0}", static_cast<int>(part)));
		}
		Constants::EquipFlag Constants::GetEquipFlag(Constants::EquipParts part)
		{
			switch (part)
			{
			case EquipParts::CASH_WEAPON:		return EquipFlag::CASH_Weapon;
			case EquipParts::CASH_HEAD:			return EquipFlag::CASH_Head;
			case EquipParts::CASH_CHEST:		return EquipFlag::CASH_Chest;
			case EquipParts::CASH_LEG:			return EquipFlag::CASH_Leg;
			case EquipParts::CASH_HAND:			return EquipFlag::CASH_Hand;
			case EquipParts::CASH_FOOT:			return EquipFlag::CASH_Foot;
			case EquipParts::CASH_SUPPORT:		return EquipFlag::CASH_Support;
			case EquipParts::CASH_MASK:			return EquipFlag::CASH_Mask;
			case EquipParts::CASH_BACK:			return EquipFlag::CASH_Back;
			case EquipParts::CASH_HIP:			return EquipFlag::CASH_Hip;
			case EquipParts::CASH_ETC1:			return EquipFlag::CASH_Etc1;
			case EquipParts::CASH_ETC2:			return EquipFlag::CASH_Etc2;
			}
			Logger::GetInstance().Info("can not found Equip Flag: {0}", static_cast<int>(part));
			throw(fmt::format(L"can not found Equip Flag: {0}", static_cast<int>(part)));
		}
		Constants::EquipParts Constants::GetEquipParts(Equipment position)
		{
			switch (position)
			{
			case Equipment::Weapon:				return EquipParts::WEAPON;
			case Equipment::Head:				return EquipParts::HEAD;
			case Equipment::Body:				return EquipParts::CHEST;
			case Equipment::Legs:				return EquipParts::LEG;
			case Equipment::Gloves:				return EquipParts::HAND;
			case Equipment::Shoes:				return EquipParts::FOOT;
			case Equipment::Support:			return EquipParts::SUPPORT;
			case Equipment::Necklace:			return EquipParts::NECKLACE;
			case Equipment::Ring1:				return EquipParts::RING;
			case Equipment::Ring2:				return EquipParts::RING;
			case Equipment::Earing1:			return EquipParts::EARING;
			case Equipment::Earing2:			return EquipParts::EARING;
			case Equipment::CashWeapon:			return EquipParts::CASH_WEAPON;
			case Equipment::CashHead:			return EquipParts::CASH_HEAD;
			case Equipment::CashBody:			return EquipParts::CASH_CHEST;
			case Equipment::CashLegs:			return EquipParts::CASH_LEG;
			case Equipment::CashGloves:			return EquipParts::CASH_HAND;
			case Equipment::CashShoes:			return EquipParts::CASH_FOOT;
			case Equipment::CashSupport:		return EquipParts::CASH_SUPPORT;
			case Equipment::CashMask:			return EquipParts::CASH_MASK;
			case Equipment::CashBack:			return EquipParts::CASH_BACK;
			case Equipment::CashHip:			return EquipParts::CASH_HIP;
			case Equipment::CashEtc1:			return EquipParts::CASH_ETC1;
			case Equipment::CashEtc2:			return EquipParts::CASH_ETC2;

			case Equipment::FameAdj:			return EquipParts::FAMEADJ;
			case Equipment::FameNoun:			return EquipParts::FAMENOUN;
			case Equipment::Pet:			return EquipParts::PET;
			case Equipment::Pet_Mask:			return EquipParts::PET_MASK;
			case Equipment::Pet_Etc1:			return EquipParts::PET_ETC1;
			case Equipment::Pet_Etc2:			return EquipParts::PET_ETC2;
			}
			Logger::GetInstance().Info("can not found Equip Part: {0}", static_cast<int>(position));
			throw(fmt::format(L"can not found Equip Part: {0}", static_cast<int>(position)));
		}

		const wchar_t* Constants::GetDefaultEquipmentMeshName(Constants::ClassType type, Constants::EquipParts part)
		{
			switch (type) {
			case Constants::ClassType::Knight:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Sieg_Body_000";
				case Constants::EquipParts::WEAPON: return L"Sieg_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Sieg_Head_000";
				case Constants::EquipParts::HAND: return L"Sieg_Hand_000";
				case Constants::EquipParts::LEG: return L"Sieg_Leg_000";
				case Constants::EquipParts::FOOT: return L"Sieg_Foot_000";
				case Constants::EquipParts::CASH_CHEST: return L"Sieg_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Sieg_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Sieg_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Sieg_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Sieg_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Sieg_Foot_000";
				}
				break;
			case Constants::ClassType::Healer:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Eir_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Eir_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Eir_Head_000";
				case Constants::EquipParts::HAND: return L"Eir_Hand_000";
				case Constants::EquipParts::LEG: return L"Eir_Leg_000";
				case Constants::EquipParts::FOOT: return L"Eir_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Eir_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Eir_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Eir_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Eir_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Eir_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Eir_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Eir_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Eir_Support_000";
					//case Constants::EquipParts::CASH_MASK : return L"Eir_Empty";
					//case Constants::EquipParts::CASH_BACK : return L"Eir_Empty";
					//case Constants::EquipParts::CASH_HIP  : return L"Eir_Empty";
					//case Constants::EquipParts::CASH_ETC1 : return L"Eir_Empty";
					//case Constants::EquipParts::CASH_ETC2 : return L"Eir_Empty";
				}
				break;
			case Constants::ClassType::Wizard:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Dainn_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Dainn_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Dainn_Head_000";
				case Constants::EquipParts::HAND: return L"Dainn_Hand_000";
				case Constants::EquipParts::LEG: return L"Dainn_Leg_000";
				case Constants::EquipParts::FOOT: return L"Dainn_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Dainn_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Dainn_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Dainn_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Dainn_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Dainn_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Dainn_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Dainn_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Dainn_Support_000";
					//case Constants::EquipParts::CASH_MASK : return L"Dainn_Empty";
					//case Constants::EquipParts::CASH_BACK : return L"Dainn_Empty";
					//case Constants::EquipParts::CASH_HIP  : return L"Dainn_Empty";
					//case Constants::EquipParts::CASH_ETC1 : return L"Dainn_Empty";
					//case Constants::EquipParts::CASH_ETC2 : return L"Dainn_Empty";
				}
				break;
			case Constants::ClassType::Thief:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Tia_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Tia_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Tia_Head_000";
				case Constants::EquipParts::HAND: return L"Tia_Hand_000";
				case Constants::EquipParts::LEG: return L"Tia_Leg_000";
				case Constants::EquipParts::FOOT: return L"Tia_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Tia_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Tia_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Tia_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Tia_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Tia_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Tia_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Tia_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Tia_Support_000";
					//case Constants::EquipParts::CASH_MASK : return L"Tia_Empty";
					//case Constants::EquipParts::CASH_BACK : return L"Tia_Empty";
					//case Constants::EquipParts::CASH_HIP  : return L"Tia_Empty";
					//case Constants::EquipParts::CASH_ETC1 : return L"Tia_Empty";
					//case Constants::EquipParts::CASH_ETC2 : return L"Tia_Empty";
				}
				break;
			case Constants::ClassType::Slime:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"CSlime";
					//case Constants::EquipParts::WEAPON :  return L"CSlime_Empty";
					//case Constants::EquipParts::HEAD : return L"CSlime_Empty";
					//case Constants::EquipParts::HAND : return L"CSlime_Empty";
					//case Constants::EquipParts::LEG : return L"CSlime_Empty";
					//case Constants::EquipParts::FOOT : return L"CSlime_Empty";
					//case Constants::EquipParts::SUPPORT : return L"CSlime_Empty";
				case Constants::EquipParts::CASH_CHEST: return L"CSlime";
					//case Constants::EquipParts::CASH_WEAPON : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_HEAD : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_HAND : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_LEG : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_FOOT : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_SUPPORT : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_MASK : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_BACK : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_HIP  : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_ETC1 : return L"CSlime_Empty";
					//case Constants::EquipParts::CASH_ETC2 : return L"CSlime_Empty";
				}
				break;
			case Constants::ClassType::DollMaster:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Dacy_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Dacy_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Dacy_Head_000";
				case Constants::EquipParts::HAND: return L"Dacy_Hand_000";
				case Constants::EquipParts::LEG: return L"Dacy_Leg_000";
				case Constants::EquipParts::FOOT: return L"Dacy_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Dacy_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Dacy_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Dacy_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Dacy_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Dacy_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Dacy_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Dacy_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Dacy_Support_000";
					//case Constants::EquipParts::CASH_MASK : return L"Dacy_Empty";
					//case Constants::EquipParts::CASH_BACK : return L"Dacy_Empty";
					//case Constants::EquipParts::CASH_HIP  : return L"Dacy_Empty";
					//case Constants::EquipParts::CASH_ETC1 : return L"Dacy_Empty";
					//case Constants::EquipParts::CASH_ETC2 : return L"Dacy_Empty";
				}
				break;
			case Constants::ClassType::DarkTemplar:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Krieg_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Krieg_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Krieg_Head_000";
				case Constants::EquipParts::HAND: return L"Krieg_Hand_000";
				case Constants::EquipParts::LEG: return L"Krieg_Leg_000";
				case Constants::EquipParts::FOOT: return L"Krieg_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Krieg_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Krieg_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Krieg_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Krieg_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Krieg_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Krieg_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Krieg_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Krieg_Support_000";
					//case Constants::EquipParts::CASH_MASK : return L"Krieg_Empty";
					//case Constants::EquipParts::CASH_BACK : return L"Krieg_Empty";
					//case Constants::EquipParts::CASH_HIP  : return L"Krieg_Empty";
					//case Constants::EquipParts::CASH_ETC1 : return L"Krieg_Empty";
					//case Constants::EquipParts::CASH_ETC2 : return L"Krieg_Empty";
				}
				break;
			case Constants::ClassType::IceSorceress:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"IceGirl_Body_000";
				case Constants::EquipParts::WEAPON:  return L"IceGirl_Weapon_000";
				case Constants::EquipParts::HEAD: return L"IceGirl_Head_000";
				case Constants::EquipParts::HAND: return L"IceGirl_Hand_000";
				case Constants::EquipParts::LEG: return L"IceGirl_Leg_000";
				case Constants::EquipParts::FOOT: return L"IceGirl_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"IceGirl_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"IceGirl_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"IceGirl_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"IceGirl_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"IceGirl_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"IceGirl_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"IceGirl_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"IceGirl_Support_000";
				}
				break;
			case Constants::ClassType::Archer:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"ElfArcher_Body_000";
				case Constants::EquipParts::WEAPON:  return L"ElfArcher_Weapon_000";
				case Constants::EquipParts::HEAD: return L"ElfArcher_Head_000";
				case Constants::EquipParts::HAND: return L"ElfArcher_Hand_000";
				case Constants::EquipParts::LEG: return L"ElfArcher_Leg_000";
				case Constants::EquipParts::FOOT: return L"ElfArcher_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"ElfArcher_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"ElfArcher_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"ElfArcher_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"ElfArcher_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"ElfArcher_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"ElfArcher_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"ElfArcher_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"ElfArcher_Support_000";
				}
				break;
			case Constants::ClassType::BountyHunter:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"BountyHunter_Body_000";
				case Constants::EquipParts::WEAPON:  return L"BountyHunter_Weapon_000";
				case Constants::EquipParts::HEAD: return L"BountyHunter_Head_000";
				case Constants::EquipParts::HAND: return L"BountyHunter_Hand_000";
				case Constants::EquipParts::LEG: return L"BountyHunter_Leg_000";
				case Constants::EquipParts::FOOT: return L"BountyHunter_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"BountyHunter_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"BountyHunter_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"BountyHunter_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"BountyHunter_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"BountyHunter_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"BountyHunter_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"BountyHunter_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"BountyHunter_Support_000";
				}
				break;
			case Constants::ClassType::Bard:
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Bard_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Bard_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Bard_Head_000";
				case Constants::EquipParts::HAND: return L"Bard_Hand_000";
				case Constants::EquipParts::LEG: return L"Bard_Leg_000";
				case Constants::EquipParts::FOOT: return L"Bard_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Bard_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Bard_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Bard_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Bard_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Bard_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Bard_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Bard_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Bard_Support_000";
				}
				break;
			case Constants::ClassType::DualWield:///ͽű�ĳ�� by kpongky( 09.06.09 )
				switch (part) {
				case Constants::EquipParts::CHEST: return L"DualWield_Body_000";
				case Constants::EquipParts::WEAPON:  return L"DualWield_Weapon_000";
				case Constants::EquipParts::HEAD: return L"DualWield_Head_000";
				case Constants::EquipParts::HAND: return L"DualWield_Hand_000";
				case Constants::EquipParts::LEG: return L"DualWield_Leg_000";
				case Constants::EquipParts::FOOT: return L"DualWield_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"DualWield_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"DualWield_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"DualWield_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"DualWield_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"DualWield_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"DualWield_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"DualWield_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"DualWield_Support_000";
				}
				break;
			case Constants::ClassType::Fighter:///�ű�ĳ�� by kpongky( 09.08.10 )
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Fighter_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Fighter_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Fighter_Head_000";
				case Constants::EquipParts::HAND: return L"Fighter_Hand_000";
				case Constants::EquipParts::LEG: return L"Fighter_Leg_000";
				case Constants::EquipParts::FOOT: return L"Fighter_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Fighter_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Fighter_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Fighter_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Fighter_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Fighter_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Fighter_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Fighter_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Fighter_Support_000";
				}
				break;
			case Constants::ClassType::DarkEir:///�ű�ĳ�� by kpongky( 09.11.19 )
				switch (part) {
				case Constants::EquipParts::CHEST: return L"DarkEir_Body_000";
				case Constants::EquipParts::WEAPON:  return L"DarkEir_Weapon_000";
				case Constants::EquipParts::HEAD: return L"DarkEir_Head_000";
				case Constants::EquipParts::HAND: return L"DarkEir_Hand_000";
				case Constants::EquipParts::LEG: return L"DarkEir_Leg_000";
				case Constants::EquipParts::FOOT: return L"DarkEir_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"DarkEir_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"DarkEir_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"DarkEir_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"DarkEir_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"DarkEir_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"DarkEir_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"DarkEir_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"DarkEir_Support_000";
				}
				break;
			case Constants::ClassType::Aruta:///�ű�ĳ�� by kpongky( 09.11.19 )
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Aruta_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Aruta_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Aruta_Head_000";
				case Constants::EquipParts::HAND: return L"Aruta_Hand_000";
				case Constants::EquipParts::LEG: return L"Aruta_Leg_000";
				case Constants::EquipParts::FOOT: return L"Aruta_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Aruta_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Aruta_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Aruta_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Aruta_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Aruta_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Aruta_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Aruta_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Aruta_Support_000";
				}
				break;

			case Constants::ClassType::Gaon:///�ű�ĳ�� by kpongky( 09.11.19 )
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Gaon_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Gaon_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Gaon_Head_000";
				case Constants::EquipParts::HAND: return L"Gaon_Hand_000";
				case Constants::EquipParts::LEG: return L"Gaon_Leg_000";
				case Constants::EquipParts::FOOT: return L"Gaon_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Gaon_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Gaon_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Gaon_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Gaon_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Gaon_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Gaon_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Gaon_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Gaon_Support_000";
				}
				break;
			case Constants::ClassType::Iris:///�ű�ĳ�� by kpongky( 09.11.19 )
				switch (part) {
				case Constants::EquipParts::CHEST: return L"Iris_Body_000";
				case Constants::EquipParts::WEAPON:  return L"Iris_Weapon_000";
				case Constants::EquipParts::HEAD: return L"Iris_Head_000";
				case Constants::EquipParts::HAND: return L"Iris_Hand_000";
				case Constants::EquipParts::LEG: return L"Iris_Leg_000";
				case Constants::EquipParts::FOOT: return L"Iris_Foot_000";
				case Constants::EquipParts::SUPPORT: return L"Iris_Support_000";
				case Constants::EquipParts::CASH_CHEST: return L"Iris_Body_000";
				case Constants::EquipParts::CASH_WEAPON: return L"Iris_Weapon_000";
				case Constants::EquipParts::CASH_HEAD: return L"Iris_Head_000";
				case Constants::EquipParts::CASH_HAND: return L"Iris_Hand_000";
				case Constants::EquipParts::CASH_LEG: return L"Iris_Leg_000";
				case Constants::EquipParts::CASH_FOOT: return L"Iris_Foot_000";
				case Constants::EquipParts::CASH_SUPPORT: return L"Iris_Support_000";
				}
				break;

				//�ű�ĳ���߰� : ���⿡ �⺻ �޽� �̸�� �Է��ϴ °Ŵ. 
				//ٱ�� ĳ����� ǰ�� �����ؼ� �ٿ��ְ ���ξ �ٲ��.
				//��۾ �Ϸ �ȸ��� Ҷ� � ̳���� �����ؼ� �������Ѵ.
				//ٿ���)
				//ID : DualWieldCharacter
				//Hash : 1307419
				//Index : 11
				//DefaultMesh 
				//CHEST : 		DualWield_Body_000
				//WEAPON : 	DualWield_Weapon_000
				//HEAD : 		DualWield_Head_000
				//HAND : 		DualWield_Hand_000
				//LEG : 		DualWield_Leg_000
				//FOOT : 		DualWield_Foot_000
				//SUPPORT : 	DualWield_Support_000
				//CASH_CHEST : 	DualWield_Body_000
				//CASH_WEAPON : DualWield_Weapon_000
				//CASH_HEAD : 	DualWield_Head_000
				//CASH_HAND : 	DualWield_Hand_000
				//CASH_LEG : 	DualWield_Leg_000
				//CASH_FOOT : 	DualWield_Foot_000
				//CASH_SUPPORT : DualWield_Support_000
				//
				//�̱ۿ ��Ű: Shift + F6	<< �� Lunia2 �����Ʈ�� �߰��ϰ Lunia2/KeyInput_Lunia.xml??�� '�ű�ĳ��'�� ΰ˻��ϸ
				//��۾ �ȳ��� �ִ.

			}

			return L"";
			//return L"DefaultEquipment";
			//assert(false); // unable to find 
			//return L"";
		}


		//ChatTypes Convert ///////////////////////////////////////////////////////////////////////////
		Constants::ChatTypes Constants::ConvertStringToChatTypes(const wchar_t* string)
		{
			if (wcscmp(string, L"NormalChat") == 0)				return XRated::Constants::ChatTypes::NormalChat;
			else if (wcscmp(string, L"EmoteChat") == 0)			return XRated::Constants::ChatTypes::EmoteChat;
			else if (wcscmp(string, L"TradeChat") == 0)			return XRated::Constants::ChatTypes::TradeChat;
			else if (wcscmp(string, L"TradeSellChat") == 0)		return XRated::Constants::ChatTypes::TradeSellChat;
			else if (wcscmp(string, L"TradeBuyChat") == 0)		return XRated::Constants::ChatTypes::TradeBuyChat;
			else if (wcscmp(string, L"ShoutChat") == 0)			return XRated::Constants::ChatTypes::ShoutChat;
			else if (wcscmp(string, L"GlobalShoutChat") == 0)		return XRated::Constants::ChatTypes::GlobalShoutChat;
			else if (wcscmp(string, L"LastTextBoardMsg") == 0)	return XRated::Constants::ChatTypes::LastTextBoardMsg;
			else if (wcscmp(string, L"TextBoardChat") == 0)		return XRated::Constants::ChatTypes::TextBoardChat;
			else if (wcscmp(string, L"WhisperChat") == 0)			return XRated::Constants::ChatTypes::WhisperChat;
			else if (wcscmp(string, L"OneToOneChat") == 0)		return XRated::Constants::ChatTypes::OneToOneChat;
			else if (wcscmp(string, L"TeamChat") == 0)			return XRated::Constants::ChatTypes::TeamChat;
			else if (wcscmp(string, L"GuildChat") == 0)			return XRated::Constants::ChatTypes::GuildChat;
			else if (wcscmp(string, L"SystemChat") == 0)			return XRated::Constants::ChatTypes::SystemChat;
			else if (wcscmp(string, L"ErrorChat") == 0)			return XRated::Constants::ChatTypes::ErrorChat;
			else if (wcscmp(string, L"SystemInfoChat") == 0)		return XRated::Constants::ChatTypes::SystemInfoChat;
			else if (wcscmp(string, L"EpisodeChat") == 0)			return XRated::Constants::ChatTypes::EpisodeChat;
			else if (wcscmp(string, L"FamilyChat") == 0)			return XRated::Constants::ChatTypes::FamilyChat;
			else if (wcscmp(string, L"PartyChat") == 0)			return XRated::Constants::ChatTypes::PartyChat;
			else if (wcscmp(string, L"PartyNoticeChat") == 0)		return XRated::Constants::ChatTypes::PartyNoticeChat;

			Logger::GetInstance().Info("can't found chat types = {0}", StringUtil::ToASCII(string));
			throw(fmt::format(L"can't found chat types = {0}", string));
		}
		const wchar_t* Constants::ConvertChatTypesToString(Constants::ChatTypes type)
		{
			switch (type)
			{
			case Constants::ChatTypes::NormalChat:			return L"NormalChat";
			case Constants::ChatTypes::EmoteChat:			return L"EmoteChat";
			case Constants::ChatTypes::TradeChat:			return L"TradeChat";
			case Constants::ChatTypes::TradeSellChat:		return L"TradeSellChat";
			case Constants::ChatTypes::TradeBuyChat:		return L"TradeBuyChat";
			case Constants::ChatTypes::ShoutChat:			return L"ShoutChat";
			case Constants::ChatTypes::GlobalShoutChat:		return L"GlobalShoutChat";
			case Constants::ChatTypes::LastTextBoardMsg:	return L"LastTextBoardMsg";
			case Constants::ChatTypes::TextBoardChat:		return L"TextBoardChat";
			case Constants::ChatTypes::WhisperChat:			return L"WhisperChat";
			case Constants::ChatTypes::OneToOneChat:		return L"OneToOneChat";
			case Constants::ChatTypes::TeamChat:			return L"TeamChat";
			case Constants::ChatTypes::GuildChat:			return L"GuildChat";
			case Constants::ChatTypes::SystemChat:			return L"SystemChat";
			case Constants::ChatTypes::ErrorChat:			return L"ErrorChat";
			case Constants::ChatTypes::SystemInfoChat:		return L"SystemInfoChat";
			case Constants::ChatTypes::EpisodeChat:			return L"EpisodeChat";
			case Constants::ChatTypes::FamilyChat:			return L"Family";
			case Constants::ChatTypes::PartyChat:			return L"PartyChat";
			case Constants::ChatTypes::PartyNoticeChat:		return L"PartyNoticeChat";
			default:
				Logger::GetInstance().Info("can't found chat types = {0}", type);
				throw(fmt::format(L"can't found chat types = {0}", type));
			}
		}

		// ISerializable implementation ///////////////////////////////////////////////////////////////
		void StageEvent::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::StageEvent");
			out.Write(L"Type", static_cast<const int>(Type));
			out.Write(L"Value", static_cast<const int>(Value));
			out.Write(L"Position", Position);
			out.Write(L"Direction", static_cast<const int>(Direction));
			out.Write(L"Scale", Scale);
			out.Write(L"Effect", Effect);
			out.Write(L"Area", Area);
		}

		void StageEvent::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::StageEvent");
			in.Read(L"Type", reinterpret_cast<int&>(Type));
			in.Read(L"Value", reinterpret_cast<int&>(Value));
			in.Read(L"Position", Position);
			in.Read(L"Direction", reinterpret_cast<int&>(Direction), 2);
			in.Read(L"Scale", Scale, float3(1, 1, 1));
			in.Read(L"Effect", Effect, Locator());
			in.Read(L"Area", Area);
		}

		void ActivePoint::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::ActivePoint");
			out.Write(L"Type", static_cast<const int>(Type));
			out.Write(L"Animation", Animation);
			out.Write(L"Position", Position);
			out.Write(L"Direction", static_cast<const int>(Direction));
			out.Write(L"Scale", Scale);
			out.Write(L"Effect", Effect);
			out.Write(L"Area", Area);
		}

		void ActivePoint::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::ActivePoint");
			in.Read(L"Type", reinterpret_cast<int&>(Type));
			in.Read(L"Animation", Animation);
			in.Read(L"Position", Position);
			in.Read(L"Direction", reinterpret_cast<int&>(Direction), 2);
			in.Read(L"Scale", Scale, float3(1, 1, 1));
			in.Read(L"Effect", Effect, Locator());
			in.Read(L"Area", Area);
		}


		const ItemPosition ItemPosition::Invalid(0xff, 0xff);

		ItemPosition::ItemPosition() : Bag(0), Position(0)
		{
		}

		ItemPosition::ItemPosition(uint8 bag, uint8 position) : Bag(bag), Position(position)
		{
		}

		void ItemPosition::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"ItemPosition");
			out.Write(L"Bag", Bag);
			out.Write(L"Position", Position);
		}

		void ItemPosition::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"ItemPosition");
			in.Read(L"Bag", Bag);
			in.Read(L"Position", Position);
		}

		//bool operator<(const ItemPosition& lhs, const ItemPosition& rhs)
		//{
		//	if(lhs.Bag < rhs.Bag)
		//		return true;
		//	if(lhs.Position < rhs.Position)
		//		return true;

		//	return false;
		//}

		void ItemPack::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"ItemPack");
			out.Write(L"Position", Position);
			out.Write(L"Count", Count);
			out.Write(L"ExtendInfo", ExtendInfo);
		}

		void ItemPack::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"ItemPack");
			in.Read(L"Position", Position);
			in.Read(L"Count", Count);
			in.Read(L"ExtendInfo", ExtendInfo);
		}


		void Item::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"Item");
			out.Write(L"Id", Id);
			out.Write(L"InstanceEx", InstanceEx);
		}

		void Item::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"Item");
			in.Read(L"Id", Id);
			in.Read(L"InstanceEx", InstanceEx);
		}

		void ItemSlot::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"ItemSlot");
			out.Write(L"Id", Id);
			out.Write(L"Position", Position);
			out.Write(L"Stacked", Stacked);
			out.Write(L"instanceEx", InstanceEx);
		}

		void ItemSlot::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"ItemSlot");
			in.Read(L"Id", Id);
			in.Read(L"Position", Position);
			in.Read(L"Stacked", Stacked);
			in.Read(L"instanceEx", InstanceEx);
		}

		void InvalidEquippedItem::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"InvalidEquippedItem");
			out.Write(L"Where", static_cast<int>(where));
			out.Write(L"Enable", enable);
		}

		void InvalidEquippedItem::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"InvalidEquippedItem");
			in.Read(L"Where", reinterpret_cast<int&>(where));
			in.Read(L"Enable", enable);
		}

		void EquippedItem::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"InvalidEquippedItem");
			out.Write(L"itemHash", itemHash);
			out.Write(L"instanceEx", InstanceEx);
			out.WriteEnum(L"Where", where);
		}

		void EquippedItem::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"InvalidEquippedItem");
			in.Read(L"itemHash", itemHash);
			in.Read(L"instanceEx", InstanceEx);
			in.ReadEnum(L"Where", where);
		}

		void StoreSlot::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"StoreSlot");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"StackedCount", StackedCount);
			out.Write(L"instanceEx", InstanceEx);
			out.Write(L"SellPrice", SellPrice);
		}

		void StoreSlot::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"StoreSlot");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"StackedCount", StackedCount);
			in.Read(L"instanceEx", InstanceEx);
			in.Read(L"SellPrice", SellPrice);
		}

		bool StoreSlot::operator ==(const StoreSlot& rhs) const
		{
			return (ItemHash == rhs.ItemHash && StackedCount == rhs.StackedCount && this->InstanceEx == rhs.InstanceEx && SellPrice == rhs.SellPrice);
		}

		void ItemBasicInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"ItemInfo");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"instanceEx", this->InstanceEx);
			out.Write(L"StackCount", StackCount);
		}

		void ItemBasicInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"ItemInfo");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"instanceEx", this->InstanceEx);
			in.Read(L"StackCount", StackCount);
		}

		void RewardItem::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"RewardItem");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"instanceEx", InstanceEx);
			out.Write(L"StackCount", StackCount);
		}

		void RewardItem::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"RewardItem");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"instanceEx", InstanceEx);
			in.Read(L"StackCount", StackCount);
		}

		void PresentEventMailReward::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"PresentEventMailReward");
			out.WriteEnum(L"ExpireInfo", expireInfo);
			out.Write(L"Item", item);
		}

		void PresentEventMailReward::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"PresentEventMailReward");
			in.ReadEnum(L"ExpireInfo", expireInfo);
			in.Read(L"Item", item);
		}

		void Skill::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"Skill");
			out.Write(L"Id", Id);
			out.Write(L"Level", Level);
		}

		void Skill::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"Skill");
			in.Read(L"Id", Id);
			in.Read(L"Level", Level);
		}

		void QuickSlot::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"QuickSlot");
			out.Write(L"Id", Id);
			out.Write(L"IsSkill", IsSkill);
			out.Write(L"Pos", Pos);
			out.Write(L"instanceEx", InstanceEx);
		}

		void QuickSlot::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"QuickSlot");
			in.Read(L"Id", Id);
			in.Read(L"IsSkill", IsSkill);
			in.Read(L"Pos", Pos);
			in.Read(L"instanceEx", InstanceEx);
		}


		void Friend::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Friend");
			out.Write(L"CharacterName", CharacterName);
			out.Write(L"IsOnline", IsOnline);
			out.Write(L"ClassType", static_cast<int>(ClassType));
			out.Write(L"Level", Level);
			out.Write(L"CurrentLocation", CurrentLocation);
			out.Write(L"Memo", Memo);
		}

		void Friend::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Friend");
			in.Read(L"CharacterName", CharacterName);
			in.Read(L"IsOnline", IsOnline);
			in.Read(L"ClassType", reinterpret_cast<int&>(ClassType));
			in.Read(L"Level", Level);
			in.Read(L"CurrentLocation", CurrentLocation);
			in.Read(L"Memo", Memo);
		}

		void LobbyPlayerInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::TeamMember");
			out.Write(L"CharacterName", CharacterName);
			out.Write(L"CharacterSerial", CharacterSerial);
			out.Write(L"VirtualIdCode", VirtualIdCode);
			out.Write(L"ClassType", static_cast<int>(ClassType));
			out.Write(L"Level", Level);
			out.Write(L"Exp", Exp);
			out.Write(L"PvpLevel", PvpLevel);
			out.Write(L"PvpExp", PvpExp);
			out.Write(L"WarLevel", WarLevel);
			out.Write(L"WarExp", WarExp);
			out.Write(L"StoredLevel", StoredLevel);
			out.Write(L"RebirthCount", RebirthCount);
			out.Write(L"LastLoggedDate", LastLoggedDate);
			out.Write(L"Equipments", Equipments);
			out.Write(L"Licenses", Licenses);
			out.Write(L"LadderPoint", LadderPoint);
			out.Write(L"LadderWinCount", LadderWinCount);
			out.Write(L"LadderLoseCount", LadderLoseCount);
			out.Write(L"LadderMatchCount", LadderMatchCount);
			out.Write(L"CharacterStateFlags", static_cast<int>(StateFlags));
		}

		void LobbyPlayerInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::TeamMember");
			in.Read(L"CharacterName", CharacterName);
			in.Read(L"CharacterSerial", CharacterSerial);
			in.Read(L"VirtualIdCode", VirtualIdCode);
			in.Read(L"ClassType", reinterpret_cast<int&>(ClassType));
			in.Read(L"Level", Level);
			in.Read(L"Exp", Exp);
			in.Read(L"PvpLevel", PvpLevel);
			in.Read(L"PvpExp", PvpExp);
			in.Read(L"WarLevel", WarLevel);
			in.Read(L"WarExp", WarExp);
			in.Read(L"StoredLevel", StoredLevel);
			in.Read(L"RebirthCount", RebirthCount);
			in.Read(L"LastLoggedDate", LastLoggedDate);
			in.Read(L"Equipments", Equipments);
			in.Read(L"Licenses", Licenses);
			in.Read(L"LadderPoint", LadderPoint);
			in.Read(L"LadderWinCount", LadderWinCount);
			in.Read(L"LadderLoseCount", LadderLoseCount);
			in.Read(L"LadderMatchCount", LadderMatchCount);
			in.Read(L"CharacterStateFlags", reinterpret_cast<int&>(StateFlags));
		}

		void ItemPackage::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"ItemPackage");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"Count", Count);
			out.Write(L"ExpireDay", ExpireDay);
			out.Write(L"Description", Description);
			out.Write(L"Attr1", Attr1);
			out.Write(L"Attr2", Attr2);
			out.Write(L"Attr2", Attr3); // 3.1 by ultimate ???-> this a new scroll statuses? :D
		}

		void ItemPackage::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"ItemPackage");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"Count", Count);
			in.Read(L"ExpireDay", ExpireDay);
			in.Read(L"Description", Description);
			in.Read(L"Attr1", Attr1);
			in.Read(L"Attr2", Attr2);
			in.Read(L"Attr3", Attr3); // 3.1 by ultimate ???-> this a new scroll statuses? :D
		}

		void CashItem::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"CashItem");
			out.Write(L"OrderNumber", OrderNumber);
			out.Write(L"ProductNumber", ProductNumber);
			out.Write(L"Quantity", Quantity);
			out.Write(L"RepresentativeItemHash", RepresentativeItemHash);
			out.Write(L"Name", Name);
			out.Write(L"Packages", Packages);
		}

		void CashItem::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"CashItem");
			in.Read(L"OrderNumber", OrderNumber);
			in.Read(L"ProductNumber", ProductNumber);
			in.Read(L"Quantity", Quantity);
			in.Read(L"RepresentativeItemHash", RepresentativeItemHash);
			in.Read(L"Name", Name);
			in.Read(L"Packages", Packages);
		}

		void BagState::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"BagState");
			out.Write(L"BagNumber", BagNumber);
			out.Write(L"ExpireDate", ExpireDate);
			out.Write(L"Expired", Expired);
		}

		void BagState::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"BagState");
			in.Read(L"BagNumber", BagNumber);
			in.Read(L"ExpireDate", ExpireDate);
			in.Read(L"Expired", Expired);
		}

		void StageLocation::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::StageLocation");
			out.Write(L"StageGroupHash", StageGroupHash);
			out.Write(L"Level", Level);
			out.Write(L"Difficulty", Difficulty);
			out.Write(L"Tags", Tags); // arcrus. ��±׽ý��.
		}

		void StageLocation::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::StageLocation");
			in.Read(L"StageGroupHash", StageGroupHash);
			in.Read(L"Level", Level);
			in.Read(L"Difficulty", Difficulty, static_cast<uint8>(1));
			in.Read(L"Tags", Tags, std::vector< std::wstring >()); // arcrus. ��±׽ý��.
		}

		void SquareInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"SquareInfo");
			out.Write(L"Name", Name);
			out.Write(L"Status", static_cast<const int>(Status));
			out.Write(L"StageGroupHash", StageGroupHash);
			out.Write(L"AccessLevel", AccessLevel);
			out.Write(L"Capacity", Capacity);
			out.Write(L"OrderNumber", OrderNumber);
		}

		void SquareInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"SquareInfo");
			in.Read(L"Name", Name);
			in.Read(L"Status", reinterpret_cast<int&>(Status));
			in.Read(L"StageGroupHash", StageGroupHash);
			in.Read(L"AccessLevel", AccessLevel);
			in.Read(L"Capacity", Capacity);
			in.Read(L"OrderNumber", OrderNumber);
		}

		void GameMode::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PvpGameType");
			out.Write(L"GameType", static_cast<int>(GameType));
			out.Write(L"ItemPopup", ItemPopup);
		}

		void GameMode::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PvpGameType");
			in.Read(L"GameType", reinterpret_cast<int&>(GameType));
			in.Read(L"ItemPopup", ItemPopup);
		}

		void StateFlag::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"StateFlag");
			out.Write(L"Id", Id);
			out.Write(L"Level", (short)Level);
		}

		void StateFlag::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"StateFlag");
			in.Read(L"Id", Id);
			in.Read(L"Level", (short&)Level);
		}

		void Quest::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"Quest");
			out.Write(L"Id", Id);
			out.Write(L"CurrentState", CurrentState);
			out.Write(L"ExpiredDate", ExpiredDate);
			out.Write(L"Params", Params);
		}

		void Quest::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"Quest");
			in.Read(L"Id", Id);
			in.Read(L"CurrentState", CurrentState);
			in.Read(L"ExpiredDate", ExpiredDate);
			in.Read(L"Params", Params);
		}
		/////////////////////////////////////////////////////////////// ISerializable implementation //


		ObjectData::ObjectData()
			: GameObjectSerial(0), Type(Constants::ObjectType::Player), Radius(0)
		{
		}

		void ObjectData::operator =(const ObjectData& obj)
		{
			GameObjectSerial = obj.GameObjectSerial;
			Name = obj.Name;
			Type = obj.Type;
			Position = obj.Position;
			Direction = obj.Direction;
			Radius = obj.Radius;
		}

		ItemData::ItemData(ObjectData& obj)
			: BaseObject(obj)
		{
		}

		CharacterData::CharacterData(ObjectData& obj)
			: BaseObject(obj), Xp(0), Level(0), Money(0), BankMoney(0), Team(0), Alliance(0)
			, Hp(0), MaxHp(0), Mp(0), MaxMp(0), Speed(0), SpeedVector(1)
		{
		}

		void ItemData::operator= (const ItemData& item)
		{
			BaseObject = item.BaseObject;
			OwnerId = item.OwnerId;
			StackCount = item.StackCount;
			PrivateItem = item.PrivateItem;
			this->InstanceEx = item.InstanceEx;
		}

		void CharacterData::operator =(const CharacterData& ch)
		{
			BaseObject = ch.BaseObject;
			Xp = ch.Xp;
			Level = ch.Level;
			Money = ch.Money;
			BankMoney = ch.BankMoney;
			Team = ch.Team;
			Alliance = ch.Alliance;

			Hp = ch.Hp;
			MaxHp = ch.MaxHp;
			Mp = ch.Mp;
			MaxMp = ch.MaxMp;

			Speed = ch.Speed;
			SpeedVector = ch.SpeedVector;
		}


		NonPlayerData::NonPlayerData(CharacterData& character)
			: BaseCharacter(character), Npc(NpcType::Normal)
		{
		}

		void NonPlayerData::operator =(const NonPlayerData& npc)
		{
			BaseCharacter = npc.BaseCharacter;
			Npc = npc.Npc;
			//Scale=npc.Scale;
		}


		PlayerData::PlayerData(CharacterData& character)
			: BaseCharacter(character), Job(Constants::ClassType::Healer), Life(0), MaxLife(0), SkillPoint(0)
			, BonusLife(0), UsableBonusLifeInStage(0)
			, Sp(0), MaxSp(0), AddedSkillPointPlus(0), StoredLevel(0), RebirthCount(0), StoredSkillPoint(0), LastRebirthDateTime(DateTime::Infinite)
			, achievementScore(0)
			//, fullFactor(1.0f), goodFactor(1.0f), sosoFactor(1.0f), hungryFactor(1.0f)
		{
			//PS.Xp = 0;
		}

		void PlayerData::operator =(const PlayerData& pc)
		{
			BaseCharacter = pc.BaseCharacter;
			Job = pc.Job;
			Life = pc.Life;
			MaxLife = pc.MaxLife;
			BonusLife = pc.BonusLife;
			UsableBonusLifeInStage = std::min<unsigned short>(pc.UsableBonusLifeInStage, pc.BonusLife);
			SkillPoint = pc.SkillPoint;
			AddedSkillPointPlus = pc.AddedSkillPointPlus;
			Sp = pc.Sp;
			MaxSp = pc.MaxSp;
			Skills = pc.Skills;
			Equipments = pc.Equipments;
			//PS = pc.PS;
			StoredLevel = pc.StoredLevel;
			RebirthCount = pc.RebirthCount;
			StoredSkillPoint = pc.StoredSkillPoint;
			LastRebirthDateTime = pc.LastRebirthDateTime;
			achievementScore = pc.achievementScore;
		}


		void NexonGuildInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"NexonGuildInfo");
			out.Write(L"GuildOid", GuildOid);
			out.Write(L"GuildId", GuildId);
			out.Write(L"GuildName", GuildName);
			out.Write(L"CreatedDate", CreatedDate);
			out.Write(L"MasterCharacterName", MasterCharacterName);
			out.Write(L"NumberOfMembers", NumberOfMembers);
			out.Write(L"Intro", Intro);
		}

		void NexonGuildInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"NexonGuildInfo");
			in.Read(L"GuildOid", GuildOid);
			in.Read(L"GuildId", GuildId);
			in.Read(L"GuildName", GuildName);
			in.Read(L"CreatedDate", CreatedDate);
			in.Read(L"MasterCharacterName", MasterCharacterName);
			in.Read(L"NumberOfMembers", NumberOfMembers);
			in.Read(L"Intro", Intro);
		}

		void AllMGuildInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"AllMGuildInfo");
			out.Write(L"GuildId", GuildId);
			out.Write(L"GuildExp", GuildExp);
			out.Write(L"GuildAlias", GuildAlias);
			out.Write(L"MyPlayTime", MyPlayTime);
			out.Write(L"TotalPlayTime", TotalPlayTime);
			out.Write(L"StartedTimeToPlay", StartedTimeToPlay);
			out.Write(L"MyContributed", MyContributed);
			out.Write(L"Point", Point);
			out.Write(L"ShopOpenDate", ShopOpenDate);
			out.Write(L"ShopCloseDate", ShopCloseDate);
			out.Write(L"Rank", Rank);
			out.Write(L"RankExpiredDate", RankExpiredDate);
			out.Write(L"Tax", Tax);
			out.Write(L"TaxPayDay", TaxPayDay);
			out.Write(L"GuildName", GuildName);
			out.Write(L"Message", Message);
			out.Write(L"MasterName", MasterName);
			out.Write(L"GuildLevel", static_cast<const int>(GuildLevel));
			out.Write(L"MemberCount", MemberCount);
			out.Write(L"CreatedDate", CreatedDate);
			out.Write(L"Grade", Grade);
			out.Write(L"MyGrade", MyGrade);
			out.Write(L"GuildMemberId", GuildMemberId);
		}

		void AllMGuildInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"AllMGuildInfo");
			in.Read(L"GuildId", GuildId);
			in.Read(L"GuildExp", GuildExp);
			in.Read(L"GuildAlias", GuildAlias);
			in.Read(L"MyPlayTime", MyPlayTime);
			in.Read(L"TotalPlayTime", TotalPlayTime);
			in.Read(L"StartedTimeToPlay", StartedTimeToPlay);
			in.Read(L"MyContributed", MyContributed);
			in.Read(L"Point", Point);
			in.Read(L"ShopOpenDate", ShopOpenDate);
			in.Read(L"ShopCloseDate", ShopCloseDate);
			in.Read(L"Rank", Rank);
			in.Read(L"RankExpiredDate", RankExpiredDate);
			in.Read(L"Tax", Tax);
			in.Read(L"TaxPayDay", TaxPayDay);
			in.Read(L"GuildName", GuildName);
			in.Read(L"Message", Message);
			in.Read(L"MasterName", MasterName);
			in.Read(L"GuildLevel", reinterpret_cast<int&>(GuildLevel), 3);
			in.Read(L"MemberCount", MemberCount);
			in.Read(L"CreatedDate", CreatedDate);
			in.Read(L"Grade", Grade);
			in.Read(L"MyGrade", MyGrade);
			in.Read(L"GuildMemberId", GuildMemberId);
		}

		void AllMGuildInfo::GradeInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"AllMGuildInfo::GradeInfo");
			out.Write(L"GradeName", GradeName);
			out.Write(L"Authority", Authority);
		}

		void AllMGuildInfo::GradeInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"AllMGuildInfo::GradeInfo");
			in.Read(L"GradeName", GradeName);
			in.Read(L"Authority", Authority);
		}

		uint16 AllMGuildInfo::GetLimitMemberCount() const
		{
#if !defined(_JAPAN) && !defined(_KOREA)
			switch (GuildLevel)
			{
			case 0: return 15;
			case 1: return 30;
			case 2: return 60;
			case 3: return 90;
			case 4: return 130;
			case 5: return 200;
			default:
				return 15;
			}
#else
			return 200;
#endif
		}

		void GuildRankInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::GuildRankInfo");
			out.Write(L"GuildID", GuildID);
			out.Write(L"Rank", Rank);
			out.Write(L"GuildName", GuildName);
			out.Write(L"GuildMemberCount", GuildMemberCount);
			out.Write(L"TotalContribution", TotalContribution);
			out.Write(L"TotalPlayTime", TotalPlayTime);
			out.Write(L"CurrentTotalPlayTime", CurrentTotalPlayTime);
			out.Write(L"GuildLevel", GuildLevel);
		}


		void GuildRankInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::GuildRankInfo");
			in.Read(L"GuildID", GuildID);
			in.Read(L"Rank", Rank);
			in.Read(L"GuildName", GuildName);
			in.Read(L"GuildMemberCount", GuildMemberCount);
			in.Read(L"TotalContribution", TotalContribution);
			in.Read(L"TotalPlayTime", TotalPlayTime);
			in.Read(L"CurrentTotalPlayTime", CurrentTotalPlayTime);
			in.Read(L"GuildLevel", GuildLevel);
		}
		bool GuildRankInfo::CompareByRank::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return guild1.Rank > guild2.Rank;
		}
		bool GuildRankInfo::CompareByName::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return (std::wcscmp(guild1.GuildName.c_str(), guild2.GuildName.c_str()) > 0);
		}
		bool GuildRankInfo::CompareByMemberCount::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return guild1.GuildMemberCount > guild2.GuildMemberCount;
		}
		bool GuildRankInfo::CompareByContribution::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return guild1.TotalContribution > guild2.TotalContribution;
		}
		bool GuildRankInfo::CompareByLevel::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return guild1.GuildLevel > guild2.GuildLevel;
		}
		bool GuildRankInfo::CompareByPlayTime::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return guild1.TotalPlayTime > guild2.TotalPlayTime;
		}
		bool GuildRankInfo::CompareByCurrentPlayTime::operator()(const GuildRankInfo& guild1, const GuildRankInfo& guild2) const
		{
			return guild1.CurrentTotalPlayTime > guild2.CurrentTotalPlayTime;
		}

		void GuildShopItem::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::GuildShopItem");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"ExpiredDate", ExpiredDate);
		}

		void GuildShopItem::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::GuildShopItem");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"ExpiredDate", ExpiredDate);
		}
		void AllMBasicGuildInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"AllMBasicGuildInfo");
			out.Write(L"GuildName", GuildName);
			out.Write(L"GuildId", GuildId);
			out.Write(L"MemberCount", MemberCount);
		}

		void AllMBasicGuildInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"AllMBasicGuildInfo");
			in.Read(L"GuildName", GuildName);
			in.Read(L"GuildId", GuildId);
			in.Read(L"MemberCount", MemberCount);
		}


		void AllMGuildUserInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"AllMGuildUserInfo");
			out.Write(L"GuildMemberId", GuildMemberId);
			out.Write(L"CharacterName", CharacterName);
			uint64 Instance;
			Instance = CharacterInfo;
			out.Write(L"CharacterInfo", Instance);
			out.Write(L"LastLogin", LastLogin);
			out.Write(L"Contributed", Contributed);
			out.Write(L"PrivateMessage", PrivateMessage);
		}

		void AllMGuildUserInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"AllMGuildUserInfo");
			in.Read(L"GuildMemberId", GuildMemberId);
			in.Read(L"CharacterName", CharacterName);
			uint64 Instance;
			in.Read(L"CharacterInfo", Instance);
			CharacterInfo = Instance;
			in.Read(L"LastLogin", LastLogin);
			in.Read(L"Contributed", Contributed);
			in.Read(L"PrivateMessage", PrivateMessage);
		}

		void Mail::HeaderInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"Mail::HeaderInfo");
			out.Write(L"Index", Index);
			out.Write(L"Title", Title);
			out.Write(L"Flag", Flag);
			out.Write(L"IsRead", IsRead);
			out.Write(L"Sender", Sender);
			out.Write(L"SentDate", SentDate);
		}

		void Mail::HeaderInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"Mail::HeaderInfo");
			in.Read(L"Index", Index);
			in.Read(L"Title", Title);
			in.Read(L"Flag", Flag);
			in.Read(L"IsRead", IsRead);
			in.Read(L"Sender", Sender);
			in.Read(L"SentDate", SentDate);
		}

		void Mail::ContentsInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"Mail::ContentsInfo");
			out.Write(L"Index", Index);
			out.Write(L"Message", Message);
			out.Write(L"AttachedMoney", AttachedMoney);
			out.Write(L"AttachedItems", AttachedItems);
			out.Write(L"StampItemHash", StampItemHash);
			out.Write(L"IsSystemMail", IsSystemMail);
		}

		void Mail::ContentsInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"Mail::ContentsInfo");
			in.Read(L"Index", Index);
			in.Read(L"Message", Message);
			in.Read(L"AttachedMoney", AttachedMoney);
			in.Read(L"AttachedItems", AttachedItems);
			in.Read(L"StampItemHash", StampItemHash);
			in.Read(L"IsSystemMail", IsSystemMail);
		}

		void Fishing::FishingInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"Fishing::FishingInfo");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"Count", Count);
			out.Write(L"CharacterName", CharacterName);
			out.Write(L"FishingTime", FishingTime);
		}

		void Fishing::FishingInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"Fishing::FishingInfo");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"Count", Count);
			in.Read(L"CharacterName", CharacterName);
			in.Read(L"FishingTime", FishingTime);
		}

		std::wstring GetRank(uint8 rank)
		{
			std::wstring missionRank;
			switch (rank)
			{
			case XRated::RankS:
				missionRank = L"S";
				break;
			case XRated::RankA:
				missionRank = L"A";
				break;
			case XRated::RankB:
				missionRank = L"B";
				break;
			case XRated::RankC:
				missionRank = L"C";
				break;
			case XRated::RankF:
				missionRank = L"F";
				break;
			}

			return missionRank;
		}

		int32 GetGainXPByStatisticsRank(uint8 rank, float gainXp)
		{
			float xp = 0;

			switch (rank)
			{
			case XRated::RankS:
				xp = gainXp * 0.1f;
				break;
			case XRated::RankA:
				xp = gainXp * 0.06f;
				break;
			case XRated::RankB:
				xp = gainXp * 0.04f;
				break;
			case XRated::RankC:
				xp = gainXp * 0.02f;
				break;
			case XRated::RankF:
				xp = gainXp * 0.0f;
				break;
			}

			return static_cast<int>(xp + 0.5f);
		}

		float GetGainXPRateByReviveCount(uint16 totalReviveCount)
		{
			if (totalReviveCount < 5) return 0.0f;
			else if (totalReviveCount >= 5 && totalReviveCount < 10) return 0.1f;
			else if (totalReviveCount >= 10 && totalReviveCount < 15) return 0.2f;
			else if (totalReviveCount >= 15) return 0.3f;

			return 0.0f;
		}

		int GetNextRankTime(uint8 rank, int standatdTime)
		{
			//if (rank == XRated::RankS) return 0;

			float TargetVal = 0.0f;
			switch (rank)
			{
			case XRated::RankS:
				break;
			case XRated::RankA:
				TargetVal = 0.75f;
				break;
			case XRated::RankB:
				TargetVal = 1.0f;
				break;
			case XRated::RankC:
				TargetVal = 1.5f;
				break;
				///	RankF۴ ¹��Ѵ??.........
			case XRated::RankF:
				TargetVal = 2.0f;
				break;
			}

			return static_cast<int>(TargetVal * standatdTime);
		}

		float GetGradeRank(uint8 rank)
		{
			float grade = 0.f;

			switch (rank)
			{
			case XRated::RankS:
				grade = 7;
				break;
			case XRated::RankA:
				grade = 5;
				break;
			case XRated::RankB:
				grade = 4;
				break;
			case XRated::RankC:
				grade = 3;
				break;
			case XRated::RankF:
				grade = 0;
				break;
			}

			return grade;
		}

		uint8 GetPlayerRankWithLevelGap(uint16 playerLevel, uint8 playerRank, int proprietyLevel) //�??����ġ�̴..
		{
			if (playerRank == XRated::RankF) return playerRank;

			if ((playerLevel > proprietyLevel) && (playerLevel - proprietyLevel >= 10))
				++playerRank;

			return playerRank;
		}

		void MissionResultInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::MissionResultInfo");
			out.Write(L"MaxAirCombo", MaxAirCombo);
			out.Write(L"Rank", Rank);
			out.Write(L"ClearXp", ClearXp);
			out.Write(L"StageXp", StageXp);
		}
		void MissionResultInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::MissionResultInfo");
			in.Read(L"MaxAirCombo", MaxAirCombo);
			in.Read(L"Rank", Rank);
			in.Read(L"ClearXp", ClearXp);
			in.Read(L"StageXp", StageXp);
		}

		uint8 MissionResultFunctions::GetPlayTimeRank(int clearTime, int standardTime)
		{
			if (standardTime == 0) return XRated::RankF; //ٸ�� ���ؽð�� ̹��� Fǳ��� -0-;;

			float val = static_cast<float>(clearTime) / static_cast<float>(standardTime);

			if (val <= 0.75f)
				return XRated::RankS;
			else if (val <= 1.0f)
				return XRated::RankA;
			else if (val <= 1.5f)
				return XRated::RankB;
			else if (val <= 2.0f)
				return XRated::RankC;

			return XRated::RankF;
		}

		uint8 MissionResultFunctions::GetLifeRank(bool death, uint16 curLife)
		{

			if (curLife >= 2)
				return XRated::RankS;
			else if (curLife == 1)
				return XRated::RankA;

			//curLife԰� 0�ΰ�
			if (death) return XRated::RankF;
			return XRated::RankB;
		}

		uint8 MissionResultFunctions::GetSecretRank(int foundSecret, int totalSecret)
		{
			float point = (static_cast<float>(foundSecret) + 1.0f) / (static_cast<float>(totalSecret) + 1.0f);

			//�Ҽ�� 2��° �ڸ����� �ݿø�
			point *= 10.0f;
			point += 0.5f;
			int p = static_cast<int>(point);
			point = (static_cast<float>(p) / 10.f) * 6.f;

			if (point >= 6.0f)
				return XRated::RankS;
			else if (point >= 4.0f && point < 6.0f)
				return XRated::RankA;
			else if (point >= 2.0f && point < 4.0f)
				return XRated::RankB;
			else if (point >= 1.3f && point < 2.0f)
				return XRated::RankC;

			return XRated::RankF;
		}

		uint8 MissionResultFunctions::GetAirComboRank(uint32 maxAirComboCount)
		{
			if (maxAirComboCount > 15)
				return XRated::RankS;
			else if (maxAirComboCount >= 8 && maxAirComboCount <= 15)
				return XRated::RankA;
			else if (maxAirComboCount >= 4 && maxAirComboCount <= 7)
				return XRated::RankB;
			else if (maxAirComboCount >= 1 && maxAirComboCount <= 3)
				return XRated::RankC;

			return XRated::RankF;
		}

		uint8 MissionResultFunctions::GetStylePointRank(float stylePointPercentage)
		{
			if (stylePointPercentage >= 0.8)
				return	XRated::RankS;
			else if (stylePointPercentage >= 0.6)
				return	XRated::RankA;
			else if (stylePointPercentage >= 0.4)
				return	XRated::RankB;
			else if (stylePointPercentage >= 0.2)
				return	XRated::RankC;

			return	XRated::RankF;
		}

		uint8 MissionResultFunctions::GetStageClearRank(bool clear, int playTime, int standardTime, bool death, uint16 curLife, int foundSecret, int totalSecret, uint32 maxAirComboCount, float stylePointPercentage)
		{
			if (!clear) return XRated::RankF;

			uint8 playTimeRank = GetPlayTimeRank(playTime, standardTime);
			uint8 lifeRank = GetLifeRank(death, curLife);
			uint8 secretRank = GetSecretRank(foundSecret, totalSecret);
			uint8 airComboRank = GetAirComboRank(maxAirComboCount);
			uint8 stylePointRank = GetStylePointRank(stylePointPercentage);

			float totalGrade = static_cast<float>((GetGradeRank(playTimeRank) * 0.25) + (GetGradeRank(lifeRank) * 0.35) + (GetGradeRank(secretRank) * 0.05) + (GetGradeRank(airComboRank) * 0.15) + (GetStylePointRank(stylePointRank) * 0.2));
			if (totalGrade >= 6.0f)
				return XRated::RankS;
			else if (totalGrade >= 4.5f && totalGrade < 6.0f)
				return XRated::RankA;
			else if (totalGrade >= 3.0f && totalGrade < 4.5f)
				return XRated::RankB;
			else if (totalGrade >= 1.5f && totalGrade < 3.0f)
				return XRated::RankC;

			return XRated::RankF;
		}

		/*
		int Rank(int kill,int life,int combo)
		{
			//TODO: calculate rank by statisics values
			int value;
			value = kill * 100;
			value += (life * 10000);
			value += (combo * 100);
			if( value >= 35000 )
				return XRated::RankAPlusPlus;
			else if( value > 33000 )
				return XRated::RankAPlus;
			else if( value > 31000 )
				return XRated::RankA;
			if( value >= 25000 )
				return XRated::RankBPlusPlus;
			else if( value > 23000 )
				return XRated::RankBPlus;
			else if( value > 21000 )
				return XRated::RankB;
			if( value >= 15000 )
				return XRated::RankCPlusPlus;
			else if( value > 13000 )
				return XRated::RankCPlus;
			else
				return XRated::RankC;

			//return XRated::RankC;
		}
		*/

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Exp Functions
		float ExpFunctions::Pvp::CalculateSurvivalXpWithLevelDifference(uint16 opLv, uint16 myLv)
		{
			float xp = (float)opLv * 5.0f + 10.0f;

			int32 levelGap = (int32)opLv - (int32)myLv;
			if (levelGap >= 0 && levelGap <= 4)
				return xp * 7.0f;
			else if (levelGap > 4 && levelGap < 14)
				return (float)xp * (float)(14 - levelGap) * 0.7f; //	/10.0f * 7.0f;
			else if (levelGap < 0 && levelGap > -10)
				return (float)xp * (float)(10 + levelGap) * 0.7f; //	/10.0f * 7.0f;
			else
				return 0;
		}

		int ExpFunctions::Pvp::CalculateXpForWinner(uint16 myLv, uint16 myPvpLv, float avgLv, float deter, int memCnt)
		{
			if (avgLv == 0)
				avgLv = 0.1f;

			float xp = (deter * (10.0f + 4.0f * (float)(memCnt - 1)) + 100.0f);

			xp = xp * ((float)myLv > avgLv ? 1.0f : (float)myLv / avgLv);

			int lvGap = (int)myPvpLv - (int)myLv;
			if (lvGap > 0)
				xp = xp - xp * (lvGap > 4 ? 1.0f : (float)lvGap * 0.2f);


			if (xp < 80)
				xp = 80;

			return (int)xp;
		}

		int ExpFunctions::Pvp::CalculateXpForLoser(uint16 myLv, uint16 myPvpLv)
		{
			int xp = 0;
			if (myPvpLv < 30) {
				xp = 20;
				int lvGap = (int)myPvpLv - (int)myLv;
				if (lvGap > 0)
					xp = (int)((float)xp * (lvGap > 4 ? 1.0f : lvGap * 0.2f));
			}
			else
				//return 0;
				xp = ((int)myPvpLv - 30) * -5;

			//return xp < 0 ? 0 : xp;
			return xp;
		}

		int ExpFunctions::Pvp::CalculateMinimumPlayMinute(int memCnt)
		{
			return (60 * memCnt + 120) / 60;
		}

		float ExpFunctions::Pvp::CalculateLevelGapBonusRate(float deltaAvgLv)
		{
			int gap = static_cast<int>(deltaAvgLv);
			if (gap < 0 || gap > 7) /* �츮��� Ƿ�� ̳����쳪 ��� �ʹ� ��� ̳�� °�(���¡�� κ�)*/
			{
				return 1.0f;
			}
			else
			{
				float rate = 1.0f + 0.02f * gap;
				if (rate > 1.1f) rate = 1.1f; // (�ִ 1.1���� ��� � ����)
				return rate;
			}
		}

		float ExpFunctions::Pvp::CalculateMemCntBonusRate(int memCnt)
		{
			return (static_cast<float>(memCnt) * 0.2f) + 0.6f;
		}

		int ExpFunctions::Pvp::CalculateStageXpForWinner(uint16 myLv, float deltaAvgLv /* �������lv - տ츮����lv */, float /*playTime*/ /*մ��:�*/, int memCnt)
		{
			//if ( playTime/60 >= CalculateMinimumPlayMinute( memCnt ) )
			//{
			float bonus = CalculateLevelGapBonusRate(deltaAvgLv);
			/*ALLM_IMPORTANT((L"CalculateStageXpForWinner: %d ( Lv: %d, dALv: %f, playtime: %f sec, members: %d )",
				static_cast<int>( myLv * 82 * CalculateMinimumPlayMinute( memCnt )  * bonus )
				, myLv
				, deltaAvgLv
				, playTime
				, memCnt ));
			*/
			return static_cast<int>(myLv * 82 * CalculateMinimumPlayMinute(memCnt) * CalculateMemCntBonusRate(memCnt) * bonus);
			//}
			//else
			//{
			//	//ALLM_IMPORTANT((L"CalculateStageXpForWinner: PlayTime is defficient!( %f sec / Members : %d )", playTime, memCnt ));
			//	return 0;
			//}
		}

		int ExpFunctions::Pvp::CalculateStageXpForLoser(uint16 myLv, float deltaAvgLv /* ʻ������lv - տ츮����lv */, float playTime /*մ��:�*/, int memCnt)
		{
			int r = CalculateStageXpForWinner(myLv, deltaAvgLv, playTime, memCnt);
			//ALLM_IMPORTANT((L"CalculateStageXpForLoser : %d ( 75% of winner )" , static_cast<int>( r * 0.75f ) ));
			return static_cast<int>(r * 0.1f);

		}

		int ExpFunctions::Pvp::GetXp(int memberCnt)
		{
			return (10 * memberCnt + 90) * memberCnt;
		}

		float ExpFunctions::Stage::CalculateXpWithLevelGap(float xp, int levelGap)
		{
			if (levelGap > 0) { //ʽ������ � �??�̴
				switch (levelGap) {
				case 1:
				case 2:
				case 3:
					break;
				case 4:
				case 5:
					xp *= 1.05f;
					break;
				case 6:
				case 7:
					xp *= 1.1f;
					break;
				case 8:
				case 9:
					xp *= 1.15f;
					break;
				case 10:
				case 11:
					xp *= 1.2f;
					break;
				default:
					xp *= 1.25f;
					break;
				}
			}
			else { //��÷��̾ � �??�̴
				switch (levelGap) {
				case 0:
				case -1:
				case -2:
				case -3:
					break;
				case -4:
					xp *= 0.75f;
					break;
				case -5:
					xp *= 0.5f;
					break;
				case -6:
					xp *= 0.25f;
					break;
				case -7:
					xp *= 0.12f;
					break;
				case -8:
					xp *= 0.1f;
					break;
				case -9:
					xp *= 0.08f;
					break;
				case -10:
					xp *= 0.06f;
					break;
				case -11:
					xp *= 0.04f;
					break;
				case -12:
					xp *= 0.02f;
					break;
				default:
					xp *= 0.01f;
					break;
				}
			}
			return xp;
		}

		float ExpFunctions::Stage::GetAddExpFactorByRebirthCount(const uint16 rebirthCount)
		{
			if (rebirthCount <= 0) return 1.0f;
			uint16 count = rebirthCount;
			if (count > 15) count = 15;
			return pow(1.2f, static_cast<float>(count));
		}


		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Gold Functions
		float GoldFunctions::Pvp::GetPlayTimeRate(int memCnt)
		{
			return (30.f * static_cast<float>(memCnt) + 60.f) / 60.f;
		}

		float GoldFunctions::Pvp::GetBasicMoneyPerMin(uint16 level)
		{
			return 10.625f * static_cast<float>(level) + 59.375f;
		}

		float GoldFunctions::Pvp::GetLifeBonusRate(int deathCount)
		{
			return 0.24f - (0.08f * static_cast<float>(deathCount));
		}

		float GoldFunctions::Pvp::GetKillBonusRate(int killCount)
		{
			if (killCount > 12) killCount = 12;
			return 0.5f * static_cast<float>(killCount) / 6.f;
		}

		uint32 GoldFunctions::Pvp::CacluateVictoryMoneyReward(uint16 level, int memCnt, int killCount, int deathCount)
		{
			return static_cast<uint32>((GetBasicMoneyPerMin(level) * GetPlayTimeRate(memCnt) * (1.f + GetLifeBonusRate(deathCount) + GetKillBonusRate(killCount))) + 0.5f);
		}

		uint32 GoldFunctions::Pvp::CacluateDefeatedMoneyReward(uint16 level, int memCnt, int killCount, int deathCount)
		{
#if defined(_TAIWAN) || defined(_CHINA)
			return static_cast<uint32>(CacluateVictoryMoneyReward(level, memCnt, killCount, deathCount) * 0.1f);
#else
			return static_cast<uint32>(CacluateVictoryMoneyReward(level, memCnt, killCount, deathCount) * 0.9f);
#endif
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Status Functions
	//	float StatusFunctions::GetHpRatePerUserCnt(NonPlayerData::NpcType type, int userCnt)
	//	{
	//		float hpRateTable[NonPlayerData::NpcType::Cnt][8] = {
	//#if defined (_TAIWAN)
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.32f, 1.44f, 1.56f, 1.68f, 1.8f, 1.92f, 2.04f},
	//			{1.0f, 1.32f, 1.44f, 1.56f, 1.68f, 1.8f, 1.92f, 2.04f}
	//#else
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{1.0f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{0.5f, 1.03f, 1.06f, 1.09f, 1.12f, 1.15f, 1.18f, 1.21f},
	//			{0.5f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f},
	//			{0.5f, 1.1f, 1.2f, 1.3f, 1.4f, 1.5f, 1.6f, 1.7f}
	//#endif
	//		};
	//		if ( userCnt>0 && userCnt<=8 )
	//			return hpRateTable[type][userCnt-1];
	//
	//		return hpRateTable[type][7];
	//	}

		float StatusFunctions::GetHPperVit(uint16 lv)
		{
			if (lv < 20)
				return 5;
			else if (lv < 60)
				return 6;

			return 7;
		}

		float StatusFunctions::GetMPperInt(uint16 lv)
		{
			if (lv < 20)
				return 5;
			else if (lv < 60)
				return 6;

			return 7;
		}

		float StatusFunctions::GetReducedCooldownRate(int dex)
		{
			//#ifdef _TAIWAN
			//		return 1.0f - ( dex > Constants::MaxStat ? (float)Constants::MaxStat / (float)220 * 0.1f : (float)dex / (float)220 * 0.1f );
			//#else
			return 1.0f - (float)dex / (float)330 * 0.1f;
			//#endif
		}

		float GetIncreasedRateFactor(int value)
		{
			if (value > 2000)
				return 0.5f;
			else if (value > 1500)
				return 0.4f;
			else if (value > 1000)
				return 0.3f;
			else if (value > 500)
				return 0.2f;

			return 0.1f;
		}


		float StatusFunctions::GetIncreasedDamageRate(int str)//ٿ�� ����ϸ StatCalculator.cpp 鵵 ����ؾ��. �ã�ƺ�� ����.
		{
			//#ifdef _TAIWAN
			//		return 1.0f + ( str > Constants::MaStat ? (float)Constants::MaxStat / (float)200 * 0.1f : (float)str / (float)200 * 0.1f );
			//#else
			if (str > 10000)
			{
				double value = static_cast<double>((str - 1000) / 1000 * 1.35);
				return static_cast<float>(((value * value) * 0.03) + ((0.0008 * static_cast<double>(str)) + 8.28));
			}
			else if (str > 3000)
			{
				////��� 10000�̻�� �����ϰ �ó���ϵ�� ��Ѵ.
				//if( str > 10000 ) {
				//	str = 10000;
				//}
				double value = static_cast<double>(static_cast<int>((str - 1000) / 1000 * 2));
				return static_cast<float>(((value * value) * 0.03) + ((0.0008 * static_cast<double>(str)) + 2.99));
			}
			else {
				return 1.0f + (float)str / 330.0f * GetIncreasedRateFactor(str);
			}
			//#endif
		}
		float StatusFunctions::GetIncreasedDamageRateInPvp(int str)//ٿ�� ����ϸ StatCalculator.cpp 鵵 ����ؾ��. �ã�ƺ�� ����.
		{
			if (str > 500)
			{
				//((((0.6 * Str - 300) ^ 0.17) * 10) -1.9) * 0.01 = Dmg%		

				return 1.0f + (pow((0.6f * (float)str - 300.f), 0.17f) * 10.f - 1.9f) * 0.01f;

				//hitIgnoreMultiplyValue = (1.f * pow(abs(hitIgnoreMultiplyValue0), 0.7f) * 2.f + 80.f) / 100.f * hitIgnoreMultiply;
			}
			else
			{
				return 1.0f + (float)str / 330.0f * 0.1f;
			}
		}
		float StatusFunctions::GetIncreasedDamageRateByAtk(float atkVal)
		{
			if (atkVal <= 0) return 0.0f;

			if (atkVal / 2 > 10000)
			{
				double value = static_cast<double>((atkVal / 2 - 1000) / 1000 * 1.35);
				return static_cast<float>(((value * value) * 0.03 - 1) + (0.0004 * atkVal + 8.28)) * 0.2;
			}
			else if (atkVal / 2 > 3000)
			{
				double value = static_cast<double>((atkVal / 2 - 1000) / 1000 * 2);
				return static_cast<float>(((value * value) * 0.03 - 1) + (0.0004 * atkVal + 2.99)) * 0.2;
			}
			else if (atkVal / 2 > 2000)
			{
				return static_cast<float>(atkVal / 660 * 0.1);
			}
			else
			{
				return static_cast<float>(atkVal / 660 * (atkVal / 1000 * 0.1 + 0.1) * 0.5);
			}
		}
		float StatusFunctions::GetIncreasedDamageRateByAtkInPvp(float atkVal)
		{
			if (atkVal <= 0) return 0.0f;

			if (atkVal / 2 > 500)
			{
				return ((pow((0.6f * ((float)atkVal / 2.f) - 300.f), 0.17f) * 10.f - 1.9f) * 0.01f) * 0.5f;
			}
			else
			{
				return (((float)atkVal / 2.f) / 330.0f * 0.1f) * 0.5f;
			}
		}
		float StatusFunctions::GetIncreasedDamageRateForNonPlayer(int str)
		{
			//#ifdef _TAIWAN
			//		return 1.0f + ( str > Constants::MaxStat ? (float)Constants::MaxStat / (float)200 * 0.1f : (float)str / (float)200 * 0.1f );
			//#else
			return 1.0f + (float)str / 660.0f * GetIncreasedRateFactor(str);
			//#endif
		}

		float StatusFunctions::GetIncreasedHealRate(int intelligence)
		{
			return 1.0f + (float)intelligence / 330.0f * GetIncreasedRateFactor(intelligence);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//Physical Functions

		float4x4 PhysicalFunctions::GetOrientation(const float3& position, const float3& direction)
		{
			float4x4 tmp;
			tmp.Direction(position, direction, float3(0, 1, 0));
			return tmp;
		}

		float3 PhysicalFunctions::ApplyGravity(const float3& prev, float dt)
		{
			//		h = (-(t/T-1)^2n+1)*H (t>=T) 
			//		h = (-(-t/T+1)^2n+1)*H (t<T)
			// --> v = v0-((t-v0)^m)*(1/v0^(m-1))  : m=2n
			//float m=4;
			//float t=dt;
			//float v0=prev.y;

			//float v = v0 - abs(pow(t-v0, m)*(pow(1/v0, m-1)));


			float3 g(0, -9.8f, 0);
			float3 v = prev + g * dt;
			return v;
		}

		float3 PhysicalFunctions::ApplyGravity(const float3& position, const float3& prev, float dt)
		{
			float3 v = ApplyGravity(prev, dt);
			float3 pos = position + v * dt;
			if (pos.y < 0) pos.y = 0;
			return pos;
		}

		const float3& PhysicalFunctions::DirectionToDefinedDirection(float3 dir)
		{
			//if (backward) dir *= -1;
			if (dir.x == 0) dir.x = 0.000001f;// devide by zero

			if (dir.x >= 0 && dir.z > 0) {//1��и
				if (dir.x < dir.z) {	//1��Ⱥи -_-餻
					if (dir.z / dir.x > 2.4f)	//67.5��� Ǳ��. -_-;;
						return Constants::DirectionF::Up;
					else
						return Constants::DirectionF::RightUp;
				}
				else {//2��Ⱥи
					if (dir.z / dir.x >= 0.414213f)
						return Constants::DirectionF::RightUp;
					else
						return Constants::DirectionF::Right;
				}
			}
			else if (dir.x >= 0 && dir.z <= 0) {//2��и
				if (dir.x >= -dir.z) {//3��Ⱥи
					if (-dir.z / dir.x < 0.414213f)
						return Constants::DirectionF::Right;
					else
						return Constants::DirectionF::RightDown;
				}
				else {//4��Ⱥи
					if (-dir.z / dir.x <= 2.4f)
						return Constants::DirectionF::RightDown;
					else
						return Constants::DirectionF::Down;
				}
			}
			else if (dir.x < 0 && dir.z < 0) {//3��и
				if (dir.x > dir.z) {//5��Ⱥи
					if (dir.z / dir.x > 2.4f) //67.5鵵� Ǳ��. -_-;;
						return Constants::DirectionF::Down;
					else
						return Constants::DirectionF::LeftDown;
				}
				else {//6��Ⱥи
					if (dir.z / dir.x >= 0.414213f)
						return Constants::DirectionF::LeftDown;
					else
						return Constants::DirectionF::Left;
				}
			}
			else {//4��и
				if (-dir.x >= dir.z) {
					if (dir.z / (-dir.x) < 0.414213f)
						return Constants::DirectionF::Left;
					else
						return Constants::DirectionF::LeftUp;
				}
				else {
					if (dir.z / (-dir.x) < 2.4f)
						return Constants::DirectionF::LeftUp;
					else
						return Constants::DirectionF::Up;
				}
			}
		}

		float3 PhysicalFunctions::GetSpinnedDirection(const float3& targetDirection, float3 sourceDirection, uint16 spinLimit, float dt)
		{
			float spinByTime = spinLimit * (dt / 0.1f);

			float SINLEFT = sin((float)spinByTime * Math::pi / 180.0f), COSLEFT = cos((float)spinByTime * Math::pi / 180.0f);
			float SINRIGHT = sin((float)(360 - spinByTime) * Math::pi / 180.0f), COSRIGHT = cos((float)(360 - spinByTime) * Math::pi / 180.0f);
			int quarterA, quarterB;
			float inclineA = sourceDirection.x ? sourceDirection.z / sourceDirection.x : sourceDirection.z / 0.0000001f;
			float inclineB = targetDirection.x ? targetDirection.z / targetDirection.x : targetDirection.z / 0.0000001f;
			float3 spinnedDirectionLeft(sourceDirection.x * COSLEFT - sourceDirection.z * SINLEFT, 0,
				sourceDirection.x * SINLEFT + sourceDirection.z * COSLEFT);
			float3 spinnedDirectionRight(sourceDirection.x * COSRIGHT - sourceDirection.z * SINRIGHT, 0,
				sourceDirection.x * SINRIGHT + sourceDirection.z * COSRIGHT);
			if (sourceDirection.x >= 0 && sourceDirection.z >= 0)
				quarterA = 1;
			else if (sourceDirection.x >= 0 && sourceDirection.z < 0)
				quarterA = 2;
			else if (sourceDirection.x < 0 && sourceDirection.z < 0)
				quarterA = 3;
			else
				quarterA = 4;
			if (targetDirection.x >= 0 && targetDirection.z >= 0)
				quarterB = 1;
			else if (targetDirection.x >= 0 && targetDirection.z < 0)
				quarterB = 2;
			else if (targetDirection.x < 0 && targetDirection.z < 0)
				quarterB = 3;
			else
				quarterB = 4;

			switch (quarterA) {
			case 1: //����� 1̻�и
				switch (quarterB) {
				case 2: //��ȸ�
					sourceDirection = spinnedDirectionRight;
					break;
				case 4: //���ȸ�
					sourceDirection = spinnedDirectionLeft;
					break;
				case 1:
					if (inclineA < inclineB) {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
					else {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					break;
				default:
					if (inclineA < inclineB) {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					else {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
				}
				break;
			case 2: //����� 2̻�и
				switch (quarterB) {
				case 1: //���ȸ�
					sourceDirection = spinnedDirectionLeft;
					break;
				case 3: //��ȸ�
					sourceDirection = spinnedDirectionRight;
					break;
				case 2:
					if (inclineA < inclineB) {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
					else {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					break;
				default:
					if (inclineA < inclineB) {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					else {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
				}
				break;
			case 3: //����� 3̻�и
				switch (quarterB) {
				case 2: //���ȸ�
					sourceDirection = spinnedDirectionLeft;
					break;
				case 4: //��ȸ�
					sourceDirection = spinnedDirectionRight;
					break;
				case 3:
					if (inclineA < inclineB) {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
					else {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					break;
				default:
					if (inclineA < inclineB) {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					else {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
				}
				break;
			default: //����� 4̻�и
				switch (quarterB) {
				case 3: //���ȸ�
					sourceDirection = spinnedDirectionLeft;
					break;
				case 1: //��ȸ�
					sourceDirection = spinnedDirectionRight;
					break;
				case 4:
					if (inclineA < inclineB) {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
					else {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					break;
				default:
					if (inclineA < inclineB) {//��ȸ�
						sourceDirection = spinnedDirectionRight;
					}
					else {//���ȸ�
						sourceDirection = spinnedDirectionLeft;
					}
				}
			}

			return sourceDirection;
		}

		float3 PhysicalFunctions::RotatePositionByDirection(const float3& position, int angle, const float3& direction)
		{
			//������� ̱����
			if (direction.x == 0) {
				if (direction.z > 0) //8
					return RotatePosition(position, angle);
				else if (direction.z < 0) //2
					return RotatePosition(position, 180 - angle); //360-180
			}
			else if (direction.x < 0) {
				if (direction.z == 0) //4
					return RotatePosition(position, 90 - angle); //360-270
				else if (direction.z > 0) //7
					return RotatePosition(position, 45 - angle); //360-315
				else //1
					return RotatePosition(position, 135 - angle); //360-225
			}
			else {//���
				if (direction.z > 0) //9
					return RotatePosition(position, 315 - angle); //360-45
				else if (direction.z == 0) //6
					return RotatePosition(position, 270 - angle); //360-90
				else // 3
					return RotatePosition(position, 225 - angle); //360-135
			}
			throw;
			//return float3(0,0,0);	//�����Ҽ �� ��ڵ� ��ּ�ó�� by Caspian 071002
		}

		float3 PhysicalFunctions::RotatePosition(const float3& position, int angle)
		{
			float3 spinnedPos = position;
			//ȸ� �ȯ. Up �϶�� ±״�
			spinnedPos.x = position.x * cos(Math::DegToRad<float>((float)angle)) - position.z * sin(Math::DegToRad<float>((float)angle));
			spinnedPos.z = position.x * sin(Math::DegToRad<float>((float)angle)) + position.z * cos(Math::DegToRad<float>((float)angle));
			return spinnedPos;
		}

		float3 PhysicalFunctions::RotatePositionLimitedAngle(const float3& targetPos, int direction)
		{
			switch (direction) {
			case 1:
				return RotatePosition(targetPos, 135);
			case 2:
				return float3(-targetPos.x, 0, -targetPos.z);
			case 3:
				return RotatePosition(targetPos, 225);
			case 4:
				return float3(-targetPos.z, 0, targetPos.x);
			case 6:
				return float3(targetPos.z, 0, -targetPos.x);
			case 7:
				return RotatePosition(targetPos, 45);
			case 8:
				return targetPos;
			case 9:
				return RotatePosition(targetPos, 315);
			}
			throw;
		}

		bool PhysicalFunctions::CheckCollision(const float3& targetPos, float range, ObjectData& objData)
		{
			if (Math::Length<float>(objData.Position - targetPos) <= objData.Radius + range)
				//if ( Math::Length<float>( float2(objData.Position.x-targetPos.x, objData.Position.z-targetPos.z) ) <= objData.Radius+range )
				return true;
			return false;
		}

		bool PhysicalFunctions::CheckCollision(const float3& targetPos, const float3& targetDirection, float range, ObjectData& objData)
		{
			float3 pos = objData.Position;
			float radius = objData.Radius;

			//��.. ڼ�� а� ĵ��.

			//ټ����϶� (y=b), �����϶� (x=X), a=1 �϶�, a=-1 �϶� �װ��� α����ؼ� Ǯ�����.
			if (targetDirection.x == 0) {	//ڼ�� ��϶� (x=X)
				float startY = targetPos.z, endY = targetPos.z + range * targetDirection.z;
				//�ϴ ���� Ǽ ������. (r^2 - (X-a')^2
				float determinant = pow(radius, 2) - pow((targetPos.x - pos.x), 2);
				if (determinant < 0) //ڱ��� ̾�. �� ��� �ʾҴ.
					return false;
				else if (determinant == 0) {//ٱ��� ��ϳ��.
					float y = pos.z;
					if (y >= (startY > endY ? endY : startY) && y < (startY > endY ? startY : endY))
						// ٵ�� �¾Ҵ پ���. ��׾�.
						return true;
				}
				else {//���� ̵ΰ��.
					float sqrtDeterminant = sqrt(determinant);
					float y1 = pos.z + sqrtDeterminant, y2 = pos.z - sqrtDeterminant;
					if (y1 >= (startY > endY ? endY : startY) && y1 < (startY > endY ? startY : endY))
						// ٵ�� �¾Ҵ پ���. ��׾�.
						return true;
					else if (y2 >= (startY > endY ? endY : startY) && y2 < (startY > endY ? startY : endY))
						return true;
				}
			}
			else if (targetDirection.z == 0) { //��� ��϶� (y=b)
				float startX = targetPos.x, endX = targetPos.x + range * targetDirection.x;
				//�ϴ ���� Ǽ ������. (r^2 - (Y-b')^2
				float determinant = pow(radius, 2) - pow((targetPos.z - pos.z), 2);
				if (determinant < 0) //ڱ��� ̾�. �� ��� �ʾҴ.
					return false;
				else if (determinant == 0) {//ٱ��� ��ϳ��.
					float x = pos.x;
					if (x >= (startX > endX ? endX : startX) && x < (startX > endX ? startX : endX))
						// ٵ�� �¾Ҵ پ���. ��׾�.
						return true;
				}
				else {//���� ̵ΰ��.
					float sqrtDeterminant = sqrt(determinant);
					float x1 = pos.x + sqrtDeterminant, x2 = pos.x - sqrtDeterminant;
					if (x1 >= (startX > endX ? endX : startX) && x1 < (startX > endX ? startX : endX))
						return true;
					else if (x2 >= (startX > endX ? endX : startX) && x2 < (startX > endX ? startX : endX))
						return true;
				}
			}
			else if (targetDirection.z / targetDirection.x == 1) { // a=1��϶�
				float startX = targetPos.x, endX = targetPos.x + (sqrt(pow(range, 2) / 2)) * targetDirection.x;
				//��� Ǽ ��. (b+a`+b`)^2 - 2(b^2 + 2a`b + a`^2 + b`^2 - r^2)
				float b = targetPos.z - targetPos.x;
				float determinant = pow((b + pos.x + pos.z), 2)
					- 2 * (pow(b, 2) + 2 * pos.x * b + pow(pos.x, 2) + pow(pos.z, 2) - pow(radius, 2));
				if (determinant < 0) //걳�� ̾�. �� ��� �ʾҴ.
					return false;
				else if (determinant == 0) {//ٱ��� ��ϳ��.
					float y = (b + pos.x + pos.z) / 2;
					float x = y - b;
					if (x >= (startX > endX ? endX : startX) && x < (startX > endX ? startX : endX))
						// ٵ�� �¾Ҵ پ���. ��׾�.
						return true;
				}
				else {//���� ̵ΰ��.
					float sqrtDeterminant = sqrt(determinant);
					float y1 = ((b + pos.x + pos.z) + sqrtDeterminant) / 2, y2 = ((b + pos.x + pos.z) - sqrtDeterminant) / 2;
					float x1 = y1 - b, x2 = y2 - b;
					if (x1 >= (startX > endX ? endX : startX) && x1 < (startX > endX ? startX : endX))
						return true;
					else if (x2 >= (startX > endX ? endX : startX) && x2 < (startX > endX ? startX : endX))
						return true;
				}
			}
			else if (targetDirection.z / targetDirection.x == -1) { // a=-1��϶�
				float startX = targetPos.x, endX = targetPos.x + (sqrt(pow(range, 2) / 2)) * targetDirection.x;

				float b = targetPos.z + targetPos.x;
				float determinant = pow((b - pos.x + pos.z), 2)
					- 2 * (pow(b, 2) - 2 * pos.x * b + pow(pos.x, 2) + pow(pos.z, 2) - pow(radius, 2));
				if (determinant < 0) //걳�� ̾�. �� ��� �ʾҴ.
					return false;
				else if (determinant == 0) {//ٱ��� ��ϳ��.
					float y = (b - pos.x + pos.z) / 2;
					float x = b - y;
					if (x >= (startX > endX ? endX : startX) && x < (startX > endX ? startX : endX))
						// ٵ�� �¾Ҵ پ���. ��׾�.
						return true;
				}
				else {//���� ̵ΰ��.
					float sqrtDeterminant = sqrt(determinant);
					float y1 = ((b - pos.x + pos.z) + sqrtDeterminant) / 2, y2 = ((b - pos.x + pos.z) - sqrtDeterminant) / 2;
					float x1 = b - y1, x2 = b - y2;
					if (x1 >= (startX > endX ? endX : startX) && x1 < (startX > endX ? startX : endX))
						return true;
					else if (x2 >= (startX > endX ? endX : startX) && x2 < (startX > endX ? startX : endX))
						return true;
				}
			}
			else { //��̷���� º�ä� ��浹 �˻� ù߻.
				float startX = targetPos.x, endX = targetPos.x + (sqrt(pow(range, 2) / 2)) * (targetDirection.x);
				//���� Ǽ ��. (b+a`+b`)^2 - 2(b^2 + 2a`b + a`^2 + b`^2 - r^2)
				float b = targetPos.z - targetPos.x;
				float determinant = pow((b + pos.x + pos.z), 2)
					- 2 * (pow(b, 2) + 2 * pos.x * b + pow(pos.x, 2) + pow(pos.z, 2) - pow(radius, 2));
				if (determinant < 0) //걳�� ̾�. �� ��� �ʾҴ.
					return false;
				else if (determinant == 0) {//ٱ��� ��ϳ��.
					float y = (b + pos.x + pos.z) / 2;
					float x = y - b;
					if (x >= (startX > endX ? endX : startX) && x < (startX > endX ? startX : endX))
						// ٵ�� �¾Ҵ پ���. ��׾�.
						return true;
				}
				else {//���� ̵ΰ��.
					float sqrtDeterminant = sqrt(determinant);
					float y1 = ((b + pos.x + pos.z) + sqrtDeterminant) / 2, y2 = ((b + pos.x + pos.z) - sqrtDeterminant) / 2;
					float x1 = y1 - b, x2 = y2 - b;
					if (x1 >= (startX > endX ? endX : startX) && x1 < (startX > endX ? startX : endX))
						return true;
					else if (x2 >= (startX > endX ? endX : startX) && x2 < (startX > endX ? startX : endX))
						return true;
				}
			}
			return false;
		}

		bool PhysicalFunctions::CheckCollision(const float3& targetPos, const float3& direction, float height, float width, ObjectData& objData)
		{
			float3 tmpDir = direction;
			if (CheckCollision(targetPos, direction, height, objData))
				return true;

			if (tmpDir == Constants::DirectionF::Up) {
				if (CheckCollision(targetPos + Constants::DirectionF::Right * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::RightUp) {
				if (CheckCollision(targetPos + Constants::DirectionF::RightDown * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::Right) {
				if (CheckCollision(targetPos + Constants::DirectionF::Down * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::RightDown) {
				if (CheckCollision(targetPos + Constants::DirectionF::LeftDown * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::Down) {
				if (CheckCollision(targetPos + Constants::DirectionF::Left * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::LeftDown) {
				if (CheckCollision(targetPos + Constants::DirectionF::LeftUp * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::Left) {
				if (CheckCollision(targetPos + Constants::DirectionF::Up * width, tmpDir, height, objData))
					return true;
			}
			else if (tmpDir == Constants::DirectionF::LeftUp) {
				if (CheckCollision(targetPos + Constants::DirectionF::RightUp * width, tmpDir, height, objData))
					return true;
			}

			float3 pos = objData.Position - targetPos;
			float3 spinnedPos = pos;
			if (tmpDir == Constants::DirectionF::RightUp) {
				spinnedPos = PhysicalFunctions::RotatePosition(pos, 45);
			}
			else if (tmpDir == Constants::DirectionF::Right) {
				spinnedPos.x = -pos.z;
				spinnedPos.z = pos.x;
			}
			else if (tmpDir == Constants::DirectionF::RightDown) {
				spinnedPos = PhysicalFunctions::RotatePosition(pos, 135);
			}
			else if (tmpDir == Constants::DirectionF::Down) {
				spinnedPos.x = -pos.x;
				spinnedPos.z = -pos.z;
			}
			else if (tmpDir == Constants::DirectionF::LeftDown) {
				spinnedPos = PhysicalFunctions::RotatePosition(pos, 225);
			}
			else if (tmpDir == Constants::DirectionF::Left) {
				spinnedPos.x = pos.z;
				spinnedPos.z = -pos.x;
			}
			else if (tmpDir == Constants::DirectionF::LeftUp) {
				spinnedPos = PhysicalFunctions::RotatePosition(pos, 315);
			}
			if (spinnedPos.x >= 0 && spinnedPos.x <= width && spinnedPos.z >= 0 && spinnedPos.z <= height)
				return true;

			return false;
		}

		bool PhysicalFunctions::CheckCollision(const float3& targetPos, const float3& targetDirection, float range, float startAngle, float endAngle, ObjectData& objData)
		{
			float3 pos = objData.Position;
			float radius = objData.Radius;

			float x = pos.x - targetPos.x;
			float y = pos.z - targetPos.z;
			float b = abs(x);
			float a = sqrt(pow(x, 2) + pow(y, 2));
			float targetAngle = Math::RadToDeg<float>(acos(b / a));
			if (x > 0 && y >= 0)
				targetAngle = 90 - targetAngle;
			else if (x > 0 && y <= 0)
				targetAngle += 90;
			else if (x < 0 && y <= 0)
				targetAngle = 270 - targetAngle;
			else if (x < 0 && y >= 0)
				targetAngle += 270;

			float sAngle = startAngle, eAngle = endAngle;
			if (targetDirection.x > 0 && targetDirection.z > 0) {//45
				sAngle += 45.0f;
				eAngle += 45.0f;
			}
			else if (targetDirection.x > 0 && targetDirection.z == 0) {//90
				sAngle += 90.0f;
				eAngle += 90.0f;
			}
			else if (targetDirection.x > 0 && targetDirection.z < 0) {//135
				sAngle += 135.0f;
				eAngle += 135.0f;
			}
			else if (targetDirection.x == 0 && targetDirection.z < 0) {//180
				sAngle += 180.0f;
				eAngle += 180.0f;
			}
			else if (targetDirection.x < 0 && targetDirection.z < 0) {//225
				sAngle += 225.0f;
				eAngle += 225.0f;
			}
			else if (targetDirection.x < 0 && targetDirection.z == 0) {//270
				sAngle += 270.0f;
				eAngle += 270.0f;
			}
			else if (targetDirection.x < 0 && targetDirection.z > 0) {//315
				sAngle += 315.0f;
				eAngle += 315.0f;
			}
			sAngle = sAngle >= 360 ? sAngle - 360 : sAngle;
			eAngle = eAngle >= 360 ? eAngle - 360 : eAngle;
			if (sAngle < eAngle) {
				if (targetAngle >= sAngle && targetAngle <= eAngle) {
					if (a <= range + radius)
						return true;
				}
				else {

				}
			}
			else if (sAngle >= eAngle)
				if (targetAngle >= sAngle || targetAngle <= eAngle)
					if (a <= range + radius)
						return true;
			return false;
		}

		bool IsAbleToJoinStage(StageLicense target, const std::vector<StageLicense>& myLicenses)
		{
			std::vector<StageLicense>::const_iterator itr = std::find_if(myLicenses.begin(), myLicenses.end(),
				StageLocation::FinderOnlyStageGroupHash(target.StageGroupHash));
			if (itr == myLicenses.end()) return false; 
			if ((*itr).Level >= target.Level) return true;
			return false;
		}

		void PvpRoomInfo::BattleGroundInfoType::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PvpRoomInfo::BattleGroundInfoType");
			out.Write(L"GoalKillCount", GoalKillCount);
			out.Write(L"LimitPlayTime", LimitPlayTime);
		}


		void PvpRoomInfo::BattleGroundInfoType::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PvpRoomInfo::BattleGroundInfoType");
			in.Read(L"GoalKillCount", GoalKillCount);
			in.Read(L"LimitPlayTime", LimitPlayTime);
		}

		void PvpRoomInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PvpRoomInfo");
			out.Write(L"Id", Id);
			out.Write(L"Title", Title);
			out.WriteEnum(L"RoomStatus", RoomStatus);
			out.WriteEnum(L"GameType", GameType);
			out.Write(L"Map", Map);
			out.Write(L"Owner", Owner);
			out.Write(L"IsPrivateRoom", IsPrivateRoom);
			out.Write(L"IsRandomTeam", IsRandomTeam);
			out.Write(L"IsEquipSetRewardDisable", IsEquipSetRewardDisable);
			out.Write(L"IsPetNotUsable", IsPetNotUsable);
			out.Write(L"IsRebirthSkillDisable", IsRebirthSkillDisable);
			out.Write(L"IsInvincibleAfterRevive", IsInvincibleAfterRevive);
			out.Write(L"StatusLimit", StatusLimit);
			//out.Write(L"RandomMap", RandomMap);
			out.Write(L"MaxNumberOfMembersPerTeam", MaxNumberOfMembersPerTeam);
			out.Write(L"MaxNumberOfSpectator", MaxNumberOfSpectator);

			out.Write(L"BattleGroundInfo", BattleGroundInfo);
		}
		void PvpRoomInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PvpRoomInfo");
			in.Read(L"Id", Id);
			in.Read(L"Title", Title);
			in.ReadEnum(L"RoomStatus", RoomStatus);
			in.ReadEnum(L"GameType", GameType);
			in.Read(L"Map", Map);
			in.Read(L"Owner", Owner);
			in.Read(L"IsPrivateRoom", IsPrivateRoom);
			in.Read(L"IsRandomTeam", IsRandomTeam);
			in.Read(L"IsEquipSetRewardDisable", IsEquipSetRewardDisable);
			in.Read(L"IsPetNotUsable", IsPetNotUsable);
			in.Read(L"IsRebirthSkillDisable", IsRebirthSkillDisable);
			in.Read(L"IsInvincibleAfterRevive", IsInvincibleAfterRevive);
			in.Read(L"StatusLimit", StatusLimit);

			//in.Read(L"RandomMap", RandomMap);
			in.Read(L"MaxNumberOfMembersPerTeam", MaxNumberOfMembersPerTeam);
			in.Read(L"MaxNumberOfSpectator", MaxNumberOfSpectator);

			in.Read(L"BattleGroundInfo", BattleGroundInfo);
		}


		void PvpChannelInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PvpChannelInfo");
			out.Write(L"Name", Name);
			out.Write(L"MinLevel", MinLevel);
			out.Write(L"MaxLevel", MaxLevel);
			out.WriteEnum(L"Status", Status);
		}

		void PvpChannelInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PvpChannelInfo");
			in.Read(L"Name", Name);
			in.Read(L"MinLevel", MinLevel);
			in.Read(L"MaxLevel", MaxLevel);
			in.ReadEnum(L"Status", Status);
		}

		StylePointProcess::StylePointProcess(int minusScore, int minusTime, float startGab)
			: currentPoint(0), nowProcess(false), timerData(0.f), maxPoint(0), minusStartTime(0.f), stopCount(0), playerCount(1)
		{
			processMinusScore = minusScore;
			processMinusTime = minusTime;
			processStartGab = startGab;
		}
		StylePointProcess::~StylePointProcess()
		{
		}
		void StylePointProcess::Start(int startScore, int maxScroe)
		{
			timerData = static_cast<float>(processMinusTime);
			nowProcess = true;

			currentPoint = startScore;
			maxPoint = maxScroe;
			minusStartTime = processStartGab;
			stopCount = 0;
		}
		bool StylePointProcess::ReStart()
		{
			if (nowProcess == true) return false;

			--stopCount;
			if (stopCount > 0) return false;

			nowProcess = true;
			timerData = static_cast<float>(processMinusTime);

			minusStartTime = 10.f;
			stopCount = 0;

			return true;
		}
		bool StylePointProcess::Stop()
		{
			++stopCount;

			if (nowProcess == false) return false;
			if (stopCount > 1) return false;

			nowProcess = false;
			return true;
		}
		bool StylePointProcess::Process(float dt)
		{
			if (nowProcess == false)
				return false;
			if (currentPoint <= 0)
				return false;

			if (minusStartTime > 0.f) {
				minusStartTime -= dt;
				return false;
			}

			if (timerData <= 0.f)
			{
				currentPoint -= processMinusScore;
				timerData += processMinusTime;

				if (currentPoint <= 0) currentPoint = 0;
			}
			else
			{
				timerData -= dt;
			}

			return true;
		}

		int StylePointProcess::AddStylePoint(int addValue)
		{
			int realAddPoint = 0;
			if (nowProcess) {
				switch (playerCount)
				{
				case 3:
					realAddPoint = static_cast<int>(static_cast<float>(addValue) * 0.9f);
					break;
				case 2:
					realAddPoint = static_cast<int>(static_cast<float>(addValue) * 0.8f);
					break;
				case 1:
					realAddPoint = static_cast<int>(static_cast<float>(addValue) * 0.7f);
					break;
				default:
					realAddPoint = addValue;
				}

				currentPoint += realAddPoint;
				if (currentPoint >= maxPoint)
					currentPoint = maxPoint;
			}

			return realAddPoint;
		}

		void StylePointProcess::SetCurrentStylePoint(int point)
		{
			currentPoint = point;
		}

		float StylePointProcess::GetGaguePercentage()
		{
			if (currentPoint != 0)
				return static_cast<float>(currentPoint) / static_cast<float>(maxPoint);
			else
				return 0;
		}


		const float Pet::ExpUpPerSec = 1.0;
		const float Pet::MaxFull = 100000.0f;
		const float Pet::MaxOverFull = 1000000.0f;
		const float Pet::AddEffectStateTime = 60.0f;

		const float Pet::Feeling::Border::OverFull = 0.90f;
		const float Pet::Feeling::Border::UnderFull_OverGood = 0.75f;
		const float Pet::Feeling::Border::UnderGood_OverSoso = 0.50f;
		const float Pet::Feeling::Border::UnderSoso_OverNotGood = 0.25f;
		const float Pet::Feeling::Border::UnderNotGood_OverHungry = 0.03f;
		const float Pet::Feeling::Border::UnderHungry_OverStarving = 0.00f;

		const float Pet::TiredFactorRelatedWithFull::Full = 1.6f;
		const float Pet::TiredFactorRelatedWithFull::Good = 1.2f;
		const float Pet::TiredFactorRelatedWithFull::Soso_NotGood = 0.8f;
		const float Pet::TiredFactorRelatedWithFull::Hungry_Starving = 0.4f;

		const float Pet::RareProbabilityFactorRelatedWithFull::Full_Good = 1.0f;
		const float Pet::RareProbabilityFactorRelatedWithFull::Soso = 0.7f;
		const float Pet::RareProbabilityFactorRelatedWithFull::NotGood = 0.5f;
		const float Pet::RareProbabilityFactorRelatedWithFull::Hungry_Starving = 0.3f;

		const float Pet::RelatedWithPresentNFull::PresentLimitFull = 1.00f; // L
		const float Pet::RelatedWithPresentNFull::Constant_C = 4000000.0f;  // C
		const float Pet::RelatedWithPresentNFull::Constant_A = 10.0f;       // A

		PetCaredBySchool::PetCaredBySchool()
			: ExpFactor(0.0f), Start(0, 0, 0, 0, 0, 0), End(0, 0, 0, 0, 0, 0)
		{
		}

		void PetCaredBySchool::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"PetCaredBySchool");
			out.Write(L"PetItemHash", PetItemHash);
			out.Write(L"PetItemSerial", PetItemSerial);
			out.Write(L"PetItemInstanceEx", PetItemInstanceEx);
			out.Write(L"PetItemCount", PetItemCount);
			out.Write(L"ExpFactor", ExpFactor);
			out.Write(L"Start", Start);
			out.Write(L"End", End);
		}

		void PetCaredBySchool::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"PetCaredBySchool");
			in.Read(L"PetItemHash", PetItemHash);
			in.Read(L"PetItemSerial", PetItemSerial);
			in.Read(L"PetItemInstanceEx", PetItemInstanceEx);
			in.Read(L"PetItemCount", PetItemCount);
			in.Read(L"ExpFactor", ExpFactor);
			in.Read(L"Start", Start);
			in.Read(L"End", End);
		}

		void PetToolTipInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PetToolTipInfo");
			out.Write(L"IsRarePet", IsRarePet);
			out.Write(L"PetName", PetName);
			out.Write(L"Level", Level);
			out.Write(L"Exp", Exp);
			out.Write(L"RareProbability", RareProbability);
			out.Write(L"EnchantSerial", EnchantSerial);
		}
		void PetToolTipInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PetToolTipInfo");
			in.Read(L"IsRarePet", IsRarePet);
			in.Read(L"PetName", PetName);
			in.Read(L"Level", Level);
			in.Read(L"Exp", Exp);
			in.Read(L"RareProbability", RareProbability);
			in.Read(L"EnchantSerial", EnchantSerial);
		}


		Pet::Pet()
			: PetSerial(0), PetHash(0), Level(0), Exp(0), Appear(false), Full(0.0f), PrevFull(0.0f),
			TiredFactor(1.0f), ExpFactor(1.0f),
			EffectStateDelayTime(0.0f), EffectStateDelayTimeForDisplay(0.0f), EmotionDelay(0.0f),
			RareProbability(0.0f), FullSum(0.0f), LevelUpPeriod(0.0f),
			NextLevelExp(0), IsRarePet(false), Equipments(Constants::Equipment::MaxPetEquipCnt)
			, EnchantSerial(0) 
		{
			Equipments.reserve(3);
			PetName.reserve(50);
		}

		void Pet::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Pet");
			out.Write(L"IsRarePet", IsRarePet);
			out.Write(L"PetSerial", PetSerial);
			out.Write(L"PetName", PetName);
			out.Write(L"PetHash", PetHash);
			out.Write(L"Level", Level);
			out.Write(L"Exp", Exp);
			out.Write(L"NextLevelExp", NextLevelExp);
			out.Write(L"Appear", Appear);
			out.Write(L"Full", Full);
			out.Write(L"TiredFactor", TiredFactor);
			out.Write(L"ExpFactor", ExpFactor);
			out.Write(L"EffectStateDelayTime", EffectStateDelayTime);
			out.Write(L"EmotionDelay", EmotionDelay);
			out.Write(L"RareProbability", RareProbability);
			out.Write(L"FullSum", FullSum);
			out.Write(L"LevelUpPeriod", LevelUpPeriod);
			out.Write(L"EnchantSerial", EnchantSerial);
		}

		void Pet::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Pet");
			in.Read(L"IsRarePet", IsRarePet);
			in.Read(L"PetSerial", PetSerial);
			in.Read(L"PetName", PetName);
			in.Read(L"PetHash", PetHash);
			in.Read(L"Level", Level);
			in.Read(L"Exp", Exp);
			in.Read(L"NextLevelExp", NextLevelExp);
			in.Read(L"Appear", Appear);
			in.Read(L"Full", Full);
			in.Read(L"TiredFactor", TiredFactor);
			in.Read(L"ExpFactor", ExpFactor);
			in.Read(L"EffectStateDelayTime", EffectStateDelayTime);
			EffectStateDelayTimeForDisplay = EffectStateDelayTime;	//�Ŭ���̾�Ʈ ����ȭ�� �� ��߰��.
			in.Read(L"EmotionDelay", EmotionDelay);
			in.Read(L"RareProbability", RareProbability);
			in.Read(L"FullSum", FullSum);
			in.Read(L"LevelUpPeriod", LevelUpPeriod);
			in.Read(L"EnchantSerial", EnchantSerial);
		}

		void Pet::Equipment(uint32 itemHash, int64 instance, Constants::Equipment position)
		{
			int pos = static_cast<int>(position) - static_cast<int>(XRated::Constants::Equipment::Pet_Mask);
			if ((Constants::Equipment::MaxPetEquipCnt > pos) && (pos >= 0)) {
				Equipments.at(pos).Id = itemHash;
				Equipments.at(pos).InstanceEx = instance;
				return;
			}
			Logger::GetInstance().Info("invalid pet equipment position : {0}", position);
			throw(fmt::format(L"invalid pet equipment position : {0}", position));
		}
		void Pet::UnEquipment(Constants::Equipment position)
		{
			int pos = static_cast<int>(position) - static_cast<int>(XRated::Constants::Equipment::Pet_Mask);
			if ((Constants::Equipment::MaxPetEquipCnt > pos) && (pos >= 0)) {
				Equipments.at(pos).Id = 0;
				Equipments.at(pos).InstanceEx = 0;
				return;
			}
			Logger::GetInstance().Info("invalid pet equipment position : {0}", position);
			throw(fmt::format(L"invalid pet equipment position : {0}", position));
		}
		void Pet::AllUnEquipment()
		{
			size_t max = Equipments.size();
			for (size_t i = 0; i < max; ++i)
			{
				Equipments.at(i).Id = 0;
			}
		}
		const Item* Pet::GetEquipment(Constants::Equipment position)
		{
			int pos = static_cast<int>(position) - static_cast<int>(XRated::Constants::Equipment::Pet_Mask);
			if ((Constants::Equipment::MaxPetEquipCnt > pos) && (pos >= 0)) {
				if (Equipments.at(pos).Id != 0) {
					return &Equipments.at(pos);
				}
				return NULL;
			}
			Logger::GetInstance().Info("invalid pet equipment position : {0}", position);
			throw(fmt::format(L"invalid pet equipment position : {0}", position));
		}

		bool Pet::operator!=(const Pet& in) const
		{
			if (PetSerial != in.PetSerial)
				return true;

			return false;
		}

		float Pet::GetFullChangeByPresent(uint32 sellPrice, uint32 count) const
		{
			float l = RelatedWithPresentNFull::PresentLimitFull * 100.0f;
			float c = RelatedWithPresentNFull::Constant_C;
			float a = RelatedWithPresentNFull::Constant_A;
			float x = sqrt(static_cast<float>(sellPrice)) * static_cast<float>(count) * 30.0f;
			float p = GetFullRatio() * 100.0f;

			return ((l - (c * l / (((c * p) / (l - p)) + a * l * x + c)) - p) / 100.0f) * Pet::MaxFull;
		}

		void Pet::Feed(uint32 amount, uint32 count, bool overFeed)
		{
			Full += static_cast<float>(amount * count);

			if (overFeed)
			{
				if (Full > Pet::MaxOverFull)
				{
					Full = Pet::MaxOverFull;
				}
			}
			else
			{
				if (Full > Pet::MaxFull)
				{
					Full = Pet::MaxFull;
				}
			}
		}

		void Pet::Give(uint32 sellPrice, uint32 count)
		{
			const static float MaxPercentByGive = 0.8f;

			Full += GetFullChangeByPresent(sellPrice, count);

			if (Full > Pet::MaxFull)
				Full = Pet::MaxFull;

			if (GetFullRatio() > MaxPercentByGive)
			{
				Full = Pet::MaxFull * MaxPercentByGive;
			}
		}

		int Pet::GetFeedableFoodCount(uint32 foodAmount) const
		{
			float feedableAmount = Pet::MaxFull - Full;

			if (feedableAmount < 0.0f)
			{
				return -1;
			}

			return static_cast<int>(feedableAmount / static_cast<float>(foodAmount));
		}

		int Pet::GetOverFeedableFoodCount(uint32 foodAmount) const
		{
			float feedableAmount = Pet::MaxFull - Full;

			if (feedableAmount < 0.0f)
			{
				return -1;
			}

			return static_cast<int>(feedableAmount / static_cast<float>(foodAmount));
		}

		bool Pet::Update(float dt)
		{
			if (PetName != L"")
			{
				FullSum += (GetFullRatio() * dt);
				LevelUpPeriod += dt;
			}

			if (Appear == true)
			{
				Exp += (dt * ExpUpPerSec * ExpFactor);
				Full -= static_cast<float>((dt * TiredFactor * GetTiredFactorRelatedWithFull()));


				if (Full < 0.0f)
				{
					Full = 0.0f;
					return false;
				}

				EffectStateDelayTime += dt;
				EffectStateDelayTimeForDisplay += dt;

				if (EffectStateDelayTimeForDisplay > AddEffectStateTime)// Լ����� ǵ���ȭ ��� 5�� ����� ΰ���Ѵ.
				{
					EffectStateDelayTimeForDisplay = 0.0f;
				}

				if (EmotionDelay > 0.0f)
					EmotionDelay -= dt;

				return true;
			}

			return false;
		}

		void Pet::UpdateByPetSchool(float dt, float schoolExpFactor)
		{
			if (Appear == true)
			{
				return;
			}
			else
			{
				Exp += dt * ExpUpPerSec * (ExpFactor + schoolExpFactor);
			}
		}

		int Pet::GetBuffIndex() const
		{
			switch (GetFeeling())
			{
			case Feeling::State::Full:
			case Feeling::State::Good:
				return 3;
			case Feeling::State::Soso:
				return 2;
			case Feeling::State::NotGood:
				return 1;
			case Feeling::Hungry:
			case Feeling::Starving:
				return 0;
			}

			return 0;
		}

		double Pet::GetTiredFactorRelatedWithFull() const
		{
			switch (GetFeeling())
			{
			case Feeling::State::Full:
			case Feeling::State::Good:
				return TiredFactorRelatedWithFull::Full;
			case Feeling::State::Soso:
				return TiredFactorRelatedWithFull::Good;
			case Feeling::State::NotGood:
				return TiredFactorRelatedWithFull::Soso_NotGood;
			case Feeling::Hungry:
			case Feeling::Starving:
				return TiredFactorRelatedWithFull::Hungry_Starving;
			}

			return 0.0;
		}

		float Pet::GetAddedRareProbability(double fullRate, float maxRareProbability, float rareProbabilityAddPerLevel, uint32 level) const
		{
			float m = maxRareProbability;
			float d = rareProbabilityAddPerLevel;
			float t = static_cast<float>(Pet::MaxLevel);
			float lv = static_cast<float>(level);
			float f = 0.0f;

			if (lv < 1.0f)		    return 0.0f;
			if (fullRate > 1.0f)	fullRate = 1.0f;

			if (fullRate >= Feeling::Border::UnderFull_OverGood)
				f = RareProbabilityFactorRelatedWithFull::Full_Good;
			else if (fullRate >= Feeling::Border::UnderGood_OverSoso)
				f = RareProbabilityFactorRelatedWithFull::Soso;
			else if (fullRate >= Feeling::Border::UnderSoso_OverNotGood)
				f = RareProbabilityFactorRelatedWithFull::NotGood;
			else
				f = RareProbabilityFactorRelatedWithFull::Hungry_Starving;

			return ((m / (t - 1.0f) - (t - 2.0f) * d / 2 + (lv - 1.0f) * d) * f);
		}

		float Pet::GetFullRatio() const
		{
			return Full / Pet::MaxFull;
		}

		float Pet::GetPrevFullRatio() const
		{
			return PrevFull / Pet::MaxFull;
		}

		Pet::Feeling::State Pet::GetFeeling() const
		{
			float fullRatio = GetFullRatio();

			if (fullRatio > Feeling::Border::OverFull)
				return Feeling::State::Full;
			else if (fullRatio > Feeling::Border::UnderFull_OverGood)
				return Feeling::State::Good;
			else if (fullRatio > Feeling::Border::UnderGood_OverSoso)
				return Feeling::State::Soso;
			else if (fullRatio > Feeling::Border::UnderSoso_OverNotGood)
				return Feeling::State::NotGood;
			else if (fullRatio > Feeling::Border::UnderNotGood_OverHungry)
				return Feeling::Hungry;

			return Feeling::Starving;
		}

		Pet::Feeling::State Pet::GetPrevFeeling() const
		{
			float fullRatio = GetPrevFullRatio();

			if (fullRatio > Feeling::Border::OverFull)
				return Feeling::State::Full;
			else if (fullRatio > Feeling::Border::UnderFull_OverGood)
				return Feeling::State::Good;
			else if (fullRatio > Feeling::Border::UnderGood_OverSoso)
				return Feeling::State::Soso;
			else if (fullRatio > Feeling::Border::UnderSoso_OverNotGood)
				return Feeling::State::NotGood;
			else if (fullRatio > Feeling::Border::UnderNotGood_OverHungry)
				return Feeling::Hungry;

			return Feeling::Starving;
		}
		bool Pet::IsValidPetNameSize(const std::wstring& name)
		{
			std::string tmp = StringUtil::To<std::string>(name);
			if (tmp.size() <= MaxPetNameSize) {
				return true;
			}
			return false;
		}

		PetItemSlot::PetItemSlot()
			: Position(0), ItemHash(0), InstanceEx(0), Type(PositionType::Invaild), Stacked(0)
		{
		}

		PetItemSlot::PetItemSlot(const PositionType& Type, const uint8& position, const uint32& itemHash, const XRated::InstanceEx& instance, const uint16& stacked)
			: Position(position), ItemHash(itemHash), InstanceEx(instance), Type(Type), Stacked(stacked)
		{
		}

		void PetItemSlot::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PetItemSlot");
			out.WriteEnum(L"Type", Type);
			out.Write(L"Position", Position);
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"Stacked", Stacked);

			out.Write(L"instanceEx", InstanceEx);
		}

		void PetItemSlot::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PetItemSlot");
			in.ReadEnum(L"Type", Type);
			in.Read(L"Position", Position);
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"Stacked", Stacked);

			in.Read(L"instanceEx", InstanceEx);
		}

		void PetDataWithItemPos::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::PetDataWithItemPos");
			out.Write(L"Pet", Pet);
			out.Write(L"PetItemPosition", PetItemPosition);
		}

		void PetDataWithItemPos::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::PetDataWithItemPos");
			in.Read(L"Pet", Pet);
			in.Read(L"PetItemPosition", PetItemPosition);
		}
		bool StoreSlotForPet::operator ==(const StoreSlot& rhs) const
		{
			return (ItemHash == rhs.ItemHash 
				&& StackedCount == rhs.StackedCount 
				&& this->InstanceEx== rhs.InstanceEx 
				&& SellPrice == rhs.SellPrice);
		}
		void StoreSlotForPet::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::StoreSlotForPet");
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"StackedCount", StackedCount);
			out.Write(L"instanceEx", InstanceEx);
			out.Write(L"SellPrice", SellPrice);
			out.Write(L"PetData", PetData);
		}
		void StoreSlotForPet::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::StoreSlotForPet");
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"StackedCount", StackedCount);
			in.Read(L"instanceEx", InstanceEx);
			in.Read(L"SellPrice", SellPrice);
			in.Read(L"PetData", PetData);
		}

		void ConfirmTradeInfo::TradeItem::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::ConfirmTradeInfo::TradeItem");
			out.Write(L"hash", hash);
			out.Write(L"instanceEx", InstanceEx);
			out.Write(L"count", count);
			out.Write(L"isPetItem", isPetItem);
			if (isPetItem)
				out.Write(L"pet", pet);
		}

		void ConfirmTradeInfo::TradeItem::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::ConfirmTradeInfo::TradeItem");
			in.Read(L"hash", hash);
			in.Read(L"instanceEx", InstanceEx);
			in.Read(L"count", count);
			in.Read(L"isPetItem", isPetItem);
			if (isPetItem)
				in.Read(L"pet", pet);
		}

		void ConfirmTradeInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::ConfirmTradeInfo");
			out.Write(L"playerSerial", playerSerial);
			out.Write(L"playerName", playerName);
			out.Write(L"items", items);
			out.Write(L"money", money);
		}

		void ConfirmTradeInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::ConfirmTradeInfo");
			in.Read(L"playerSerial", playerSerial);
			in.Read(L"playerName", playerName);
			in.Read(L"items", items);
			in.Read(L"money", money);
		}

		const int Constants::SecurityCard::MaxRow = 10;
		const int Constants::SecurityCard::MaxColumn = 8;
		const int Constants::SecurityCard::QuestionCount = 3;

		/*String Constants::SecurityCard::GetSecurityCodeString(int row, int col)
		{
			return StringUtil::ToUnicode(StringUtil::Format("%c%d", (65 + col), row));
		}*/


		void Family::Info::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Family::Info");
			out.Write(L"Serial", Serial);
			out.Write(L"CreatedTime", CreatedTime);
		}

		void Family::Info::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Family::Info");
			in.Read(L"Serial", Serial);
			in.Read(L"CreatedTime", CreatedTime);
		}
		void Family::MemberInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Family::MemberInfo");
			out.Write(L"MemberSerial", MemberSerial);
			out.Write(L"CharacterName", CharacterName);
			out.WriteEnum(L"Class", Class);
			out.Write(L"StageLevel", StageLevel);
			out.Write(L"PvpLevel", PvpLevel);
			out.Write(L"IsGuest", IsGuest);
			out.Write(L"IsOnline", IsOnline);
			out.Write(L"PlayTime", PlayTime);
			out.Write(L"JoinedDate", JoinedDate);
			out.Write(L"LastLoggedDate", LastLoggedDate);
		}

		void Family::MemberInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Family::MemberInfo");
			in.Read(L"MemberSerial", MemberSerial);
			in.Read(L"CharacterName", CharacterName);
			in.ReadEnum(L"Class", Class);
			in.Read(L"StageLevel", StageLevel);
			in.Read(L"PvpLevel", PvpLevel);
			in.Read(L"IsGuest", IsGuest);
			in.Read(L"IsOnline", IsOnline);
			in.Read(L"PlayTime", PlayTime);
			in.Read(L"JoinedDate", JoinedDate);
			in.Read(L"LastLoggedDate", LastLoggedDate);
		}


		void Family::RewardCondition::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Family::RewardCondition");
			out.Write(L"NextPlayTimeForPersonalPlay", NextPlayTimeForPersonalPlay);
			out.Write(L"ReceivedDateGroupPlay", ReceivedDateGroupPlay);
			out.Write(L"MemorialDay", MemorialDay);
		}

		void Family::RewardCondition::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Family::RewardCondition");
			in.Read(L"NextPlayTimeForPersonalPlay", NextPlayTimeForPersonalPlay);
			in.Read(L"ReceivedDateGroupPlay", ReceivedDateGroupPlay);
			in.Read(L"MemorialDay", MemorialDay);
		}

		void OpenMarket::ProductInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::OpenMarket::ProductInfo");
			out.Write(L"RegistrationNumber", RegistrationNumber);
			out.WriteEnum(L"RegistrationGrade", Grade);
			out.Write(L"ItemHash", ItemHash);
			out.Write(L"instanceEx", InstanceEx);
			out.Write(L"StackedCount", StackedCount);
			out.Write(L"Seller", Seller);
			out.Write(L"ExpirationDate", ExpirationDate);
			out.Write(L"Price", Price);
			out.Write(L"IsPetProduct", IsPetProduct);
			if (IsPetProduct)
			{
				out.Write(L"Pet", Pet);
			}
		}
		void OpenMarket::ProductInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::OpenMarket::ProductInfo");
			in.Read(L"RegistrationNumber", RegistrationNumber);
			in.ReadEnum(L"RegistrationGrade", Grade);
			in.Read(L"ItemHash", ItemHash);
			in.Read(L"instanceEx", InstanceEx);
			in.Read(L"StackedCount", StackedCount);
			in.Read(L"Seller", Seller);
			in.Read(L"ExpirationDate", ExpirationDate);
			in.Read(L"Price", Price);
			in.Read(L"IsPetProduct", IsPetProduct);
			if (IsPetProduct)
			{
				in.Read(L"Pet", Pet);
			}
		}
		void UnidentifiedItemInfo::ProbabilityTable::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Database::Info::UnidentifiedItemInfo::ProbabilityTable");
			out.Write(L"Id", Id);
			out.Write(L"StackedCount", StackedCount);
			out.Write(L"Probability", Probability);
			out.Write(L"ExpireMin", static_cast<uint32>(ExpireMin));
			out.Write(L"ExpireDue", static_cast<uint32>(ExpireDue));
			out.Write(L"ExpireDate", ExpireDate);
			out.Write(L"CashEnchant1Index", CashEnchant1Index);
			out.Write(L"CashEnchant2Index", CashEnchant2Index);
			out.Write(L"CashEnchant3Index", CashEnchant3Index);
			out.Write(L"CashEnchant4Index", CashEnchant4Index);
			out.Write(L"InstanceEx", InstanceEx.Instance);
			std::wstring temp(L"0");
			if (InstanceEx.ExpireDate == InstanceEx::NoExpiration)
				temp = L"0";
			else
				temp = InstanceEx.ExpireDate.ToString();
			out.Write(L"InstanceEx.ExpireDate", temp);
		}

		void UnidentifiedItemInfo::ProbabilityTable::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Database::Info::UnidentifiedItemInfo::ProbabilityTable");
			in.Read(L"Id", Id);
			in.Read(L"StackedCount", StackedCount, static_cast<uint16>(1));
			in.Read(L"Probability", Probability, float(0));
			in.Read(L"ExpireMin", ExpireMin, static_cast<uint32>(0));
			in.Read(L"ExpireDue", ExpireDue, static_cast<uint32>(0));
			in.Read(L"ExpireDate", ExpireDate, std::wstring());
			in.Read(L"CashEnchant1Index", CashEnchant1Index, static_cast<uint16>(0));
			in.Read(L"CashEnchant2Index", CashEnchant2Index, static_cast<uint16>(0));
			in.Read(L"CashEnchant3Index", CashEnchant3Index, static_cast<uint16>(0));
			in.Read(L"CashEnchant4Index", CashEnchant4Index, static_cast<uint16>(0));
			in.Read(L"InstanceEx", InstanceEx.Instance, int64(0));
			std::wstring temp(L"0");
			in.Read(L"InstanceEx.ExpireDate", temp, std::wstring(L"0"));
			if (temp == L"0")
				InstanceEx.ExpireDate = InstanceEx::NoExpiration;
			else
				InstanceEx.ExpireDate.Parse(temp);

			/* in.Read(L"Instance", Instance, uint64(0));
			in.Read(L"InstanceExpireDate", InstanceExpireDate, std::wstring(L"0")); */
		}

		void UnidentifiedItemInfo::EnchantProbabilityList::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Database::Info::UnidentifiedItemInfo::EnchantProbabilityList");
			out.Write(L"ReinforcementProbabilitys", ReinforcementProbabilitys);
			out.Write(L"LightReinforcementProbabilitys", LightReinforcementProbabilitys);
		}

		void UnidentifiedItemInfo::EnchantProbabilityList::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Database::Info::UnidentifiedItemInfo::EnchantProbabilityList");
			in.Read(L"ReinforcementProbabilitys", ReinforcementProbabilitys, std::vector< float >());
			in.Read(L"LightReinforcementProbabilitys", LightReinforcementProbabilitys, std::vector< float >());
		}


		void UnidentifiedItemInfo::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Database::Info::UnidentifiedItemInfo");
			out.Write(L"Id", Id);
			out.Write(L"AutomaticIdentify", static_cast<int32>(AutomaticIdentify));
			out.Write(L"ProbabilityTablesPerLevelList", probabilityTablesPerLevelList);
			out.Write(L"ClassSpecializedRate", ClassSpecializedRate);
			out.Write(L"MajorStatSpecializedRate", MajorStatSpecializedRate);
			out.Write(L"EnchantProbabilitys", EnchantProbabilitys);
		}

		void UnidentifiedItemInfo::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Database::Info::UnidentifiedItemInfo");
			in.Read(L"Id", Id);
			in.Read(L"AutomaticIdentify", reinterpret_cast<int32&>(AutomaticIdentify), 0);
			in.Read(L"ProbabilityTablesPerLevelList", probabilityTablesPerLevelList);
			in.Read(L"ClassSpecializedRate", ClassSpecializedRate, 0.f);
			in.Read(L"MajorStatSpecializedRate", MajorStatSpecializedRate, 0.f);
			in.Read(L"EnchantProbabilitys", EnchantProbabilitys, EnchantProbabilityList());
		}


		void UnidentifiedItemInfo::Sort()
		{
		}

		void UnidentifiedItemInfo::ProbabilityTableListPerLevel::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"XRated::Database::Info::UnidentifiedItemInfo::ProbabilityTableListPerLevel");
			out.Write(L"Level", Level);
			out.Write(L"ProbabilityTableList", Table);
		}

		void UnidentifiedItemInfo::ProbabilityTableListPerLevel::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"XRated::Database::Info::UnidentifiedItemInfo::ProbabilityTableListPerLevel");
			in.Read(L"Level", Level);
			in.Read(L"ProbabilityTableList", Table);
		}
		const UnidentifiedItemInfo::ProbabilityTableList* UnidentifiedItemInfo::GetProbabilityTable(uint32 level) const
		{
			int i = static_cast<int>(probabilityTablesPerLevelList.size()) - 1;
			for (; i >= 0; --i)
			{
				if (probabilityTablesPerLevelList[i].Level <= level) {
					return &(probabilityTablesPerLevelList[i].Table);
				}
			}
			return NULL;
		}

		UnidentifiedItemInfo::ProbabilityTableList* UnidentifiedItemInfo::GetProbabilityTableForModify(uint32 level)
		{
			int i = static_cast<int>(probabilityTablesPerLevelList.size()) - 1;
			for (; i >= 0; --i)
			{
				if (probabilityTablesPerLevelList[i].Level == level) {
					return &(probabilityTablesPerLevelList[i].Table);
				}
			}
			return NULL;
		}

		const DateTime InstanceEx::NoExpiration(L"2050-12-31 00:00:00");
		const DateTime InstanceEx::Expired(L"2000-01-01 00:00:00");
		bool InstanceEx::IsExpired(const DateTime& now) const
		{
			if (ExpireDate == NoExpiration)
				return false;
			if (ExpireDate.GetDate().GetYear(0) == 0)
				return false;
			if (ExpireDate > now)
				return false;
			return true;
		}
		void InstanceEx::ForceExpiration()
		{
			DateTime newExpireDate(DateTime::Now());
			newExpireDate.GetDate().SetYear(newExpireDate.GetDate().GetYear() - 1);
			ExpireDate = newExpireDate;
		}

		void InstanceEx::MakeUnlimitedPeriod()
		{
			ExpireDate = NoExpiration;
		}
		DateTime InstanceEx::GetExpiredDate() const
		{
			return ExpireDate;
		}
		DateTime InstanceEx::ExtensionExpiredDay(uint32 day)
		{
			if (*this == NoExpiration)
				return DateTime::Infinite;
			this->ExpireDate.Add(DateTime::Unit::Day, day);

			return this->ExpireDate;
		}
		void InstanceEx::Serialize(Serializer::IStreamWriter& out) const
		{
			out.Begin(L"InstanceEx");
			out.Write(L"Instance", Instance);

			ExpireDateFormat tmp;
			tmp.Year = ExpireDate.GetDate().GetYear();
			tmp.Month = ExpireDate.GetDate().GetMonth();
			tmp.Day = ExpireDate.GetDate().GetDay();
			tmp.Hour = ExpireDate.GetTime().GetHour();
			tmp.Minute = ExpireDate.GetTime().GetMinute();

			out.Write(L"ExpireDate", reinterpret_cast<const uint32&>(tmp));

		}
		void InstanceEx::Deserialize(Serializer::IStreamReader& in)
		{
			in.Begin(L"InstanceEx");
			in.Read(L"Instance", Instance);

			ExpireDateFormat tmp;
			in.Read(L"ExpireDate", reinterpret_cast<uint32&>(tmp));
			if (reinterpret_cast<uint32&>(tmp) == 0)
				ExpireDate = DateTime::Infinite;
			else
				ExpireDate = DateTime(tmp.Year, tmp.Month, tmp.Day, tmp.Hour, tmp.Minute, 0);
		}
	}
}
