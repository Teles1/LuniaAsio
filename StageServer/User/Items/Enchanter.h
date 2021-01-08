#pragma once
#include <Info/Info/Enchant/Enchant.h>
#include <Core/Serializer/Serializer.h>
#include <Core/Utils/Math/Math.h>

namespace Lunia { namespace XRated { namespace StageServer{

	class EnchantException : public Exception
	{
	public:
		EnchantException(const wchar_t* message) : Exception(message) {}
	};

	class IdentifyEnchanter 
	{
	public:
		IdentifyEnchanter();
		Database::Enchant::EnchantBitfields operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source/*, int identifyIndex*/) const; 

	private:
		Math::RandomByPortion status;
		Math::RandomByPortion levels;
	};

	class AdvancedIdentifyEnchanter 
	{
	public:
		AdvancedIdentifyEnchanter();
		Database::Enchant::EnchantBitfields operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source , const std::vector< uint32 >& statusTypeInfo/*, int identifyIndex*/) const; 
		Database::Enchant::EnchantBitfields operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source/*, int identifyIndex*/) const; 

	private:
		Math::RandomByPortion status;
		Math::RandomByPortion levels;
	};

	class CashIdentifyEnchanter
	{
	public:
		CashIdentifyEnchanter();
		Database::Enchant::EnchantBitfields operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source/*, int identifyIndex*/) const; 

	private:
		Math::RandomByPortion status;
		Math::RandomByPortion levels;
	};
	
	struct EnchantResult
	{
		enum States {
			Compleate,			//< enchant�� �ϴµ� ���簡 ������.
			Error				//< enchant�� �ϴµ� ���簡 �־���(���ͼ��� �ȵǴ� ���� ������ ����̴�).

		};
		States								State;
		Database::Enchant::EnchantBitfields	Instance;
		EnchantResult(const Database::Enchant::EnchantBitfields instance,States state) : Instance(instance),State(state) {}
	};
	
	
	class ReinforceEnchanter
	{
	public:
		EnchantResult operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source, uint32 catalystHash, uint16 numberOfCatalysts) const;
	};


	class ExtractEnchanter
	{
	public:
		std::pair<uint32/*ItemHash*/, uint16/*count*/> operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source) const;
	};


	
	class LightReinforceEnchanter
	{
	public:
		EnchantResult operator()(const Database::Info::ItemInfo& info, Database::Enchant::EnchantBitfields source, uint32 catalystHash, uint16 numberOfCatalysts) const;
	};

} } }