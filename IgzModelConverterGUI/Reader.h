#pragma once
#include <stdio.h>
#include "umHalf.h"

class Reader
{
	public:
		int ReadByte(FILE* f);

		int ReadUByte(FILE* f);

		short ReadShort(FILE* f);

		unsigned short ReadUShort(FILE* f);

		half ReadHalfFloat(FILE* f);

		float ReadFloat(FILE* f);

		long ReadLong(FILE* f);

		unsigned long ReadULong(FILE* f);

		long long ReadLongLong(FILE* f);

		unsigned long long ReadULongLong(FILE* f);
};

