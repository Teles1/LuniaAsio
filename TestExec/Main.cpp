#pragma once

#include <AngelScript/angelscript.h>
#include <AngelScript/scriptstdstring/scriptstdstring.h>
#include <AngelScript/scriptbuilder/scriptbuilder.h>
#include <cassert>
#include <string>
#include <iostream>
void MessageCallback(const asSMessageInfo* msg) {
	
}
void print(const std::string& in){
	std::cout << in << std::endl;
}
int main(int argc, char* argv[]) {
	auto* engine = asCreateScriptEngine();
	{
		int r = engine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
		RegisterStdString(engine);
		r = engine->RegisterGlobalFunction("void print(const string &in)", asFUNCTION(print), asCALL_CDECL); 
		assert(r >= 0);
	}


	{
		CScriptBuilder builder;
		int r = builder.StartNewModule(engine, "MyModule");
		if (r < 0)
		{
			// If the code fails here it is usually because there
			// is no more memory to allocate the module
			printf("Unrecoverable error while starting a new module.\n");
			return 0;
		}
		r = builder.AddSectionFromFile("Test.as");
		if (r < 0)
		{
			// The builder wasn't able to load the file. Maybe the file
			// has been removed, or the wrong name was given, or some
			// preprocessing commands are incorrectly written.
			printf("Please correct the errors in the script and try again.\n");
			return 0;
		}
		r = builder.BuildModule();
		if (r < 0)
		{
			// An error occurred. Instruct the script writer to fix the 
			// compilation errors that were listed in the output stream.
			printf("Please correct the errors in the script and try again.\n");
			return 0;
		}
		
	}
	{
		asIScriptModule* mod = engine->GetModule("MyModule");
		asIScriptFunction* func = mod->GetFunctionByDecl("void main()");
		if (func == 0)
		{
			// The function couldn't be found. Instruct the script writer
			// to include the expected function in the script.
			printf("The script must have the function 'void main()'. Please add it and try again.\n");
			return 0;
		}
		// Create our context, prepare it, and then execute
		asIScriptContext* ctx = engine->CreateContext();
		ctx->Prepare(func);
		int r = ctx->Execute();
		if (r != asEXECUTION_FINISHED)
		{
			// The execution didn't complete as expected. Determine what happened.
			if (r == asEXECUTION_EXCEPTION)
			{
				// An exception occurred, let the script writer know what happened so it can be corrected.
				printf("An exception '%s' occurred. Please correct the code and try again.\n", ctx->GetExceptionString());
			}
		}
	}
	return 0;
}