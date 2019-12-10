#include "Reader.h"
	char Reader::ReadByte(FILE* f, int e)
	{

		char read;
		fread_s(&read, 1, 1, 1, f);
		return read;

	}

	unsigned char Reader::ReadUByte(FILE* f, int e)
	{
		unsigned char read;
		fread_s(&read, 1, 1, 1, f);
		return read;
	}


	short Reader::ReadShort(FILE* f, int e)
	{
		if (e == 0)
		{
			short read;
			fread_s(&read, 2, 2, 1, f);
			return read;
		}
		else
		{
			unsigned char read[2];
			fread_s(read, 2, 2, 1, f);
			return static_cast<short>(
				(read[1]) |
				(read[0] << 8));
		}

	}

	unsigned short Reader::ReadUShort(FILE* f, int e)
	{
		if (e == 0)
		{
			unsigned short read;
			fread_s(&read, 2, 2, 1, f);
			return read;
		}
		else
		{
			unsigned char read[2];
			fread_s(&read, 2, 2, 1, f);
			return static_cast<unsigned short>(
				(read[1]) |
				(read[0] << 8));
		}
	}

	half Reader::ReadHalfFloat(FILE* f, int e)
	{
		if (e == 0)
		{
			half read;
			fread_s(&read, 2, 2, 1, f);
			return read;
		}
		else
		{
			unsigned char read[2];
			fread_s(read, 2, 2, 1, f);
			return static_cast<float>(
				(read[1]) |
				(read[0] << 8));
		}
		//half read;
		//fread_s(&read, 2, 2, 1, f);
		//return read;
	}

	float Reader::ReadFloat(FILE* f, int e)
	{
		if (e == 0)
		{
			float read;
			fread_s(&read, 4, 4, 1, f);
			return read;
		}
		else
		{
			unsigned char read[4];
			fread_s(read, 4, 4, 1, f);
			return static_cast<float>(
				(read[3]) |
				(read[2] << 8) |
				(read[1] << 16) |
				(read[0] << 24));
		}
	}

	long Reader::ReadLong(FILE* f, int e)
	{
		if (e == 0)
		{
			long read;
			fread_s(&read, 4, 4, 1, f);
			return read;
		}
		else
		{
			unsigned char read[4];
			fread_s(read, 4, 4, 1, f);
			return static_cast<long>(
				(read[3]) |
				(read[2] << 8) |
				(read[1] << 16) |
				(read[0] << 24));
		}
	}

	unsigned long Reader::ReadULong(FILE* f, int e)
	{
		if (e == 0)
		{
			unsigned long read;
			fread_s(&read, 4, 4, 1, f);
			return read;
		}
		else
		{
			unsigned char read[4];
			fread_s(read, 4, 4, 1, f);
			return static_cast<unsigned long>(
				(read[3]) |
				(read[2] << 8) |
				(read[1] << 16) |
				(read[0] << 24));
		}
	}

	long long Reader::ReadLongLong(FILE* f, int e)
	{
		if (e == 0)
		{
			long long read;
			fread_s(&read, 8, 8, 1, f);
			return read;
		}
		else
		{
			unsigned char read[8];
			fread_s(read, 8, 8, 1, f);
			return static_cast<long long>(
				(read[7]) |
				(read[6] << 8) |
				(read[5] << 16) |
				(read[4] << 24) |
				(read[3] << 32) |
				(read[2] << 40) |
				(read[1] << 48) |
				(read[0] << 56));
		}
	}

	unsigned long long Reader::ReadULongLong(FILE* f, int e)
	{
		if (e == 0)
		{
			unsigned long long read;
			fread_s(&read, 8, 8, 1, f);
			return read;
		}
		else
		{
			unsigned char read[8];
			fread_s(read, 8, 8, 1, f);
			return static_cast<unsigned long long>(
				(read[7]) |
				(read[6] << 8) |
				(read[5] << 16) |
				(read[4] << 24) |
				(read[3] << 32) |
				(read[2] << 40) |
				(read[1] << 48) |
				(read[0] << 56));
		}
	}

	std::string Reader::ReadString(FILE* f)
	{
		std::string read;
		char c;
		do
		{
			fread_s(&c, 1, 1, 1, f);
			read.push_back(c);
		} while (c != '\0');
		return read;
	}
	std::string Reader::Reader::ReadString(FILE* f, int chars)
	{
		std::string read;
		char c;
		for (int i = 0; i < chars; i++)
		{
			fread_s(&c, 1, 1, 1, f);
			read.push_back(c);
		}
		return read;
	}
	