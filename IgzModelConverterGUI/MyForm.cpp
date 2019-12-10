#include "MyForm.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Runtime::InteropServices;
using namespace System::Collections;
using namespace System::Collections::Generic;
using namespace System::IO;

[STAThreadAttribute]
int Main(cli::array<String^>^ args) {
	if (args->Length > 2) //Command Line for ARD :-)
	{
		int game;
		int type;
		if (args[1] == "nst")
		{
			game = 1;
		}
		else if (args[1] == "ctr")
		{
			game = 2;
		}
		else if (args[1] == "skyl")
		{
			game = 3;
		}
		if (args[2] == "act")
		{
			type = 1;
		}
		else if (args[2] == "mod")
		{
			type = 2;
		}
		else if (args[2] == "sky")
		{
			type = 3;
		}
		IGZ::IGZ igZ(game, type, (char*)(void*)Marshal::StringToHGlobalAnsi(args[0]), IGZ::IGZ::LITTLE);
		return 0;
	}
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false);
	IgzModelConverterGUI::MyForm form;
	Application::Run(%form);
	return 0;
}