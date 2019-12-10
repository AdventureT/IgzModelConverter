#pragma once
#include <stdio.h>
#include "umHalf.h"
#include <string>
#include <stdlib.h>
	class Reader
	{
	public:
		char ReadByte(FILE* f,int e);

		unsigned char ReadUByte(FILE* f, int e);

		short ReadShort(FILE* f, int e);

		unsigned short ReadUShort(FILE* f, int e);

		half ReadHalfFloat(FILE* f, int e);

		float ReadFloat(FILE* f, int e);

		long ReadLong(FILE* f, int e);

		unsigned long ReadULong(FILE* f, int e);

		long long ReadLongLong(FILE* f, int e);

		unsigned long long ReadULongLong(FILE* f, int e);

		std::string ReadString(FILE* f);

		std::string ReadString(FILE* f, int chars);

	};