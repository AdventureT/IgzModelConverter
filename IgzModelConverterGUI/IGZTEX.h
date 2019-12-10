#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "umHalf.h"
#include "windows.h"
#include <vcclr.h>
#include <shellapi.h>
#include <msclr\marshal_cppstd.h>
#include "Reader.h"

namespace IGZTEX
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Runtime::InteropServices;
	using namespace System::Collections;
	using namespace System::Collections::Generic;
	using namespace std;
	using namespace System::IO;
	//namespace fs = std::experimental::filesystem;
	using namespace msclr::interop;
	class IGZTEX : Reader
	{

	private:
		FILE* f;
		long igzVersion = 0;
		long* dataOffset = new long[4];
		long* dataCount = new long[4];
		long* dataSize = new long[4];
		char* fileNameTex = new char[256];
		
	public:
		IGZTEX(char* fileName,int e); //Constructor
	protected:

		struct TSTR
		{
			long count;
			long size;
			long start;
			vector<std::string> text;
		};

		struct TMET
		{
			long count;
			long size;
			long start;
			vector<std::string> text;
		};

		struct MTSZ
		{
			long count;
			long size;
			long start;
			vector<long> unknown;
		};

		struct EXID
		{
			long count;
			long size;
			long start;
			vector<unsigned long> idenifier;
		};

		struct ROFS
		{
			long count;
			long size;
			long start;
			//vector<byte> unknown; //Isn't right
		};

		struct REXT
		{
			long count;
			long size;
			long start;
			vector<long> unknown;
		};

		struct ROOT
		{
			long count;
			long size;
			long start;
			vector<long> unknown;
		};

		struct RVTB
		{
			long count;
			long size;
			long start;
			//vector<byte> unknown; //Seems to be not going for bytes though
		};

		struct RSTT
		{
			long count;
			long size;
			long start;
			//vector<byte> unknown; //Seems to be not going for bytes though
		};

		struct RPID
		{
			long count;
			long size;
			long start;
		};

		struct RHND
		{
			long count;
			long size;
			long start;
			vector<long> unknown;
		};

		struct ONAM
		{
			long count;
			long size;
			long start;
			vector<long> unknown;
		};

		TSTR tstr;
		TMET tmet;
		MTSZ mtsz;
		EXID exid;
		ROFS rofs;
		REXT rext;
		ROOT root;
		ONAM onam;
		RVTB rvtb;
		RHND rhnd;
		RPID rpid;
		RSTT rstt;
		int endian;
		void readHeader();
		void readTextureFile();
		void readTags();
	};

}


