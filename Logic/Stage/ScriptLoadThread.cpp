#include "ScriptLoadThread.h"
#include "ScriptEngine.h"
#include <Info/Info/Database.h>
#include <Core/Utils/StringUtil.h>

#include <Core/Resource/Resource.h>
#include <Core/DeltaTimer.h>
#include <Core/FileIO/FileIO.h>
#include <Core/Utils/ConfigReader.h>

#include <AngelScript/scriptstdstring/scriptstdstring.h>
#include <AngelScript/scriptarray/scriptarray.h>
using namespace Lunia::XRated::Database;

namespace Lunia { namespace XRated {	namespace Logic {

	void ScriptLoader::Output::Parse(const char* text)
	{
		int pos=0;
		std::string tmp;
		tmp = StringUtil::Tokenize(text, " (", pos);
		tmp = StringUtil::Tokenize(text, " (", pos);
        int lineNum = atoi(tmp.c_str());
		uint32 originalLineNum = translator.ResolveOriginalLine(lineNum);
		Logger::GetInstance().Error( "{0} : original line = {1}", lineNum, originalLineNum);
	}

	void ScriptLoader::Output::Write(const char* text)
	{
		if (strlen(text))
			Parse(text);
		Logger::GetInstance().Info( "[{0}] {1}", stageCode.c_str(), text );
	}

	ScriptLoader::ScriptLoader() : Runnable(L"ScriptLoader")
	{
		if (Config::GetInstance().GetKind() == ServerKind::SquareKind) {
			Database::Info::StageGroup* stageGroup = Lunia::XRated::Database::DatabaseInstance().InfoCollections.StageGroups.Retrieve(53518598); // i dont know why this is static.
			for ( std::vector<Database::Info::StageInfo*>::iterator i = stageGroup->Stages.begin() ; i != stageGroup->Stages.end() ; ++i) {
				ASPool[StringUtil::ToASCII((*i)->Id)];
			}
		} else {
			std::map<std::wstring, int> stagePool;
			Resource::SerializerStreamReader reader = Lunia::Resource::ResourceSystemInstance().CreateDefaultDeserializer(
				StringUtil::ToUnicode(Config::GetInstance().Get<std::string>("PoolInfoPath")).c_str() );
			reader->Read(L"Pools", stagePool);
			std::map<std::wstring, int>::const_iterator i = stagePool.begin();
			while( i != stagePool.end() ) {
				std::string stageCode = StringUtil::ToASCII((*i).first);
				ASList& aslist = ASPool[ stageCode ];

				if (Config::GetInstance().Get<bool>("IgnorePoolInfoPath") == false ) {
					for ( int z=0 ; z < (*i).second ; ++z)
						aslist.push_back( CreateASModule(stageCode) );
				}

				++i;
			}
		}
		this->StartThread(THREAD_PRIORITY_BELOW_NORMAL);
	}

	ScriptLoader::~ScriptLoader()
	{
		{
			AutoLockCS lock(cs);
			jobs.clear();
		}

		StopThread();

		Logger::GetInstance().Warn(L"{0} ASPool releasing", ASPool.size());
		std::map<std::string, ASList>::iterator i = ASPool.begin();
		for ( ; i != ASPool.end() ; ++i) {
			ASList::iterator iter = (*i).second.begin();
			for ( ; iter != (*i).second.end() ; ++iter) {
				(*iter)->engine->Release();
				//(*iter)->context->Release();
				delete (*iter);
			}
		}
		Logger::GetInstance().Warn(L"ASPool releasing completed");
	}


	// Implement a simple message callback function
	void MessageCallback(const asSMessageInfo* msg, void* param)
	{
		const char* type = "ERR ";
		if (msg->type == asMSGTYPE_WARNING)
			type = "WARN";
		else if (msg->type == asMSGTYPE_INFORMATION)
			type = "INFO";
		LoggerInstance().Info("{0} ({1}, {2}) : {3} : {4}\n", msg->section, msg->row, msg->col, type, msg->message);
	}
	ScriptLoader::ASModule* ScriptLoader::CreateASModule(const std::string& name)
	{
		ASModule* asmodule;
		asmodule = new ASModule;
		asmodule->engine = asCreateScriptEngine(ANGELSCRIPT_VERSION);
		if (!asmodule->engine) {
			Logger::GetInstance().Exception(L"unable to create script engine.");
		}
		asmodule->engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);

		asmodule->context = asmodule->engine->CreateContext();
		if (!asmodule->context) {
			Logger::GetInstance().Exception(L"unable to create script context.");
		}
		CScriptBuilder builder;
		int r = builder.StartNewModule(asmodule->engine, name.c_str());
		if (r < 0)
		{
			LoggerInstance().Exception("Unrecoverable error while starting a new module.");
		}

		Info::StageInfo* info = DatabaseInstance().InfoCollections.Stages.Retrieve(StringUtil::To<std::wstring>(name.c_str()).c_str());
		const std::vector<char>& source = info->GetScriptSource();

		asmodule->out.SetStageCode(name);

		//module->engine->SetCommonMessageStream(&module->out);

		//��ó����
		Preprocessor::VectorOutStream VOS;
		Preprocessor::NullOutStream nullStream;
		HeadLoader fs(source);
		Preprocessor::preprocess(name, fs, VOS, nullStream, asmodule->out.GetTranslator());

		/*
		r = module->engine->AddScriptSection(0, name.c_str(), VOS.data(), (int)VOS.size(), 0);
		if ( r < 0 ) {
			Logger::GetInstance().Exception(L"Unable to add scriptsection to engine.");
		}
		*/
		r = builder.AddSectionFromMemory(name.c_str(), VOS.data(), (int)VOS.size(), 0);
		if (r < 0) {
			Logger::GetInstance().Exception(L"Unable to add scriptsection to engine.");
		}
		r = BindScript(asmodule);
		if (r < 0) {
			Logger::GetInstance().Exception(L"Cannot bind script.");
		}
		r = builder.BuildModule();
		if ( r < 0 ) {
			FileIO::File scFile;
			scFile.Open(L"tmpScript.txt", FileIO::File::WriteMode);
			scFile.Write( (unsigned char*)VOS.data(), (int)VOS.size());
			scFile.Close();
			Logger::GetInstance().Exception(L"Script build error.");
		}
		asmodule->module = asmodule->engine->GetModule(name.c_str());
		return asmodule;
	}

	ScriptLoader::ASModule* ScriptLoader::GetASModule(const std::string& name)
	{
		std::map<std::string, ASList>::iterator i = ASPool.find(name);

#ifdef _SINGLE
		if ( i == ASPool.end() ) {
			ASPool[name];
			i = ASPool.find(name);
		}
#else
		if ( i == ASPool.end() ) {
			ASPool[name];
			i = ASPool.find(name);
			Logger::GetInstance().Error(L"Unable to find proper stage pool.[%s]", StringUtil::ToUnicode(name).c_str());
		}
#endif

		ASList::iterator iter = (*i).second.begin();
		for ( ; iter != (*i).second.end() ; ++iter) {//���� �����Ǿ� �ִ� pool ���� �˻��� �Ѵ�.
			if ( !(*iter)->bUsing )
				return (*iter);
		}

		//��� �ִ� pool �� ����. ���� ����������.
		Logger::GetInstance().Warn( L"Not Enough Stage Pool. [{0}/{1}]", StringUtil::ToUnicode(name).c_str(), (int)(*i).second.size());
		//Lunia_IMPORTANT(( L"available memory size : {} MB", Lunia::Debugging::GetLargestFreeMemoryBlockSize()/1024/1024 ));
		ASModule* module = CreateASModule(name);
		(*i).second.push_back( module );
		return module;
	}
	void ScriptLoader::Run()
	{
		Sleep(1);
		UpdateQueue();

		int jobsCount(0);

		Ticket ticket(Ticket::Create, NULL, "", 0);
		{
			AutoLockCS lock(cs);
			if ( jobs.empty() ) return; // nothing to do
			ticket=*jobs.begin(); // why didn't one use 'std::queue' ????
			jobs.erase(jobs.begin());
			jobsCount=jobs.size();
		}

		switch ( ticket.type ) {
		case Ticket::TicketType::Create :
			{
				//���� �ִ� engine instance�� �ִٸ� �ְ� �ƴϸ� ����.
				ASModule* module = GetASModule(ticket.module);
				module->bUsing = true;
				module->listener = ticket.listener;
				{
					AutoLock lock(ticket.listener->GetSyncRoom());
					ticket.listener->SetEngine(module->engine);
					ticket.listener->SetContext(module->context);
					ticket.listener->SetModule(module->module);
				}
				Logger::GetInstance().Warn("a roomCs(0x) got a module(stagename:{0}), {1} jobs left", ticket.module.c_str(), jobsCount);
				ticket.listener->ScriptLoaded(ticket.uniqueId);
			}
			break;
		case Ticket::TicketType::Release :
			{
				std::map<std::string, ASList>::iterator i = ASPool.find( ticket.module );
				if (!ticket.module.empty() && i == ASPool.end())
					Logger::GetInstance().Exception("Some chinese stuff that was one day readable.");

				if (ticket.module.empty())
					break;

				//stage�� pool�� ���鼭 ��ġ�ϴ� ���� ã�´�.
				ASList::iterator iter = (*i).second.begin();
				for ( ; iter != (*i).second.end() ; ++iter ) {
					if ( (*iter)->listener == ticket.listener ) {//�̳��̴�. Ǯ������.
						Logger::GetInstance().Warn( "a roomCs(0x) released a module");
						(*iter)->listener = NULL;
						(*iter)->bUsing = false;
#ifdef _SINGLE
						//Single������ pool�� ���� ���� �� ����������.
						//Reload�� �ɰ��̱� ������
						delete (*iter);
						(*i).second.erase( iter );
#endif
						break;
					}
				}
				if (iter==i->second.end()) // unable to find the listener in the pool - should not be happened
				{
					Logger::GetInstance().Error("a room(stagename:{0}) could not be found in pool", ticket.module);
				}
			}
			break;
		}
	}

	void ScriptLoader::UpdateQueue()
	{
		//�۾� queue������ ����� �۾��� ����Ѵ�.
		//�۾��� ��û�ϴ� �Ͱ� �ߺ����� �ʰ� �Ѵ�.
		AutoLockCS locker(cs);

		for (JobList::iterator i = jobsToReserve.begin() ; i != jobsToReserve.end() ; ++i)
			jobs.push_back( (*i) );

		jobsToReserve.clear();
	}

	template<typename T>
	void ConstructArray(std::vector<T> *self)
	{
		new(self) std::vector<T>();
	}

	template<typename T>
	void ConstructArray(int length, std::vector<T> *self)
	{
		new(self) std::vector<T>(length);
	}

	//void DoNothing(void*)
	//{
	//}

	void DestructIntArray(std::vector<int> *self)
	{
		self->~vector();
	}

	void DestructUInt32Array(std::vector<uint32> *self)
	{
		self->~vector();
	}

	void DestructFloatArray(std::vector<float> *self)
	{
		self->~vector();
	}


	int ScriptLoader::BindScript(ASModule* module)
	{
		int r;
		//Register std::string class
		RegisterStdString(module->engine);
		RegisterScriptArray(module->engine, true);
		//Register stage interface(IStageScript) class
		//r = module->engine->RegisterObjectType("IStageScript", sizeof(IStageScript), asOBJ_VALUE | asGetTypeTraits<IStageScript>()); if (r < 0) return r;

		//Register ScriptEngine class and behaviour
		r = module->engine->RegisterObjectType("ScriptEngine", sizeof(ScriptEngine), asOBJ_REF); if (r < 0) return r;
		r = module->engine->RegisterObjectBehaviour("ScriptEngine", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptEngine, AddRef), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectBehaviour("ScriptEngine", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptEngine, Release), asCALL_THISCALL); if (r < 0) return r;
		/* I dont get why they added a count for this if they not gonna use it?! */

		/* arrays should resgister as custom way to validate index range or so. */
		
		/* register int arrays */
		//r = module->engine->RegisterObjectType("int[]", sizeof(std::vector<int>), asOBJ_VALUE | asOBJ_APP_CLASS_CDA);
		//if (r < 0) return r;
		//r = module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(ConstructArray<int>, (std::vector<int> *), void), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTIONPR(ConstructArray<int>, (int, std::vector<int> *), void), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructIntArray), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_FACTORY, "int &f(int[]&in)", asMETHODPR(std::vector<int>, operator=, (const std::vector<int> &), std::vector<int>&), asCALL_THISCALL);
		//r = module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_INDEX, "int &f(int)", asMETHODPR(std::vector<int>, at, (std::size_t), int&), asCALL_THISCALL);
		//r = module->engine->RegisterObjectMethod("int[]", "int length()", asMETHOD(std::vector<int>, size), asCALL_THISCALL);
		//r=module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_ADDREF, "void f()", asFUNCTION(DoNothing), asCALL_CDECL_OBJFIRST); if ( r < 0) return r;
		//r=module->engine->RegisterObjectBehaviour("int[]", asBEHAVE_RELEASE, "void f()", asFUNCTION(DoNothing), asCALL_CDECL_OBJFIRST); if ( r < 0) return r;

		/* register uint32 arrays */
		//r = module->engine->RegisterObjectType("uint32[]", sizeof(std::vector<uint32>), asOBJ_APP_CLASS_CDA); if (r < 0) return r;
		//r = module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(ConstructArray<uint32>, (std::vector<uint32> *), void), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTIONPR(ConstructArray<uint32>, (int, std::vector<uint32> *), void), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructUInt32Array), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_ASSIGNMENT, "uint32[] &f(uint32[]&in)", asMETHODPR(std::vector<uint32>, operator=, (const std::vector<uint32> &), std::vector<uint32>&), asCALL_THISCALL);
		//r = module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_INDEX, "uint32 &f(int)", asMETHODPR(std::vector<uint32>, at, (std::size_t), uint32&), asCALL_THISCALL);
		//r = module->engine->RegisterObjectMethod("uint32[]", "int length()", asMETHOD(std::vector<uint32>, size), asCALL_THISCALL);
		//r=module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_ADDREF, "void f()", asFUNCTION(DoNothing), asCALL_CDECL_OBJFIRST); if ( r < 0) return r;
		//r=module->engine->RegisterObjectBehaviour("uint32[]", asBEHAVE_RELEASE, "void f()", asFUNCTION(DoNothing), asCALL_CDECL_OBJFIRST); if ( r < 0) return r;

		/* register float arrays */
		//r = module->engine->RegisterObjectType("float[]", sizeof(std::vector<float>), asOBJ_APP_CLASS_CDA); if (r < 0) return r;
		//r = module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_CONSTRUCT, "void f()", asFUNCTIONPR(ConstructArray<float>, (std::vector<float> *), void), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_CONSTRUCT, "void f(int)", asFUNCTIONPR(ConstructArray<float>, (int, std::vector<float> *), void), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_DESTRUCT, "void f()", asFUNCTION(DestructFloatArray), asCALL_CDECL_OBJLAST);
		//r = module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_ASSIGNMENT, "float[] &f(float[]&in)", asMETHODPR(std::vector<float>, operator=, (const std::vector<float> &), std::vector<float>&), asCALL_THISCALL);
		//r = module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_INDEX, "float &f(int)", asMETHODPR(std::vector<float>, at, (std::size_t), float&), asCALL_THISCALL);
		//r = module->engine->RegisterObjectMethod("float[]", "int length()", asMETHOD(std::vector<float>, size), asCALL_THISCALL);
		//r=module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_ADDREF, "void f()", asFUNCTION(DoNothing), asCALL_CDECL_OBJFIRST); if ( r < 0) return r;
		//r=module->engine->RegisterObjectBehaviour("float[]", asBEHAVE_RELEASE, "void f()", asFUNCTION(DoNothing), asCALL_CDECL_OBJFIRST); if ( r < 0) return r;


		//Register member functions

		//Helper
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool IsPlayer(uint32)", asMETHOD(ScriptEngine, IsPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetPlayerCnt()", asMETHOD(ScriptEngine, GetPlayerCnt), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetActivePlayerCnt()", asMETHOD(ScriptEngine, GetActivePlayerCnt), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint8 GetPlayerTeam(uint32)", asMETHOD(ScriptEngine, GetPlayerTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetPlayerClass(uint32)", asMETHOD(ScriptEngine, GetPlayerClass), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetRandomInt(int, int)", asMETHOD(ScriptEngine, GetRandomInt), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GetNPCPosition(uint32, float&out, float&out)", asMETHOD(ScriptEngine, GetNPCPosition), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetPlayerLv(uint32)", asMETHOD(ScriptEngine, GetPlayerLv), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GetServerTime(int&out, int&out, int&out, int&out)", asMETHOD(ScriptEngine, GetServerTime), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ShowScriptMsg(const string&in)", asMETHOD(ScriptEngine, ShowScriptMsg), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetLocaleCode()", asMETHOD(ScriptEngine, GetLocaleCode), asCALL_THISCALL); if (r < 0) return r;

		// Addby Caspian 20071115
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint16 GetPlayerCntByTeam(uint8)", asMETHOD(ScriptEngine, GetPlayerCntByTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetPlayerSerial(uint8, uint16)", asMETHOD(ScriptEngine, GetPlayerSerial), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint16 GetPlayablePlayerCnt()", asMETHOD(ScriptEngine, GetPlayablePlayerCnt), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint16 GetPlayablePlayerCntByTeam(uint8)", asMETHOD(ScriptEngine, GetPlayablePlayerCntByTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool IsPlayable(uint32)", asMETHOD(ScriptEngine, IsPlayable), asCALL_THISCALL); if (r < 0) return r;

		// AddBy Caspian 20071120
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void SetPvpMissionClearDelay(float)", asMETHOD(ScriptEngine, SetPvpMissionClearDelay), asCALL_THISCALL); if (r < 0) return r;

		////Functions
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void SetPvpItemInfo(uint32)", asMETHOD(ScriptEngine, SetPvpItemInfo), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DropPvpItem()", asMETHOD(ScriptEngine, DropPvpItem), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddInterestLocation(int, bool)", asMETHOD(ScriptEngine, AddInterestLocation), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool IsFulfilledInterestLocation(int)", asMETHOD(ScriptEngine, IsFulfilledInterestLocation), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddInterestObjectLocation(uint32, float, bool)", asMETHOD(ScriptEngine, AddInterestObjectLocation), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddInterestObjects(uint32, bool)", asMETHOD(ScriptEngine, AddInterestObjects), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddInterestNPCPath(uint32, bool)", asMETHOD(ScriptEngine, AddInterestNPCPath), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddSpecifiedObjects(uint32)", asMETHOD(ScriptEngine, AddSpecifiedObjects), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void Pause(float)", asMETHOD(ScriptEngine, Pause), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void LocPlayers(bool)", asMETHOD(ScriptEngine, LocPlayers), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void TimerRun(int, float)", asMETHOD(ScriptEngine, TimerRun), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void TimerPause(int)", asMETHOD(ScriptEngine, TimerPause), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void MissionClear(bool, uint8)", asMETHOD(ScriptEngine, MissionClear), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ActivateFastMatch()", asMETHOD(ScriptEngine, ActivateFastMatch), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void SecretFound()", asMETHOD(ScriptEngine, SecretFound), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ClearItem()", asMETHOD(ScriptEngine, ClearItem), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CreateItemInLocation(uint32, int, int)", asMETHOD(ScriptEngine, CreateItemInLocation), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CreateItemInPosition(uint32, float, float)", asMETHOD(ScriptEngine, CreateItemInPosition), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void createItemInLocationById(uint32, uint32, int, int)", asMETHOD(ScriptEngine, createItemInLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CreateItemInPositionById(uint32, uint32, float, float)", asMETHOD(ScriptEngine, CreateItemInPositionById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CreatePrivateItemInPosition(uint32, float, float)", asMETHOD(ScriptEngine, CreatePrivateItemInPosition), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool RemoveItemFromPlayer(uint32, uint32, int)", asMETHOD(ScriptEngine, RemoveItemFromPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool RemoveItemsFromPlayer(uint32, int[] &inout, int[] &inout)", asMETHOD(ScriptEngine, RemoveItemsFromPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool RemoveItemFromPlayerByTeam(uint32, uint32, int)", asMETHOD(ScriptEngine, RemoveItemFromPlayerByTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool AddItemToPlayer(uint32, uint32, int)", asMETHOD(ScriptEngine, AddItemToPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool AddItemsToPlayer(uint32, int[] &inout, int[] &inout)", asMETHOD(ScriptEngine, AddItemsToPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool AddItemToPlayerByTeam(uint32, uint32, int)", asMETHOD(ScriptEngine, AddItemToPlayerByTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool ExamineItemSlotFromPlayer(uint32, uint32, int)", asMETHOD(ScriptEngine, ExamineItemSlotFromPlayer), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint32 CreateObjectByPosition(uint32, float, float, uint8)", asMETHOD(ScriptEngine, CreateObjectByPosition), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CreateObjectsByPosition(uint32, float[] &inout, float[] &inout, uint8)", asMETHOD(ScriptEngine, CreateObjectsByPosition), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint32 CreateObjectByPositionD(uint32, float, float, float, float, uint8)", asMETHOD(ScriptEngine, CreateObjectByPositionD), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint32 CreateNPCByPos(uint32, float, float, uint8, int, int, int, int)", asMETHOD(ScriptEngine, CreateNPCByPos), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CreateNPCsByPos(uint32, float[] &inout, float[] &inout, uint8, int, int, int, int)", asMETHOD(ScriptEngine, CreateNPCsByPos), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint32 CreateNPCByPosD(uint32, float, float, float, float, uint8, int, int, int, int)", asMETHOD(ScriptEngine, CreateNPCByPosD), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool ChangePlayerStatusLimit(uint32[] &inout, int[] &inout)", asMETHOD(ScriptEngine, ChangePlayerStatusLimit), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool ChangePlayerStatusRate(uint32[] &inout, float[] &inout)", asMETHOD(ScriptEngine, ChangePlayerStatusRate), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DestroyObject(uint32, bool)", asMETHOD(ScriptEngine, DestroyObject), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DestroyObjectFromLocationById(uint32, int, bool)", asMETHOD(ScriptEngine, DestroyObjectFromLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DestroyObjectFromLocationByTeam(uint8, int, bool)", asMETHOD(ScriptEngine, DestroyObjectFromLocationByTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DestroyNPCFromLocationById(uint32, int, bool)", asMETHOD(ScriptEngine, DestroyNPCFromLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DestroyNPCFromLocationByTeam(uint8, int, bool)", asMETHOD(ScriptEngine, DestroyNPCFromLocationByTeam), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangePlayerTeam(uint32, uint8)", asMETHOD(ScriptEngine, ChangePlayerTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCTeam(uint32, uint8)", asMETHOD(ScriptEngine, ChangeNPCTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCAI(int, uint32, int, int)", asMETHOD(ScriptEngine, ChangeNPCAI), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCAIBySerial(uint32, int, int)", asMETHOD(ScriptEngine, ChangeNPCAIBySerial), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void MoveObjectFromLocation(int, int, uint16)", asMETHOD(ScriptEngine, MoveObjectFromLocation), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void MoveObjectFromLocationById(int, int, uint32, uint16)", asMETHOD(ScriptEngine, MoveObjectFromLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void MovePlayer(uint32, int)", asMETHOD(ScriptEngine, MovePlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void MoveAllPlayer(int)", asMETHOD(ScriptEngine, MoveAllPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveExp(uint32, uint16, uint32)", asMETHOD(ScriptEngine, GiveExp), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveExpToAllPlayer(uint16, uint32)", asMETHOD(ScriptEngine, GiveExpToAllPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveBattleEXPtoAllPlayer(uint8, uint32)", asMETHOD(ScriptEngine, GiveBattleEXPtoAllPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddHp(uint32, int)", asMETHOD(ScriptEngine, AddHp), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddMp(uint32, int)", asMETHOD(ScriptEngine, AddMp), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DealDamageFromLocationByTeam(int, uint8, float, bool)", asMETHOD(ScriptEngine, DealDamageFromLocationByTeam), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DealDamageFromLocationById(int, uint32, float, bool)", asMETHOD(ScriptEngine, DealDamageFromLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCSight(uint32, float)", asMETHOD(ScriptEngine, ChangeNPCSight), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCAction(uint32, uint32)", asMETHOD(ScriptEngine, ChangeNPCAction), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCActionInLocationById(int, uint32, uint32)", asMETHOD(ScriptEngine, ChangeNPCActionInLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeMISCStateById(uint32, uint32, bool)", asMETHOD(ScriptEngine, ChangeMISCStateById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeMISCStateInLocationById(uint32, int, uint32, bool)", asMETHOD(ScriptEngine, ChangeMISCStateInLocationById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCStateById(uint32, uint32, bool)", asMETHOD(ScriptEngine, ChangeNPCStateById), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeNPCStateInLocationById(uint32, int, uint32, bool)", asMETHOD(ScriptEngine, ChangeNPCStateInLocationById), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint32 CheckMinimumPlayerCount()", asMETHOD(ScriptEngine, CheckMinimumPlayerCount), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void CheckStateBundleForAchievement(uint32)", asMETHOD(ScriptEngine, CheckStateBundleForAchievement), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveStateBundleToObject(uint32, uint32)", asMETHOD(ScriptEngine, GiveStateBundleToObject), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint8 GetQuestState(uint32 , uint32)", asMETHOD(ScriptEngine, GetQuestState), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint8 SetQuestState(uint32 , uint32, uint8)", asMETHOD(ScriptEngine, SetQuestState), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint8 GetQuestParameter(uint32 , uint32, uint8)", asMETHOD(ScriptEngine, GetQuestParameter), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint8 SetQuestParameter(uint32 , uint32, uint8, uint32)", asMETHOD(ScriptEngine, SetQuestParameter), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DisplayTextEvent(uint8, uint16, float)", asMETHOD(ScriptEngine, DisplayTextEvent), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void NoticeTextEvent(uint32, uint8, uint16, float)", asMETHOD(ScriptEngine, NoticeTextEvent), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DisplayText(uint8, const string&in)", asMETHOD(ScriptEngine, DisplayText), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void NoticeText(uint32, uint8, const string&in)", asMETHOD(ScriptEngine, NoticeText), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void DisplayTimer(int, uint32)", asMETHOD(ScriptEngine, DisplayTimer), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "void BroadcastEvent(int)", asMETHOD(ScriptEngine, BroadcastEvent), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void NoticeEvent(uint32, int)", asMETHOD(ScriptEngine, NoticeEvent), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void NoticeClientStageEvent(uint32, int, int)", asMETHOD(ScriptEngine, NoticeClientStageEvent), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void AddMinimapPing(int, int, int, int)", asMETHOD(ScriptEngine, AddMinimapPing), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void NoticeAddMinimapPing(int, int, int, int, uint32)", asMETHOD(ScriptEngine, NoticeAddMinimapPing), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void RemoveMinimapPing(int)", asMETHOD(ScriptEngine, RemoveMinimapPing), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveLicense(uint32, uint32)", asMETHOD(ScriptEngine, GiveLicense), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveLicenseToPlayer(uint32, uint32, uint32)", asMETHOD(ScriptEngine, GiveLicenseToPlayer), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GiveCharacterLicense(uint32, uint32)", asMETHOD(ScriptEngine, GiveCharacterLicense), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "void NoticeStylePointState(uint32)", asMETHOD(ScriptEngine, NoticeStylePointState), asCALL_THISCALL); if (r < 0) return r;

		r = module->engine->RegisterObjectMethod("ScriptEngine", "void SetFloor(int)", asMETHOD(ScriptEngine, SetFloor), asCALL_THISCALL); if (r < 0) return r;

		/* Gamble */
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GambleGameEnd(int[] &inout)", asMETHOD(ScriptEngine, GambleGameEnd), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void GambleNextGame(uint32, float)", asMETHOD(ScriptEngine, GambleNextGame), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "int GetSlimeRaceIntervalInMin()", asMETHOD(ScriptEngine, GetSlimeRaceIntervalInMin), asCALL_THISCALL); if (r < 0) return r;

		/* Holiday Event */
		r = module->engine->RegisterObjectMethod("ScriptEngine", "bool IsHolidayEventTime(uint32)", asMETHOD(ScriptEngine, IsHolidayEventTime), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "uint32 GetHolidayEventNpcHash(uint32, uint32)", asMETHOD(ScriptEngine, GetHolidayEventNpcHash), asCALL_THISCALL); if (r < 0) return r;

		// Weather
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeWeatherToNone()", asMETHOD(ScriptEngine, ChangeWeatherToClear), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeWeatherToClear()", asMETHOD(ScriptEngine, ChangeWeatherToClear), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeWeatherToRain()", asMETHOD(ScriptEngine, ChangeWeatherToRain), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeWeatherToSnow()", asMETHOD(ScriptEngine, ChangeWeatherToSnow), asCALL_THISCALL); if (r < 0) return r;
		r = module->engine->RegisterObjectMethod("ScriptEngine", "void ChangeWeatherToAqua()", asMETHOD(ScriptEngine, ChangeWeatherToAqua), asCALL_THISCALL); if (r < 0) return r;
		return 0;
	}

	void ScriptLoader::LoadScript(const Ticket& ticket)
	{
		AutoLockCS locker(cs);
		jobsToReserve.push_back( ticket );
	}

	void ScriptLoader::ReleaseScriptEngine( const Ticket& ticket )
	{
		AutoLockCS locker(cs);
		jobsToReserve.push_back( ticket );
	}
    
	int ScriptLoader::GetAvailablePool(const std::string& name) const 
	{
		int returnValue = 0;
		const std::map<std::string, ASList>::const_iterator iter = ASPool.find(name);
		for ( ASList::const_iterator i = (*iter).second.begin() ; i != (*iter).second.end() ; ++i) {
			if (!(*i)->bUsing) ++returnValue;
		}
		return returnValue;
	}

}	}	}
