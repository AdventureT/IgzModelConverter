/*	a source for MasterPrinter class
	more info in README for PreCore Project

	Copyright 2018-2019 Lukas Cone

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include "masterprinter.hpp"
#include <mutex>
#include <vector>
#include <thread>

#ifndef _TCHAR_DEFINED
#ifdef _UNICODE
#define tprintf     swprintf
#else
#define tprintf     snprintf
#endif
#endif

#ifdef _MSC_VER
#include <Windows.h>
#endif

#include "datas/flags.hpp"

enum MSVC_Console_Flags
{
	MSC_Text_Blue,
	MSC_Text_Green,
	MSC_Text_Red,
	MSC_Text_Intensify,
};

typedef esFlags<short, MSVC_Console_Flags> consoleColorAttrFlags;

static struct MasterPrinter
{
	std::vector<void*> functions;
	std::mutex _mutexPrint;
	bool printThreadID = false;
	consoleColorAttrFlags consoleColorAttr;
}__MasterPrinter;

void MasterPrinterThread::AddPrinterFunction(void *funcPtr)
{
	for (auto &c : __MasterPrinter.functions)
		if (c == funcPtr)
			return;
	__MasterPrinter.functions.push_back(funcPtr);
}

void MasterPrinterThread::FlushAll()
{
	const size_t buffsize = static_cast<size_t>(_masterstream->tellp()) + 1;
	_masterstream->seekp(0);
	TCHAR *tempOut = static_cast<TCHAR*>(malloc(buffsize * sizeof(TCHAR)));
	_masterstream->read(tempOut, buffsize);

	if (buffsize > static_cast<size_t>(maximumStreamSize))
		_masterstream->str(StringType(_T("")));

	_masterstream->clear();
	_masterstream->seekg(0);

	std::lock_guard<std::mutex> guard(__MasterPrinter._mutexPrint);
	for (auto &f : __MasterPrinter.functions)
	{
		int(*Func)(const TCHAR*) = reinterpret_cast<int(*)(const TCHAR*)>(f);
		if (__MasterPrinter.printThreadID)
		{
			Func(_T("Thread[0x"));
			std::thread::id threadID = std::this_thread::get_id();
			TCHAR buffer[65];
			tprintf(buffer, 65, _T("%X"), static_cast<uint>(reinterpret_cast<uint&>(threadID)));
			Func(buffer);
			Func(_T("] "));
		}
		Func(tempOut);
	}
	free(tempOut);
}

void MasterPrinterThread::operator >> (int endWay)
{
	if (endWay)
		(*_masterstream) << std::endl;
	(*_masterstream) << std::ends;
	FlushAll();
}

void MasterPrinterThread::PrintThreadID(bool yn)
{
	__MasterPrinter.printThreadID = yn;
}

MasterPrinterThread::MasterPrinterThread() :_masterstream(new MasterStreamType()) 
{
#ifdef _MSC_VER
	CONSOLE_SCREEN_BUFFER_INFO conInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &conInfo);
	__MasterPrinter.consoleColorAttr = conInfo.wAttributes & (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
#endif
}
MasterPrinterThread::~MasterPrinterThread()
{
	if (_masterstream)
		delete _masterstream;
}

void SetConsoleTextColor(int red, int green, int blue)
{
#ifdef _MSC_VER
	consoleColorAttrFlags newFlags = __MasterPrinter.consoleColorAttr;
	newFlags(MSC_Text_Red, red > 0);
	newFlags(MSC_Text_Green, green > 0);
	newFlags(MSC_Text_Blue, blue > 0);
	newFlags(MSC_Text_Intensify, (red | green | blue) > 128);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), reinterpret_cast<consoleColorAttrFlags::ValueType &>(newFlags));
#else
	printer << "\033[38;2;" << red << ';' << green << ';' << blue << 'm' >> 0;
#endif
}

void RestoreConsoleTextColor()
{
#ifdef _MSC_VER
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), reinterpret_cast<consoleColorAttrFlags::ValueType &>(__MasterPrinter.consoleColorAttr));
#else
	printer << "\033[0m" >> 0;
#endif
}