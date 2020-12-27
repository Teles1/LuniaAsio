#pragma once
#include <Core/Runnable.h>
#include <Core/CriticalSection.h>
#pragma warning( disable : 4121 )
#include "AngelScript/angelscript.h"
#pragma warning( default : 4121 )

#include "Preprocessor/Preprocess.h"
#include <vector>
#include <map>

namespace Lunia { namespace XRated {	namespace Logic {

	class HeadLoader : public Preprocessor::FileSource {
		const std::vector<char>& buff;
	public :
		HeadLoader(const std::vector<char>& source) : buff(source) {}
		bool LoadFile(const std::string& /*filename*/, std::vector<char>& buff) { buff = this->buff; return true; }
	private :
		void operator=(const HeadLoader&);
	};

	class ScriptLoader : public Lunia::Runnable {
	public :
		class IThreadListener {
		public :
			virtual void ScriptLoaded(uint16 uniqueId)=0; 
			virtual void SetEngine(asIScriptEngine* e)=0;
			virtual void SetContext(asIScriptContext* c)=0;
			virtual ILockable& GetSyncRoom()=0;
		};

		struct Ticket {
			enum TicketType { Create=0, Release=1 };
			TicketType type;
			IThreadListener* listener;
			std::string module;
			uint16 uniqueId;
			Ticket(TicketType tp, IThreadListener* l, const std::string& stageName, uint16 id)
				: type(tp), listener(l), module(stageName), uniqueId(id) {}

			Ticket& operator= (const Ticket& t) {
				type = t.type;
				listener = t.listener;
				module = t.module;
				uniqueId = t.uniqueId;
				return *this;
			}
		};
		typedef std::vector<Ticket> JobList;

		class Output : public asIOutputStream {
			Preprocessor::LineNumberTranslator translator;
			std::string stageCode;

			void Parse(const char* text);
			void Write(const char* text);

		public :
			Preprocessor::LineNumberTranslator* GetTranslator() { return &translator; }
			void SetStageCode(const std::string& name) { stageCode = name; }
		};

		struct ASModule {
			bool bUsing;
			IThreadListener* listener;
			asIScriptEngine* engine;
			asIScriptContext* context;
			Output out;

			ASModule() : bUsing(false), listener(0), engine(0), context(0) {}
		private:
			ASModule(const ASModule&); // prevent copy construct
		};

	protected :
		//const static int INITPOOLSIZE = 5;
		//const static int STAGECNT = 5;

		typedef std::vector<ASModule*> ASList;

	protected :

		ASModule* CreateASModule(const std::string& name);
		ASModule* GetASModule(const std::string& name);
		
		void UpdateQueue();
#ifdef _SINGLE
	public:
		static int BindScript(ASModule* module);
#else
		int BindScript(ASModule* module);
#endif

	protected :
		JobList jobs;
		JobList jobsToReserve;

		Lunia::CriticalSection cs;

		std::map < std::string, ASList > ASPool;

	public :
		ScriptLoader();
		virtual ~ScriptLoader();

		void LoadScript(const Ticket& ticket);
		void ReleaseScriptEngine( const Ticket& ticket );

	public : // Helper
		int GetStageCnt() const { return (int)ASPool.size(); }
		int GetPoolSize(const std::string& name) const { return (int)ASPool.find(name)->second.size(); }
		int GetAvailablePool(const std::string& name) const;
	public : // Lunia::Runnable implement
		void Run();

	private:
		ScriptLoader(const ScriptLoader&); // no implementation - copy constructor protection
	};

	inline ScriptLoader& ScriptLoaderInstance()
	{
		static ScriptLoader m_instance_Script_loader;
		return m_instance_Script_loader;
	}

}	}	}



