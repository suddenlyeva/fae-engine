#include"ScriptEngine.hpp"
#include<string>
#include<iostream>
#include<fstream>
#include<sstream>
#include<vector>
#include <chrono>
#include <thread>

//----------------------------------------------------------------
// function for sample script
gstd::value func_print(gstd::script_machine* machine, int argc, gstd::value const * argv)
{
	std::wstring wstr = argv[0].as_string();
	std::string str = gstd::to_mbcs(wstr);
	std::cout << str;
	return gstd::value();
}

gstd::value func_println(gstd::script_machine* machine, int argc, gstd::value const * argv)
{
	std::wstring wstr = argv[0].as_string();
	std::string str = gstd::to_mbcs(wstr);
	std::cout << str << std::endl;
	return gstd::value();
}

gstd::value func_clear(gstd::script_machine* machine, int argc, gstd::value const * argv)
{
	std::system("CLS");
	return gstd::value();
}

gstd::value func_min(gstd::script_machine* machine, int argc, gstd::value const * argv)
{
	long double v1 = argv[0].as_real();
	long double v2 = argv[1].as_real();
	long double res = v1 <= v2 ? v1 : v2;
	return gstd::value(machine->get_engine()->get_real_type(), res);
}

gstd::value func_max(gstd::script_machine* machine, int argc, gstd::value const * argv)
{
	long double v1 = argv[0].as_real();
	long double v2 = argv[1].as_real();
	long double res = v1 >= v2 ? v1 : v2;
	return gstd::value(machine->get_engine()->get_real_type(), res);
}

gstd::value func_to_string(gstd::script_machine* machine, int argc, gstd::value const * argv)
{
	std::wstring res = argv[0].as_string();
	return gstd::value(machine->get_engine()->get_string_type(), res);
}

gstd::function const sampleScriptFunction[] =  
{
	{"clear", func_clear, 0 },
	{"print", func_print, 1},
	{"println", func_println, 1 },
	{"min", func_min, 2},
	{"max", func_max, 2},
	{"toString", func_to_string, 1},
};
//----------------------------------------------------------------

//----------------------------------------------------------------
// main
void RunSample(char* scriptName);
int main(int argc, char *argv[])
{

	if (argc != 2) {
		std::cerr << "Invalid Arguments" << std::endl;
		return 1;
	}

	try
	{
		RunSample(argv[1]);
	}
	catch(std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}

void RunSample(char* scriptName)
{
	//--------------------------------
	//sample script source

	std::ifstream ifile(scriptName);
	std::string source((std::istreambuf_iterator<char>(ifile)), std::istreambuf_iterator<char>());

	//--------------------------------
	//error handle
	struct ErrorHandle
	{
		static void CheckEngineError(gstd::script_engine& engine)
		{
			if(engine.get_error())
			{
				int line = engine.get_error_line();
				std::ostringstream os;
				os << std::dec << line;
				
				std::string error = "engine error:line=" + os.str() + " " + engine.get_error_message();
				throw std::exception(error.c_str());
			}
		}

		static void CheckMachineError(gstd::script_machine& machine)
		{
			if(machine.get_error())
			{
				int line = machine.get_error_line();
				std::ostringstream os;
				os << std::dec << line;
				
				std::string error = "machine error:line=" + os.str() + " " + machine.get_error_message();
				throw std::exception(error.c_str());
			}
		}
	};

	//--------------------------------
	//script function
	std::vector<gstd::function> func;
	int funcCount = sizeof(sampleScriptFunction)/sizeof(gstd::function);
	func.resize(funcCount);
	memcpy(&func[0], &sampleScriptFunction[0], sizeof(gstd::function) * funcCount);

	//--------------------------------
	//create script engine
	gstd::script_type_manager typeManager;
	gstd::script_engine engine(&typeManager, source.c_str(), func.size(), &func[0]);
	ErrorHandle::CheckEngineError(engine);

	//--------------------------------
	//create script machine
	gstd::script_machine machine(&engine);
	machine.run();
	ErrorHandle::CheckMachineError(machine);

	//--------------------------------
	//call @Setup

	//--------------------------------
	//call @Console
	if (machine.has_event("Console")) {
		std::string input;
		machine.call("Console");
		ErrorHandle::CheckMachineError(machine);
		if (machine.has_event("Setup") && !machine.get_stopped()) {
			machine.call("Setup");
			ErrorHandle::CheckMachineError(machine);
		}
		while (!machine.get_stopped() && std::getline(std::cin, input)) {
			machine.call("Console");
			ErrorHandle::CheckMachineError(machine);
		}
	}

	//--------------------------------
	//call @Ticker
	if (machine.has_event("Ticker")) {
		machine.call("Ticker");
		if (machine.has_event("Setup") && !machine.get_stopped()) {
			machine.call("Setup");
			ErrorHandle::CheckMachineError(machine);
		}
		while(!machine.get_stopped()) {
				machine.call("Ticker");
				ErrorHandle::CheckMachineError(machine);
			std::this_thread::sleep_for(std::chrono::milliseconds(1000/60));
		}
	}
}