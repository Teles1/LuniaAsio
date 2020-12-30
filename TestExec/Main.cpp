//#include <AngelScript/angelscript.h>
//#include <AngelScript/scriptstdstring/scriptstdstring.h>
//#include <AngelScript/scriptbuilder/scriptbuilder.h>
//#include <cassert>
//#include <string>
//#include <iostream>
//#include <Core/Utils/ConfigReader.h>
//
//void MessageCallback(const asSMessageInfo* msg) {
//	
//}
//void print(const std::string& in){
//	std::cout << in << std::endl;
//}
//bool teste() {
//
//	auto* engine = asCreateScriptEngine();
//	{
//		int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
//		RegisterStdString(engine);
//		r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL);
//		assert(r >= 0);
//	}
//	{
//		CScriptBuilder builder;
//		int r = builder.StartNewModule(engine, "MyModule");
//		if (r < 0)
//		{
//			// If the code fails here it is usually because there
//			// is no more memory to allocate the module
//			printf("Unrecoverable error while starting a new module.\n");
//			return 0;
//		}
//		r = builder.AddSectionFromFile("Test.as");
//		if (r < 0)
//		{
//			// The builder wasn't able to load the file. Maybe the file
//			// has been removed, or the wrong name was given, or some
//			// preprocessing commands are incorrectly written.
//			printf("Please correct the errors in the script and try again.\n");
//			return 0;
//		}
//		r = builder.BuildModule();
//		if (r < 0)
//		{
//			// An error occurred. Instruct the script writer to fix the 
//			// compilation errors that were listed in the output stream.
//			printf("ModuleBuild Please correct the errors in the script and try again.\n");
//			return 0;
//		}
//
//	}
//	{
//		asIScriptModule* mod = engine->GetModule("MyModule");
//		// Create our context, prepare it, and then execute
//		asIScriptContext* ctx = engine->CreateContext();
//		//Reading a random function from withing the script
//		{
//			auto* function = mod->GetFunctionByName("SetInstance");
//			ctx->Prepare(function);
//			int result = ctx->Execute();
//			if (result == asEXECUTION_FINISHED)
//			{
//				// The return value is only valid if the execution finished successfully
//				asDWORD ret = ctx->GetReturnDWord();
//			}
//		}
//		{
//			asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
//			if (func == 0)
//			{
//				// The function couldn't be found. Instruct the script writer
//				// to include the expected function in the script.
//				printf("The script must have the function 'void main()'. Please add it and try again.\n");
//				return 0;
//			}
//			ctx->Prepare(func);
//			int r = ctx->Execute();
//			if (r != asEXECUTION_FINISHED)
//			{
//				// The execution didn't complete as expected. Determine what happened.
//				if (r == asEXECUTION_EXCEPTION)
//				{
//					// An exception occurred, let the script writer know what happened so it can be corrected.
//					printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
//				}
//			}
//		}
//		ctx->Release();
//	}
//}

#include <Info/Info.h>
#include <Core/Utils/ConfigReader.h>
using namespace Lunia;
int main(int argc, char* argv[]) {
	Lunia::Config::GetInstance("Config_Stage.json");
	Lunia::Resource::ResourceSystemInstance().AddPath(L"..\\x64\\Debug");
	XRated::Database::DatabaseInstance().Init();		

	auto TargetItem = XRated::Database::Info::ItemInfo();
	TargetItem.Id = L"TW_091209_CashEirHand_C078_b";
	TargetItem.Hash = StringUtil::Hash(TargetItem.Id.c_str());
	TargetItem.Limits.Class = (XRated::Constants::ClassType)14;
	TargetItem.EquipParts = (XRated::Constants::EquipParts)16;

	auto PortionItem = XRated::Database::Info::ItemInfo();
	PortionItem.Id = L"ArutaItem_60SKILL_001_001";
	PortionItem.Hash = StringUtil::Hash(PortionItem.Id.c_str());

	auto start = std::chrono::high_resolution_clock::now();
	//XRated::Database::DatabaseInstance().InfoCollections.UpgradeTables.Retrieve(&PortionItem, &TargetItem);					
	auto teste = XRated::Database::DatabaseInstance().InfoCollections.UpgradeTables.RetrieveNewUpgrade(&PortionItem, &TargetItem);					
	
	auto finish = std::chrono::high_resolution_clock::now();
	auto microseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finish - start);
	Logger::GetInstance().Info("Took {0}ms", microseconds.count());


	//XRated::Database::DatabaseInstance().InfoCollections.Stages.SaveXml();
	//XRated::Database::DatabaseInstance().InfoCollections.Skills.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.EnchantTables.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.Items.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.EquipSet.Init(false);
	//XRated::Database::DatabaseInstance().InfoCollections.Pets.Init(false);
	return 0;
}
