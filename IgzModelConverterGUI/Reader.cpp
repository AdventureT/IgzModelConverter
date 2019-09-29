#include "Reader.h"

int Reader::ReadByte(FILE* f)
{
	int read;
	fread_s(&read, 1, 1, 1, f);
	return read;
}

int Reader::ReadUByte(FILE* f)
{
	unsigned int read;
	fread_s(&read, 1, 1, 1, f);
	return read;
}


short Reader::ReadShort(FILE* f)
{
	short read;
	fread_s(&read, 2, 2, 1, f);
	return read;
}

unsigned short Reader::ReadUShort(FILE* f)
{
	unsigned short read;
	fread_s(&read, 2, 2, 1, f);
	return read;
}

half Reader::ReadHalfFloat(FILE* f)
{
	half read;
	fread_s(&read, 2, 2, 1, f);
	return read;
}

float Reader::ReadFloat(FILE* f)
{
	float read;
	fread_s(&read, 4, 4, 1, f);
	return read;
}

long Reader::ReadLong(FILE* f)
{
	long read;
	fread_s(&read, 4, 4, 1, f);
	return read;
}

unsigned long Reader::ReadULong(FILE* f)
{
	unsigned long read;
	fread_s(&read, 4, 4, 1, f);
	return read;
}

long long Reader::ReadLongLong(FILE* f)
{
	long long read;
	fread_s(&read, 8, 8, 1, f);
	return read;
}

unsigned long long Reader::ReadULongLong(FILE* f)
{
	unsigned long long read;
	fread_s(&read, 8, 8, 1, f);
	return read;
}