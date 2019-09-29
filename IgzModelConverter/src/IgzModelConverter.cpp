// IgzModelConverter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

//#include "pch.h"
#include "half.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fbxsdk.h>
#include <filesystem>


using half_float::half;
namespace fs = std::experimental::filesystem;

#include<tchar.h>

int GetDir(TCHAR *fullPath, TCHAR *dir) {
	const int buffSize = 1024;

	TCHAR buff[buffSize] = { 0 };
	int buffCounter = 0;
	int dirSymbolCounter = 0;

	for (unsigned int i = 0; i < _tcslen(fullPath); i++) {
		if (fullPath[i] != L'\\') {
			if (buffCounter < buffSize) buff[buffCounter++] = fullPath[i];
			else return -1;
		}
		else {
			for (int i2 = 0; i2 < buffCounter; i2++) {
				dir[dirSymbolCounter++] = buff[i2];
				buff[i2] = 0;
			}

			dir[dirSymbolCounter++] = fullPath[i];
			buffCounter = 0;
		}
	}

	return dirSymbolCounter;
}


int ReadByte(FILE* f, int read)
{
	fread_s(&read, 1, 1, 1, f);
	return read;
}

int ReadUByte(FILE* f, unsigned int read)
{
	fread_s(&read, 1, 1, 1, f);
	return read;
}


int ReadShort(FILE* f, short read)
{
	fread_s(&read, 2, 2, 1, f);
	return read;
}

int ReadUShort(FILE* f, unsigned short read)
{
	fread_s(&read, 2, 2, 1, f);
	return read;
}

half ReadHalfFloat(FILE* f, half read)
{
	fread_s(&read, 2, 2, 1, f);
	return read;
}

float ReadFloat(FILE* f, float read)
{
	fread_s(&read, 4, 4, 1, f);
	return read;
}

int ReadLong(FILE* f, long read)
{
	fread_s(&read, 4, 4, 1, f);
	return read;
}

int ReadULong(FILE* f, unsigned long read)
{
	fread_s(&read, 4, 4, 1, f);
	return read;
}

long long ReadLongLong(FILE* f, long long read)
{
	fread_s(&read, 8, 8, 1, f);
	return read;
}

unsigned long long ReadULongLong(FILE* f)
{
	unsigned long long read;
	fread_s(&read, 8, 8, 1, f);
	return read;
}

int main(int argc, char const *argv[])
{
	#pragma region Init of variables
	FILE* f;
	char inputType[4];
	long igzVersion = 0;
	long* dataOffset = new long[4];
	long* dataCount = new long[4];
	long long subDataOffset = 0;
	long meshDataOffset = 0;
	long long boneDataOffset = 0;
	long long boneDataTableOffset = 0;
	long long boneTableOffset = 0;
	long long boneOffset = 0;
	long boneCount = 0;
	long long meshTableOffset = 0;
	long long meshCount = 0;
	int fscale = 1;
	long long vertDataOffset = 0;
	long long faceDataOffset = 0;
	long long subVertDataOffset = 0;
	unsigned long long subVertDataOffset3 = 0;
	long vertCount = 0;
	long long vertOffset = 0;
	long vertexStride = 0;
	float vertexX = 0;
	float vertexY = 0;
	float vertexZ = 0;
	float normalX = 0;
	float normalY = 0;
	float normalZ = 0;
	half huvU(0);
	half huvV(0);
	float uvU(0);
	float uvV(0);
	long long subFaceDataOffset = 0;
	const char* pChar = new char[256];
	char* MeshName = new char[256];
	int faceCount = 0;
	int faceSize = 0;
	unsigned long long faceOffset = 0;

	long chunkOffset = 0;
	char chunkTag[4];
	long chunkDataCount = 0;
	long chunkLength = 0;
	long chunkDataOffset = 0;
	long chunkEnd = 0;

	int matCount = 0;
	bool command = false;
	#pragma endregion

	#pragma region Security Checks
	if (argc < 2)	// Argument count < 2, no input file
	{
		return 0;
	}
	if (argc > 2)
	{
		// If you want to do it with the command line
		command = true;
	}
	if (fopen_s(&f, argv[1], "rb") != 0) // Security check
	{
		printf("error! cannot open file %s\n", argv[1]);
		system("pause");
		exit(1); // Terminate the program
	}
	#pragma endregion
	char inputGame[4];
	#pragma region Input
	if (command == false)
	{
		printf("Please enter your game [type nst for Crash Bandicoot N.Sane Trilogy | type ctr for Crash Team Racing Nitro-Fueled | type skl for Skylanders Imaginators]: ");
		scanf_s("%4s", inputGame, sizeof(inputGame));
		printf("Please enter your type [mod for model | act for actor | sky for skybox models]: ");
		scanf_s("%4s", inputType, sizeof(inputType));
		//printf("Asset? [type yes or no]: ");
		//scanf_s("%4s", inputType2, sizeof(inputType2));

	}
#pragma endregion

	#pragma region Magic+DataOffset
	fseek(f, 4, SEEK_CUR);
	igzVersion = ReadLong(f, igzVersion);
	fseek(f, 8, SEEK_CUR);
	for (int i = 1; i <= 4; i++)
	{
		dataCount[i] = ReadLong(f, dataCount[i]);
		fseek(f, 4, SEEK_CUR);
		dataOffset[i] = ReadLong(f, dataOffset[i]);
		fseek(f, 4, SEEK_CUR);
	}
	fseek(f, dataOffset[1], SEEK_SET);
	for (int i = 1; i <= dataOffset[1]; i++)
	{
		chunkOffset = ftell(f);
		fread_s(chunkTag, 4, 4, 1, f);
		chunkDataCount = ReadLong(f, chunkDataCount);
		chunkLength = ReadLong(f, chunkLength);
		chunkDataOffset = ReadLong(f, chunkDataOffset);
		chunkEnd = chunkOffset + chunkLength;

		if (strcmp(chunkTag, "TSTR") == 0)
		{

		}
		else
		{
			matCount = chunkDataCount;
		}
	}
	#pragma endregion

	#pragma region Data

	fseek(f, dataOffset[2], SEEK_SET);
	if (command)
	{
		if (strcmp(argv[2], "skl") == 0)
		{
			unsigned short marker = 0;
			do
			{
				fread_s(&marker, 2, 2, 1, f);
			} while (marker != 34816);
			if (strcmp(argv[3], "mod") == 0)
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 8, SEEK_CUR);
				}
			}
			else
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 4, SEEK_CUR);
				}
			}
		}
		else
		{
			int marker = 0;
			do
			{
				fread_s(&marker, 2, 2, 1, f);
			} while (marker != 272);
			if (strcmp(argv[3], "mod") == 0)
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 8, SEEK_CUR);
				}
			}
			else
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 4, SEEK_CUR);
				}
			}
		}
	}
	else
	{
		if (strcmp(inputGame, "skl") == 0)
		{
			unsigned short marker = 0;
			do
			{
				fread_s(&marker, 2, 2, 1, f);
			} while (marker != 34816);
			if (strcmp(inputType, "mod") == 0)
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 8, SEEK_CUR);
				}
			}
			else
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 4, SEEK_CUR);
				}
			}
		}
		else
		{
			int marker = 0;
			do
			{
				fread_s(&marker, 2, 2, 1, f);
			} while (marker != 272);
			if (strcmp(inputType, "mod") == 0)
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 8, SEEK_CUR);
				}
			}
			else
			{
				long test = 0;
				fread_s(&test, 4, 4, 1, f);
				if (test > 0xFF)
				{
					fseek(f, 4, SEEK_CUR);
				}
			}
		}
	}
	


	//fseek(f, 0x60, SEEK_CUR); // 0x28
	boneDataOffset = ReadLongLong(f, boneDataOffset) + dataOffset[2];
	meshDataOffset = ReadLong(f, meshDataOffset) + dataOffset[2];
	/*
	_fseeki64(f, boneDataOffset, SEEK_SET);
	fseek(f, 24, SEEK_CUR);

	boneDataTableOffset = ReadLongLong(f, boneDataTableOffset) + dataOffset[2];
	fseek(f, 8, SEEK_CUR);
	boneOffset = ReadLongLong(f, boneOffset) + dataOffset[2];

	_fseeki64(f, boneDataTableOffset, SEEK_SET);
	fseek(f, 16, SEEK_CUR);
	boneCount = ReadLong(f, boneCount);
	fseek(f, 12, SEEK_CUR);
	boneTableOffset = ReadLongLong(f, boneTableOffset) + dataOffset[2];

	_fseeki64(f, boneTableOffset, SEEK_SET);
	_fseeki64(f, boneOffset, SEEK_SET);

	float m11 = 0;
	float m12 = 0;
	float m13 = 0;
	float m14 = 0;
	float m21 = 0;
	float m22 = 0;
	float m23 = 0;
	float m24 = 0;
	float m31 = 0;
	float m32 = 0;
	float m33 = 0;
	float m34 = 0;
	float m41 = 0;
	float m42 = 0;
	float m43 = 0;
	float m44 = 0;

	for (int i = 0; i < boneCount; i++)
	{
		m11 = ReadFloat(f,m11); m12 = ReadFloat(f, m12); m13 = ReadFloat(f, m13); m14 = ReadFloat(f, m14);
			m21 = ReadFloat(f, m21); m22 = ReadFloat(f, m22); m23 = ReadFloat(f, m23); m24 = ReadFloat(f, m24);
			m31 = ReadFloat(f, m31); m32 = ReadFloat(f, m32); m33 = ReadFloat(f, m33); m34 = ReadFloat(f, m34);
			m41 = ReadFloat(f, m41); m42 = ReadFloat(f, m42); m43 = ReadFloat(f, m43); m44 = ReadFloat(f, m44);
	}

	long long *boneDataOffsetArr = new long long[(int)boneCount];
	for (int i = 1; i <= boneCount; i++)
	{
		boneDataOffsetArr[i] = ReadULongLong(f, boneDataOffsetArr[i]) + dataOffset[2];
	}
	*/
	fseek(f, meshDataOffset, SEEK_SET);
	/*
	int marker2 = 0;
	do
	{
		fread_s(&marker, 2, 2, 1, f);
		fread_s(&marker2, 2, 2, 1, f);
	} while (marker2 == 0);
	tell = ftell(f);
	do
	{
		fread_s(&marker, 2, 2, 1, f);
	} while (marker != 272);
	fseek(f, 46, SEEK_CUR);
	fread_s(&marker, 2, 2, 1, f);
	if (marker != 272)
	{
		fseek(f, -48, SEEK_CUR);
	}
	tell = ftell(f);
	*/
	int marker4 = 0;
	unsigned long Umarker = 0;
	unsigned long Umarker2 = 0;
	if (command)
	{
		if (strcmp(argv[2], "skl") == 0)
		{
			if (strcmp(argv[3], "act") == 0)
			{
				do
				{
					fread_s(&Umarker, 4, 4, 1, f);
				} while (Umarker != 0xFFFFFFFF);
				int start = ftell(f);
				do
				{
					fread_s(&marker4, 4, 4, 1, f);
				} while (marker4 != 904786743);
				int fin = ftell(f);
				fseek(f, meshDataOffset, SEEK_SET);
				marker4 = 0;
				long marker2 = 0;
				unsigned long marker3 = 0;
				/*
				do
				{
					fread_s(&marker, 2, 2, 1, f);
					if (marker != 0)
					{
						fread_s(&marker2, 2, 2, 1, f);
						fread_s(&marker3, 2, 2, 1, f);
					}
				} while (marker3 == 0 || marker2 == 0);
				*/
				do
				{
					do
					{
						fread_s(&marker3, 2, 2, 1, f);
					} while (marker3 != 34816);
					fread_s(&marker2, 4, 4, 1, f);
					marker2 += dataOffset[2];
				} while (marker2 <= start || marker2 >= fin);
				fseek(f, -4, SEEK_CUR);
			}
			else
			{
				marker4 = 0;
				int marker2 = 0;
				do
				{
					fread_s(&marker4, 2, 2, 1, f);
					fread_s(&marker2, 2, 2, 1, f);
				} while (marker2 == 0);
				do
				{
					fread_s(&marker4, 2, 2, 1, f);
				} while (marker4 != 34816);
				fseek(f, 4, SEEK_CUR);
				fread_s(&marker4, 2, 2, 1, f);
				if (marker4 == 34816)
				{

				}
				else
				{
					fseek(f, 40, SEEK_CUR);
					fread_s(&marker4, 2, 2, 1, f);
					if (marker4 != 34816)
					{
						fseek(f, -48, SEEK_CUR);
					}
					fseek(f, 14, SEEK_CUR);
					fread_s(&marker4, 2, 2, 1, f);
					if (marker4 != 34816)
					{
						fseek(f, -16, SEEK_CUR);
					}
				}
			}

		}
		else
		{
			if (strcmp(argv[3], "act") == 0)
			{
				do
				{
					fread_s(&Umarker, 4, 4, 1, f);
				} while (Umarker != 0xFFFFFFFF);
				int start = ftell(f);
				do
				{
					fread_s(&marker4, 4, 4, 1, f);
				} while (marker4 != 904786743);
				int fin = ftell(f);
				fseek(f, meshDataOffset, SEEK_SET);
				marker4 = 0;
				long marker2 = 0;
				unsigned long marker3 = 0;
				/*
				do
				{
					fread_s(&marker, 2, 2, 1, f);
					if (marker != 0)
					{
						fread_s(&marker2, 2, 2, 1, f);
						fread_s(&marker3, 2, 2, 1, f);
					}
				} while (marker3 == 0 || marker2 == 0);
				*/
				do
				{
					do
					{
						fread_s(&marker4, 2, 2, 1, f);
					} while (marker4 != 272);
					fread_s(&marker2, 4, 4, 1, f);
					marker2 += dataOffset[2];
				} while (marker2 <= start || marker2 >= fin);
				fseek(f, -4, SEEK_CUR);
			}
			else
			{
				marker4 = 0;
				int marker2 = 0;
				do
				{
					fread_s(&marker4, 2, 2, 1, f);
					fread_s(&marker2, 2, 2, 1, f);
				} while (marker2 == 0);
				do
				{
					fread_s(&marker4, 2, 2, 1, f);
				} while (marker4 != 272);
				fseek(f, 4, SEEK_CUR);
				fread_s(&marker4, 2, 2, 1, f);
				if (marker4 == 272)
				{

				}
				else
				{
					fseek(f, 40, SEEK_CUR);
					fread_s(&marker4, 2, 2, 1, f);
					if (marker4 != 272)
					{
						fseek(f, -48, SEEK_CUR);
					}
					fseek(f, 14, SEEK_CUR);
					fread_s(&marker4, 2, 2, 1, f);
					if (marker4 != 272)
					{
						fseek(f, -16, SEEK_CUR);
					}
				}
				/*
				do
				{
					fread_s(&Umarker, 4, 4, 1, f);
				} while (Umarker != 904786743);
				int start = ftell(f);
				do
				{
					fread_s(&marker, 4, 4, 1, f);
				} while (marker != 0xFFFFFFFF);
				int fin = ftell(f);
				fseek(f, meshDataOffset, SEEK_SET);
				marker = 0;
				long marker2 = 0;
				unsigned long marker3 = 0;
				do
				{
					fread_s(&marker, 2, 2, 1, f);
					if (marker != 0)
					{
						fread_s(&marker2, 2, 2, 1, f);
						fread_s(&marker3, 2, 2, 1, f);
					}
				} while (marker3 == 0 || marker2 == 0);
				tell = ftell(f);
				do
				{
					do
					{
						fread_s(&marker, 2, 2, 1, f);
						tell = ftell(f);
					} while (marker != 272);
					tell = ftell(f);
					fread_s(&marker2, 4, 4, 1, f);
					marker2 += dataOffset[2];
				} while (marker2 <= start || marker2 >= fin);
				fseek(f, -4, SEEK_CUR);
				tell = ftell(f);
				*/
			}
		}
	
	}
	else
	{
	if (strcmp(inputGame, "skl") == 0)
	{
		if (strcmp(inputType, "act") == 0)
		{
			do
			{
				fread_s(&Umarker, 4, 4, 1, f);
			} while (Umarker != 0xFFFFFFFF);
			int start = ftell(f);
			do
			{
				fread_s(&marker4, 4, 4, 1, f);
			} while (marker4 != 904786743);
			int fin = ftell(f);
			fseek(f, meshDataOffset, SEEK_SET);
			marker4 = 0;
			long marker2 = 0;
			unsigned long marker3 = 0;
			/*
			do
			{
				fread_s(&marker, 2, 2, 1, f);
				if (marker != 0)
				{
					fread_s(&marker2, 2, 2, 1, f);
					fread_s(&marker3, 2, 2, 1, f);
				}
			} while (marker3 == 0 || marker2 == 0);
			*/
			do
			{
				do
				{
					fread_s(&marker3, 2, 2, 1, f);
				} while (marker3 != 34816);
				fread_s(&marker2, 4, 4, 1, f);
				marker2 += dataOffset[2];
			} while (marker2 <= start || marker2 >= fin);
			fseek(f, -4, SEEK_CUR);
		}
		else
		{
			marker4 = 0;
			int marker2 = 0;
			do
			{
				fread_s(&marker4, 2, 2, 1, f);
				fread_s(&marker2, 2, 2, 1, f);
			} while (marker2 == 0);
			do
			{
				fread_s(&marker4, 2, 2, 1, f);
			} while (marker4 != 34816);
			fseek(f, 4, SEEK_CUR);
			fread_s(&marker4, 2, 2, 1, f);
			if (marker4 == 34816)
			{

			}
			else
			{
				fseek(f, 40, SEEK_CUR);
				fread_s(&marker4, 2, 2, 1, f);
				if (marker4 != 34816)
				{
					fseek(f, -48, SEEK_CUR);
				}
				fseek(f, 14, SEEK_CUR);
				fread_s(&marker4, 2, 2, 1, f);
				if (marker4 != 34816)
				{
					fseek(f, -16, SEEK_CUR);
				}
			}
		}

	}
	else
	{
		if (strcmp(inputType, "act") == 0)
		{
			do
			{
				fread_s(&Umarker, 4, 4, 1, f);
			} while (Umarker != 0xFFFFFFFF);
			int start = ftell(f);
			do
			{
				fread_s(&marker4, 4, 4, 1, f);
			} while (marker4 != 904786743);
			int fin = ftell(f);
			fseek(f, meshDataOffset, SEEK_SET);
			marker4 = 0;
			long marker2 = 0;
			unsigned long marker3 = 0;
			/*
			do
			{
				fread_s(&marker, 2, 2, 1, f);
				if (marker != 0)
				{
					fread_s(&marker2, 2, 2, 1, f);
					fread_s(&marker3, 2, 2, 1, f);
				}
			} while (marker3 == 0 || marker2 == 0);
			*/
			do
			{
				do
				{
					fread_s(&marker4, 2, 2, 1, f);
				} while (marker4 != 272);
				fread_s(&marker2, 4, 4, 1, f);
				marker2 += dataOffset[2];
			} while (marker2 <= start || marker2 >= fin);
			fseek(f, -4, SEEK_CUR);
		}
		else
		{
			marker4 = 0;
			int marker2 = 0;
			do
			{
				fread_s(&marker4, 2, 2, 1, f);
				fread_s(&marker2, 2, 2, 1, f);
			} while (marker2 == 0);
			do
			{
				fread_s(&marker4, 2, 2, 1, f);
			} while (marker4 != 272);
			fseek(f, 4, SEEK_CUR);
			fread_s(&marker4, 2, 2, 1, f);
			if (marker4 == 272)
			{

			}
			else
			{
				fseek(f, 40, SEEK_CUR);
				fread_s(&marker4, 2, 2, 1, f);
				if (marker4 != 272)
				{
					fseek(f, -48, SEEK_CUR);
				}
				fseek(f, 14, SEEK_CUR);
				fread_s(&marker4, 2, 2, 1, f);
				if (marker4 != 272)
				{
					fseek(f, -16, SEEK_CUR);
				}
			}
			/*
			do
			{
				fread_s(&Umarker, 4, 4, 1, f);
			} while (Umarker != 904786743);
			int start = ftell(f);
			do
			{
				fread_s(&marker, 4, 4, 1, f);
			} while (marker != 0xFFFFFFFF);
			int fin = ftell(f);
			fseek(f, meshDataOffset, SEEK_SET);
			marker = 0;
			long marker2 = 0;
			unsigned long marker3 = 0;
			do
			{
				fread_s(&marker, 2, 2, 1, f);
				if (marker != 0)
				{
					fread_s(&marker2, 2, 2, 1, f);
					fread_s(&marker3, 2, 2, 1, f);
				}
			} while (marker3 == 0 || marker2 == 0);
			tell = ftell(f);
			do
			{
				do
				{
					fread_s(&marker, 2, 2, 1, f);
					tell = ftell(f);
				} while (marker != 272);
				tell = ftell(f);
				fread_s(&marker2, 4, 4, 1, f);
				marker2 += dataOffset[2];
			} while (marker2 <= start || marker2 >= fin);
			fseek(f, -4, SEEK_CUR);
			tell = ftell(f);
			*/
		}
	}
	}
	
		/*
		do
		{
			fread_s(&Umarker, 4, 4, 1, f);
		} while (Umarker != 904786743);
		int start = ftell(f);
		do
		{
			fread_s(&marker, 4, 4, 1, f);
		} while (marker != 0xFFFFFFFF);
		int fin = ftell(f);
		fseek(f, meshDataOffset, SEEK_SET);
		marker = 0;
		long marker2 = 0;
		unsigned long marker3 = 0;
		do
		{
			fread_s(&marker, 2, 2, 1, f);
			if (marker != 0)
			{
				fread_s(&marker2, 2, 2, 1, f);
				fread_s(&marker3, 2, 2, 1, f);
			}
		} while (marker3 == 0 || marker2 == 0);
		tell = ftell(f);
		do
		{
			do
			{
				fread_s(&marker, 2, 2, 1, f);
				tell = ftell(f);
			} while (marker != 272);
			tell = ftell(f);
			fread_s(&marker2, 4, 4, 1, f);
			marker2 += dataOffset[2];
		} while (marker2 <= start || marker2 >= fin);
		fseek(f, -4, SEEK_CUR);
		tell = ftell(f);
		*/

	meshTableOffset = ReadLongLong(f, meshTableOffset) + dataOffset[2];
	meshCount = ReadLongLong(f, meshCount);
	_fseeki64(f, meshTableOffset, SEEK_SET);
	long long *meshDataOffsetArr = new long long[(int)meshCount];
	for (int i = 1; i <= meshCount; i++)
	{
		meshDataOffsetArr[i] = ReadLongLong(f, meshDataOffsetArr[i]) + dataOffset[2];
	}

	pChar = strrchr(argv[1], 92);

	if (pChar == NULL)				// If no backslash is found
		pChar = argv[1];			// Reset pChar
	else
		pChar++;	// Skip the last backslash
	char* lFilename = new char[256];
	char* exportPath = ((char*)argv[1]);
	TCHAR  dir[1024] = { 0 };
	GetDir(exportPath, dir);
	strncpy_s(MeshName, 256, pChar, strlen(pChar) - 4);
	strncpy_s(lFilename, 256, pChar, strlen(pChar) - 4);
	strcat_s(lFilename, 256, ".fbx");
	strcat_s(MeshName, 256, ".obj");
	strcat_s(dir, 256, lFilename);

	int* verticesCount = new int[(int)meshCount];
	int* faceIndicesCount = new int[(int)meshCount];
	bool* noNormals = new bool[(int)meshCount + 1];

	std::vector<float> vertices;
	std::vector<float> normals;
	std::vector<float> uvs;
	std::vector<unsigned short> faces;

	int seekForward = 0;
	long vertBlockLength = 0;
	long faceBlockLength = 0;

	long allVertFaceBlocks = 0;


	for (int n = 1; n <= meshCount; n++)
	{
		seekForward = 0;
		_fseeki64(f, meshDataOffsetArr[n], SEEK_SET);

		fseek(f, 0x48, SEEK_CUR);
		vertDataOffset = ReadLongLong(f, vertDataOffset) + dataOffset[2];
		faceDataOffset = ReadLongLong(f, faceDataOffset) + dataOffset[2];
		_fseeki64(f, vertDataOffset, SEEK_SET);
		fseek(f, 0x20, SEEK_CUR);
		if (command)
		{
			if (strcmp(argv[2], "nst") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 16, SEEK_CUR);
					vertCount = ReadLong(f, vertCount);
					verticesCount[n] = vertCount;
				}
				else
				{
					subVertDataOffset = ReadLongLong(f, subVertDataOffset) + dataOffset[2];
					_fseeki64(f, subVertDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 16, SEEK_CUR);
					vertCount = ReadLong(f, vertCount);
					verticesCount[n] = vertCount;
				}

			}
			else if (strcmp(argv[2], "ctr") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 12, SEEK_CUR);
					vertCount = ReadLong(f, vertCount);
					verticesCount[n] = vertCount;
				}
				else
				{
					fread_s(&subDataOffset, 8, 8, 1, f);
					subDataOffset += dataOffset[2]; //nst/ctr subDataOffset = ReadLongLong + dataOffset[2]
					_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 12, SEEK_CUR);
					vertCount = ReadLong(f, vertCount);
					verticesCount[n] = vertCount;
				}
			}
			else
			{
				subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
				_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
				fseek(f, 12, SEEK_CUR);
				vertCount = ReadLong(f, vertCount);
				verticesCount[n] = vertCount;
			}


			if (strcmp(argv[2], "nst") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					fseek(f, 28, SEEK_CUR);
					vertOffset = ReadShort(f, vertOffset) + dataOffset[4];
					fseek(f, 6, SEEK_CUR);
					subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
				}
				else
				{
					fseek(f, 28, SEEK_CUR);
					vertOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					subVertDataOffset3 = ReadULongLong(f) + dataOffset[2];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
				}
			}
			else if (strcmp(argv[2], "ctr") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					fseek(f, 24, SEEK_CUR);
					vertOffset = ReadShort(f, vertOffset) + dataOffset[4];
					fseek(f, 6, SEEK_CUR);
					subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
				}
				else
				{
					fseek(f, 24, SEEK_CUR);
					vertOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					subVertDataOffset3 = ReadULongLong(f) + dataOffset[2];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
				}
			}
			else
			{
				//Important! Somehow Skylanders don't have any vertOffsets or faceOffsets
				//That's because why I have to do vertStride * vertCount to get the length of 1 vertex data section
				//Same like that with face data
				//then just add the length with dataOffset[4]
				//Sky also works when you select "model"
				fseek(f, 24, SEEK_CUR);
				subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
				_fseeki64(f, subVertDataOffset3, SEEK_SET);
			}



			if (strcmp(argv[2], "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
				vertexStride = ReadLong(f, vertexStride);
				_fseeki64(f, faceDataOffset, SEEK_SET);
			}
			else if (strcmp(argv[2], "ctr") == 0)
			{
				fseek(f, 12, SEEK_CUR);
				vertexStride = ReadLong(f, vertexStride);
				_fseeki64(f, faceDataOffset, SEEK_SET);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
				vertexStride = ReadLong(f, vertexStride);
				if (allVertFaceBlocks == 0)
				{
					vertOffset = dataOffset[4];
					vertBlockLength = vertCount * vertexStride;
					allVertFaceBlocks += vertBlockLength;
				}
				else
				{
					vertOffset = allVertFaceBlocks + dataOffset[4];
					short blank = 0;
					int current = 0;
					bool _break = false;
					unsigned short faceCheck1 = 0;
					unsigned short faceCheck2 = 0;

					fseek(f, 0, SEEK_END); // seek to end of file
					int size = ftell(f); // get current file pointer
					fseek(f, 0, SEEK_SET); // seek back to beginning of file

					current = ftell(f);
					fseek(f, vertOffset, SEEK_SET);
					vertBlockLength = vertCount * vertexStride;
					if (vertBlockLength + vertOffset < size)
					{
						fseek(f, vertBlockLength, SEEK_CUR);
						do
						{
							faceCheck1 = ReadShort(f, faceCheck1);
							faceCheck2 = ReadShort(f, faceCheck2);
							if (faceCheck1 == 0 && faceCheck2 == 1)
							{
								_break = true;
							}
							else
							{
								seekForward += 2;
								fseek(f, -2, SEEK_CUR);
							}
						} while (_break == false);
						vertOffset += seekForward;
						allVertFaceBlocks += vertBlockLength;
						fseek(f, current, SEEK_SET);
					}
				}
				_fseeki64(f, faceDataOffset, SEEK_SET);
			}
			fseek(f, 0x20, SEEK_CUR);
			if (strcmp(argv[2], "nst") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 16, SEEK_CUR);
					faceCount = ReadLong(f, faceCount);
					faceIndicesCount[n] = faceCount;
				}
				else
				{
					subFaceDataOffset = ReadLongLong(f, subFaceDataOffset) + dataOffset[2];
					_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 16, SEEK_CUR);
					faceCount = ReadLong(f, vertCount);
					faceIndicesCount[n] = faceCount;
				}
			}
			else if (strcmp(argv[2], "ctr") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 12, SEEK_CUR);
					faceCount = ReadLong(f, vertCount);
					faceIndicesCount[n] = faceCount;
				}
				else
				{
					fread_s(&subFaceDataOffset, 8, 8, 1, f);
					subFaceDataOffset += dataOffset[2]; //nst/ctr subDataOffset = ReadLongLong + dataOffset[2]
					_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
					fseek(f, 12, SEEK_CUR);
					faceCount = ReadLong(f, vertCount);
					faceIndicesCount[n] = faceCount;
				}
			}
			else
			{
				subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
				_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
				fseek(f, 8, SEEK_CUR);
				faceSize = ReadLong(f, faceSize);
				faceCount = ReadLong(f, vertCount);
				faceIndicesCount[n] = faceCount;
			}
			if (strcmp(argv[2], "nst") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					fseek(f, 28, SEEK_CUR);
					faceOffset = ReadShort(f, faceOffset) + dataOffset[4];
					fseek(f, 6, SEEK_CUR);
					faceSize = ReadLong(f, faceSize);
				}
				else
				{
					fseek(f, 28, SEEK_CUR);
					faceOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					faceSize = ReadLong(f, faceSize);
				}

			}
			else if (strcmp(argv[2], "ctr") == 0)
			{
				if (strcmp(argv[3], "sky") == 0)
				{
					fseek(f, 24, SEEK_CUR);
					faceOffset = ReadShort(f, faceOffset) + dataOffset[4];
					fseek(f, 6, SEEK_CUR);
					faceSize = ReadLong(f, faceSize);
				}
				else
				{
					fseek(f, 24, SEEK_CUR);
					faceOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
					faceSize = ReadLong(f, faceSize);
				}

			}
			else
			{
				faceOffset = (allVertFaceBlocks + dataOffset[4]) + seekForward;
				faceBlockLength = faceCount * 2;
				allVertFaceBlocks += faceBlockLength;
			}
		}
		else
	{
	if (strcmp(inputGame, "nst") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 16, SEEK_CUR);
			vertCount = ReadLong(f, vertCount);
			verticesCount[n] = vertCount;
		}
		else
		{
			subVertDataOffset = ReadLongLong(f, subVertDataOffset) + dataOffset[2];
			_fseeki64(f, subVertDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 16, SEEK_CUR);
			vertCount = ReadLong(f, vertCount);
			verticesCount[n] = vertCount;
		}

	}
	else if (strcmp(inputGame, "ctr") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 12, SEEK_CUR);
			vertCount = ReadLong(f, vertCount);
			verticesCount[n] = vertCount;
		}
		else
		{
			fread_s(&subDataOffset, 8, 8, 1, f);
			subDataOffset += dataOffset[2]; //nst/ctr subDataOffset = ReadLongLong + dataOffset[2]
			_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 12, SEEK_CUR);
			vertCount = ReadLong(f, vertCount);
			verticesCount[n] = vertCount;
		}
	}
	else
	{
		subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
		_fseeki64(f, subDataOffset, SEEK_SET); // subDataOffset
		fseek(f, 12, SEEK_CUR);
		vertCount = ReadLong(f, vertCount);
		verticesCount[n] = vertCount;
	}


	if (strcmp(inputGame, "nst") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			fseek(f, 28, SEEK_CUR);
			vertOffset = ReadShort(f, vertOffset) + dataOffset[4];
			fseek(f, 6, SEEK_CUR);
			subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			_fseeki64(f, subVertDataOffset3, SEEK_SET);
		}
		else
		{
			fseek(f, 28, SEEK_CUR);
			vertOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			subVertDataOffset3 = ReadULongLong(f) + dataOffset[2];
			_fseeki64(f, subVertDataOffset3, SEEK_SET);
		}
	}
	else if (strcmp(inputGame, "ctr") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			fseek(f, 24, SEEK_CUR);
			vertOffset = ReadShort(f, vertOffset) + dataOffset[4];
			fseek(f, 6, SEEK_CUR);
			subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			_fseeki64(f, subVertDataOffset3, SEEK_SET);
		}
		else
		{
			fseek(f, 24, SEEK_CUR);
			vertOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			subVertDataOffset3 = ReadULongLong(f) + dataOffset[2];
			_fseeki64(f, subVertDataOffset3, SEEK_SET);
		}
	}
	else
	{
		//Important! Somehow Skylanders don't have any vertOffsets or faceOffsets
		//That's because why I have to do vertStride * vertCount to get the length of 1 vertex data section
		//Same like that with face data
		//then just add the length with dataOffset[4]
		//Sky also works when you select "model"
		fseek(f, 24, SEEK_CUR);
		subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
		_fseeki64(f, subVertDataOffset3, SEEK_SET);
	}



	if (strcmp(inputGame, "nst") == 0)
	{
		fseek(f, 16, SEEK_CUR);
		vertexStride = ReadLong(f, vertexStride);
		_fseeki64(f, faceDataOffset, SEEK_SET);
	}
	else if (strcmp(inputGame, "ctr") == 0)
	{
		fseek(f, 12, SEEK_CUR);
		vertexStride = ReadLong(f, vertexStride);
		_fseeki64(f, faceDataOffset, SEEK_SET);
	}
	else
	{
		fseek(f, 12, SEEK_CUR);
		vertexStride = ReadLong(f, vertexStride);
		if (allVertFaceBlocks == 0)
		{
			vertOffset = dataOffset[4];
			vertBlockLength = vertCount * vertexStride;
			allVertFaceBlocks += vertBlockLength;
		}
		else
		{
			vertOffset = allVertFaceBlocks + dataOffset[4];
			short blank = 0;
			int current = 0;
			bool _break = false;
			unsigned short faceCheck1 = 0;
			unsigned short faceCheck2 = 0;

			fseek(f, 0, SEEK_END); // seek to end of file
			int size = ftell(f); // get current file pointer
			fseek(f, 0, SEEK_SET); // seek back to beginning of file

			current = ftell(f);
			fseek(f, vertOffset, SEEK_SET);
			vertBlockLength = vertCount * vertexStride;
			if (vertBlockLength + vertOffset < size)
			{
				fseek(f, vertBlockLength, SEEK_CUR);
				do
				{
					faceCheck1 = ReadShort(f, faceCheck1);
					faceCheck2 = ReadShort(f, faceCheck2);
					if (faceCheck1 == 0 && faceCheck2 == 1)
					{
						_break = true;
					}
					else
					{
						seekForward += 2;
						fseek(f, -2, SEEK_CUR);
					}
				} while (_break == false);
				vertOffset += seekForward;
				allVertFaceBlocks += vertBlockLength;
				fseek(f, current, SEEK_SET);
			}
		}
		_fseeki64(f, faceDataOffset, SEEK_SET);
	}
	fseek(f, 0x20, SEEK_CUR);
	if (strcmp(inputGame, "nst") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 16, SEEK_CUR);
			faceCount = ReadLong(f, faceCount);
			faceIndicesCount[n] = faceCount;
		}
		else
		{
			subFaceDataOffset = ReadLongLong(f, subFaceDataOffset) + dataOffset[2];
			_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 16, SEEK_CUR);
			faceCount = ReadLong(f, vertCount);
			faceIndicesCount[n] = faceCount;
		}
	}
	else if (strcmp(inputGame, "ctr") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 12, SEEK_CUR);
			faceCount = ReadLong(f, vertCount);
			faceIndicesCount[n] = faceCount;
		}
		else
		{
			fread_s(&subFaceDataOffset, 8, 8, 1, f);
			subFaceDataOffset += dataOffset[2]; //nst/ctr subDataOffset = ReadLongLong + dataOffset[2]
			_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
			fseek(f, 12, SEEK_CUR);
			faceCount = ReadLong(f, vertCount);
			faceIndicesCount[n] = faceCount;
		}
	}
	else
	{
		subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
		_fseeki64(f, subFaceDataOffset, SEEK_SET); // subDataOffset
		fseek(f, 8, SEEK_CUR);
		faceSize = ReadLong(f, faceSize);
		faceCount = ReadLong(f, vertCount);
		faceIndicesCount[n] = faceCount;
	}
	if (strcmp(inputGame, "nst") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			fseek(f, 28, SEEK_CUR);
			faceOffset = ReadShort(f, faceOffset) + dataOffset[4];
			fseek(f, 6, SEEK_CUR);
			faceSize = ReadLong(f, faceSize);
		}
		else
		{
			fseek(f, 28, SEEK_CUR);
			faceOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			faceSize = ReadLong(f, faceSize);
		}

	}
	else if (strcmp(inputGame, "ctr") == 0)
	{
		if (strcmp(inputType, "sky") == 0)
		{
			fseek(f, 24, SEEK_CUR);
			faceOffset = ReadShort(f, faceOffset) + dataOffset[4];
			fseek(f, 6, SEEK_CUR);
			faceSize = ReadLong(f, faceSize);
		}
		else
		{
			fseek(f, 24, SEEK_CUR);
			faceOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[3];
			faceSize = ReadLong(f, faceSize);
		}

	}
	else
	{
		faceOffset = (allVertFaceBlocks + dataOffset[4]) + seekForward;
		faceBlockLength = faceCount * 2;
		allVertFaceBlocks += faceBlockLength;
	}
	}
		
		/*
		if (command)
		{
			if (strcmp(argv[2], "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
			}
			vertCount = ReadLong(f, vertCount);
			verticesCount[n] = vertCount;
			if (strcmp(argv[2], "nst") == 0)
			{
				fseek(f, 28, SEEK_CUR);
			}
			else
			{
				fseek(f, 24, SEEK_CUR);
			}
		}
		else
		{
			if (strcmp(inputGame, "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
			}
			vertCount = ReadLong(f, vertCount);
			verticesCount[n] = vertCount;
			if (strcmp(inputGame, "nst") == 0)
			{
				fseek(f, 28, SEEK_CUR);
			}
			else
			{
				fseek(f, 24, SEEK_CUR);
			}
		}

		vertOffset = (ReadULongLong(f, vertOffset) - 0x8000000) + dataOffset[3];
		subVertDataOffset3 = ReadULongLong(f, subVertDataOffset3) + dataOffset[2];
		_fseeki64(f, subVertDataOffset3, SEEK_SET);
		if (command)
		{
			if (strcmp(argv[2], "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
			}
			vertexStride = ReadLong(f, vertexStride);
			_fseeki64(f, faceDataOffset, SEEK_SET);
			fseek(f, 0x20, SEEK_CUR);
			subFaceDataOffset = ReadULongLong(f, subFaceDataOffset) + dataOffset[2];
			_fseeki64(f, subFaceDataOffset, SEEK_SET);
			if (strcmp(argv[2], "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
			}
		}
		else
		{
			if (strcmp(inputGame, "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
			}
			vertexStride = ReadLong(f, vertexStride);
			_fseeki64(f, faceDataOffset, SEEK_SET);
			fseek(f, 0x20, SEEK_CUR);
			subFaceDataOffset = ReadULongLong(f, subFaceDataOffset) + dataOffset[2];
			_fseeki64(f, subFaceDataOffset, SEEK_SET);
			if (strcmp(inputGame, "nst") == 0)
			{
				fseek(f, 16, SEEK_CUR);
			}
			else
			{
				fseek(f, 12, SEEK_CUR);
			}
		}
		
		faceCount = ReadLong(f, faceCount);
		faceIndicesCount[n] = faceCount;
		if (command)
		{
			if (strcmp(argv[2], "nst") == 0)
			{
				fseek(f, 28, SEEK_CUR);
			}
			else
			{
				fseek(f, 24, SEEK_CUR);
			}
		}
		else
		{
			if (strcmp(inputGame, "nst") == 0)
			{
				fseek(f, 28, SEEK_CUR);
			}
			else
			{
				fseek(f, 24, SEEK_CUR);
			}
		}

		faceOffset = (ReadULongLong(f, faceOffset) - 0x8000000) + dataOffset[3];
		faceSize = ReadLong(f, faceSize);
		*/
		_fseeki64(f, vertOffset, SEEK_SET);

		#pragma region Collecting vertices, normals, uvs

		switch (vertexStride)
		{
		case 0x10:
		{
			noNormals[n] = true;
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				uvU = ReadHalfFloat(f, huvU);
				uvV = ReadHalfFloat(f, huvV);
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x14:
		{
			noNormals[n] = true;
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f, huvU);
				uvV = ReadHalfFloat(f, huvV);
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x18:
		{
			noNormals[n] = true;
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				//normalX = ReadFloat(f, normalX);
				//normalY = ReadFloat(f, normalY);
				//normalZ = ReadFloat(f, normalZ);
				uvU = ReadHalfFloat(f, huvU);
				uvV = ReadHalfFloat(f, huvV);
				fseek(f, 8, SEEK_CUR);
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x20:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 4, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x24:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 8, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x28:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 8, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x2C:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 12, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x30:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 20, SEEK_CUR);
				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x34:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 20, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x38:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				fseek(f, 28, SEEK_CUR);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x3c:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 32, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);s
			}
			break;
		}
		case 0x40:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 32, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x44:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 40, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x48:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 44, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x4c:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 44, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x50:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 52, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x54:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 52, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x5c:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 64, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x60:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 64, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		case 0x8C:
		{
			for (int i = 1; i <= vertCount; i++)
			{
				vertexX = ReadFloat(f, vertexX);
				vertexY = ReadFloat(f, vertexY);
				vertexZ = ReadFloat(f, vertexZ);
				normalX = ReadFloat(f, normalX);
				normalY = ReadFloat(f, normalY);
				normalZ = ReadFloat(f, normalZ);
				fseek(f, 4, SEEK_CUR);
				uvU = half_float::half_cast<float>(ReadHalfFloat(f, huvU));
				uvV = half_float::half_cast<float>(ReadHalfFloat(f, huvV));
				//uvU = ReadHalfFloat(f, uvU);
				//uvV = ReadHalfFloat(f, uvV);
				fseek(f, 108, SEEK_CUR);

				//vertices[i] = vertexX*fscale;
				//vertices[i+1] = vertexY * fscale;
				//vertices[i+2] = vertexZ * fscale;
				vertices.push_back(vertexX*fscale);
				vertices.push_back(vertexY*fscale);
				vertices.push_back(vertexZ*fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				//printf("Vertices 1: %f 2: %f 3:%f", vertexX, vertexY, vertexZ);
			}
			break;
		}
		}


		#pragma endregion
		
		#pragma region Collecting faces
		_fseeki64(f, faceOffset, SEEK_SET);
		unsigned short faceA = 0;
		unsigned short faceB = 0;
		unsigned short faceC = 0;
		

		if (faceSize == 1)
		{
			for (int i = 1; i <= (faceCount/3); i++)
			{
				faceA = ReadUShort(f, faceA);
				faceB = ReadUShort(f, faceB);
				faceC = ReadUShort(f, faceC);

				faces.push_back(faceA);
				faces.push_back(faceB);
				faces.push_back(faceC);
			}
		}
		else
		{
			for (int i = 1; i <= (faceCount/3); i++)
			{
				faceA = ReadULong(f, faceA);
				faceB = ReadULong(f, faceB);
				faceC = ReadULong(f, faceC);

				faces.push_back(faceA);
				faces.push_back(faceB);
				faces.push_back(faceC);
			}
		}

		#pragma endregion

	}
	#pragma endregion

	//#pragma region OutputToObj
	//int rememberV = 0;
	//int rememberN = 0;
	//int rememberU = 0;
	//int rememberI = 0;
	//int FIord = 1;
	//FILE* fObj;
	//
	//if (fopen_s(&fObj, MeshName, "w") != 0)				// Security check
	//{
	//	printf("error! cannot open file %s!\n", MeshName);
	//	exit(1);
	//}
	//for (int i = 1; i <= meshCount; i++)
	//{
	//	fprintf_s(fObj, "#\n# object %s_%d\n#\n\n", "mesh", i);	// File comments
	//	for (int j = 0; j < verticesCount[i]; j++)
	//	{
	//		fprintf_s(fObj, "v  %f %f %f\n", vertices[rememberV], vertices[rememberV + 1], vertices[rememberV + 2]); // Print positions as floats
	//		rememberV += 3;
	//	}
	//	fprintf_s(fObj, "# %d vertices\n\n", verticesCount[i]);
	//	for (int j = 0; j < verticesCount[i]; j++)
	//	{
	//		fprintf_s(fObj, "vn  %f %f %f\n", normals[rememberN], normals[rememberN + 1], normals[rememberN + 2]); // Print positions as floats
	//		rememberN += 3;
	//	}
	//	fprintf_s(fObj, "# %d vertex normals\n\n", verticesCount[i]);
	//	for (int j = 0; j < verticesCount[i]; j++)
	//	{
	//		fprintf_s(fObj, "vt  %f %f\n", uvs[rememberU], uvs[rememberU + 1]); // Print positions as floats
	//		rememberU+=2;
	//	}
	//	fprintf_s(fObj, "# %d texture coords\n\n", verticesCount[i]);		// Comment for UV count
	//	faceIndicesCount[i] /= 3;
	//	for (int j = 0; j < faceIndicesCount[i]; j++)
	//	{
	//		fprintf_s(fObj, "f %d/%d/%d %d/%d/%d %d/%d/%d \n",
	//			faces[rememberI] + FIord, faces[rememberI] + FIord, faces[rememberI] + FIord
	//			, faces[rememberI + 1] + FIord, faces[rememberI + 1] + FIord, faces[rememberI + 1] + FIord
	//			, faces[rememberI + 2] + FIord, faces[rememberI + 2] + FIord, faces[rememberI + 2] + FIord);	// v/vt/vn
	//		rememberI += 3;
	//	}
	//	FIord += verticesCount[i];
	//}
	#pragma endregion
	#pragma region OutputToFbx
	FbxManager* lSdkManager = FbxManager::Create();
	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	int sumVertices = 0;
	int Viord = 0;
	int index = 0;
	int rememberI = 0;
	int remeberU = 0;
	int Niord = 0;
	/*
	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Igz File\0*.igz\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a Texture File, yo!";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		std::cout << "You chose the file \"" << filename << "\"\n";
	}
	*/
	/*
	std::string folderpathMain = "";
	if (!GetFolder(folderpathMain, (char*)"Select output/game folder"))
	{
		printf("Nothing selected! Skipping texturing...");
		system("pause");
		return 0;
	}
	char filename[MAX_PATH];

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;  // If you have a window to center over, put its HANDLE here
	ofn.lpstrFilter = "Igz Texture File\0*.igz\0";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a Texture File, yo!";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;

	if (GetOpenFileNameA(&ofn))
	{
		std::cout << "You chose the file \"" << filename << "\"\n";
	}
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[FILENAME_MAX];
	char ext[_MAX_EXT];
	char filename2[FILENAME_MAX];
	_splitpath(filename, drive, dir, fname, ext);
	snprintf(filename2, sizeof filename2, "%s.png", fname);
	char command[MAX_PATH];
	snprintf(command, sizeof command, "%s\\DontTouch\\quickbms.exe -k %s\\DontTouch\\CrashNSaneTrilogy_IGZTex.bms %s %s\\textures\\", ExePath().c_str(), ExePath().c_str(), filename, folderpathMain.c_str());
	system(command);
	*/
	for (int i = 1; i <= meshCount; i++)
	{
		char meshNode[8];
		snprintf(meshNode, sizeof meshNode, "Mesh_%d", i);
		char mesh[8];
		snprintf(mesh, sizeof mesh, "Mesh_%d", i);
		FbxNode* lMeshNode = FbxNode::Create(lScene, meshNode);
		FbxMesh* lMesh = FbxMesh::Create(lScene, mesh);
		lMeshNode->SetNodeAttribute(lMesh);
		FbxNode *lRootNode = lScene->GetRootNode();
		FbxNode *lPatchNode = lScene->GetRootNode();
		lRootNode->AddChild(lMeshNode);
		lMesh->InitControlPoints(verticesCount[i]);
		FbxVector4* lControlPoints = lMesh->GetControlPoints();
		FbxLayer* lLayer = lMesh->GetLayer(0);
		if (lLayer == NULL) {
			lMesh->CreateLayer();
			lLayer = lMesh->GetLayer(0);
		}

		// Texturing bms extraction :)
		/*
		// A texture need to be connected to a property on the material,
		// so let's use the material (if it exists) or create a new one
		FbxSurfacePhong* lMaterial = NULL;
		//get the node of mesh, add material for it.
		FbxNode* lNode = lMesh->GetNode();
		if (lNode)
		{
			lMaterial = lNode->GetSrcObject<FbxSurfacePhong>(0);
			if (lMaterial == NULL)
			{
				FbxString lMaterialName = "toto";
				FbxString lShadingName = "Phong";
				FbxDouble3 lBlack(0.0, 0.0, 0.0);
				FbxDouble3 lRed(1.0, 0.0, 0.0);
				FbxDouble3 lDiffuseColor(0.75, 0.75, 0.0);
				lMaterial = FbxSurfacePhong::Create(lScene, lMaterialName.Buffer());

				// Generate primary and secondary colors.
				lMaterial->Emissive.Set(lBlack);
				lMaterial->Ambient.Set(lRed);
				lMaterial->AmbientFactor.Set(1.);
				// Add texture for diffuse channel
				lMaterial->Diffuse.Set(lDiffuseColor);
				lMaterial->DiffuseFactor.Set(1.);
				lMaterial->TransparencyFactor.Set(0.4);
				lMaterial->ShadingModel.Set(lShadingName);
				lMaterial->Shininess.Set(0.5);
				lMaterial->Specular.Set(lBlack);
				lMaterial->SpecularFactor.Set(0.3);

				lNode->AddMaterial(lMaterial);
			}
		}
		
		FbxFileTexture* lTexture = FbxFileTexture::Create(lScene, "Colormap");
		// Set texture properties.
		lTexture->SetFileName(filename); // Resource file is in current directory.
		lTexture->SetTextureUse(FbxTexture::eStandard);
		lTexture->SetMappingType(FbxTexture::eUV);
		lTexture->SetMaterialUse(FbxFileTexture::eModelMaterial);
		lTexture->SetSwapUV(false);
		lTexture->SetTranslation(0.0, 0.0);
		lTexture->SetScale(1.0, 1.0);
		lTexture->SetRotation(0.0, 0.0);

		// don't forget to connect the texture to the corresponding property of the material
		if (lMaterial)
			lMaterial->Ambient.ConnectSrcObject(lTexture);

		lMeshNode->AddMaterial(lMaterial);
		*/
		/*
		FbxString lRootName("Root");
		FbxSkeleton* lSkeletonRootAttribute = FbxSkeleton::Create(lScene, "RootAtt");
		lSkeletonRootAttribute->SetSkeletonType(FbxSkeleton::eRoot);
		FbxNode* lSkeletonRoot = FbxNode::Create(lScene, lRootName.Buffer());
		lSkeletonRoot->SetNodeAttribute(lSkeletonRootAttribute);
		lSkeletonRoot->LclTranslation.Set(FbxVector4(-1, 1, 0.0));
		lRootNode->AddChild(lSkeletonRoot);
		*/

		if (noNormals[i] == true)
		{
			FbxLayerElementUV* lLayerElementUV = FbxLayerElementUV::Create(lMesh, "");
			// Set its mapping mode to map each normal vector to each control point.
			lLayerElementUV->SetMappingMode(FbxLayerElement::eByControlPoint);
			// Set the reference mode of so that the n'th element of the normal array maps to the n'th
			// element of the control point array.
			lLayerElementUV->SetReferenceMode(FbxLayerElement::eDirect);
			for (int j = 0; j < verticesCount[i]; j++)
			{
				FbxVector4 vertex(vertices[Viord], vertices[Viord + 1], vertices[Viord + 2]);
				FbxVector2 uv(uvs[remeberU], uvs[remeberU + 1]);
				lLayerElementUV->GetDirectArray().Add(uv);
				lControlPoints[j] = vertex;
				Viord += 3;
				remeberU += 2;

			}
			for (int y = 0; y < faceIndicesCount[i] / 3; y++)
			{
				lMesh->BeginPolygon();
				lMesh->AddPolygon(faces[rememberI]);
				lMesh->AddPolygon(faces[rememberI + 1]);
				lMesh->AddPolygon(faces[rememberI + 2]);
				lMesh->EndPolygon();
				rememberI += 3;
			}
			lLayer->SetUVs(lLayerElementUV);
			lMeshNode->LclRotation.Set(FbxVector4(-90.0, -90.0, 0.0)); //Right rotation
		}
		else
		{
			FbxLayerElementNormal* lLayerElementNormal = FbxLayerElementNormal::Create(lMesh, "");
			FbxLayerElementUV* lLayerElementUV = FbxLayerElementUV::Create(lMesh, "");
			// Set its mapping mode to map each normal vector to each control point.
			lLayerElementNormal->SetMappingMode(FbxLayerElement::eByControlPoint);
			lLayerElementUV->SetMappingMode(FbxLayerElement::eByControlPoint);
			// Set the reference mode of so that the n'th element of the normal array maps to the n'th
			// element of the control point array.
			lLayerElementNormal->SetReferenceMode(FbxLayerElement::eDirect);
			lLayerElementUV->SetReferenceMode(FbxLayerElement::eDirect);
			for (int j = 0; j < verticesCount[i]; j++)
			{
				FbxVector4 vertex(vertices[Viord], vertices[Viord + 1], vertices[Viord + 2]);
				FbxVector4 normal(normals[Niord], normals[Niord + 1], normals[Niord + 2]);
				FbxVector2 uv(uvs[remeberU], uvs[remeberU + 1]);
				lLayerElementNormal->GetDirectArray().Add(normal);
				lLayerElementUV->GetDirectArray().Add(uv);
				lControlPoints[j] = vertex;
				Viord += 3;
				Niord += 3;
				remeberU += 2;

			}
			for (int y = 0; y < faceIndicesCount[i] / 3; y++)
			{
				lMesh->BeginPolygon();
				lMesh->AddPolygon(faces[rememberI]);
				lMesh->AddPolygon(faces[rememberI + 1]);
				lMesh->AddPolygon(faces[rememberI + 2]);
				lMesh->EndPolygon();
				rememberI += 3;
			}
			lLayer->SetNormals(lLayerElementNormal);
			lLayer->SetUVs(lLayerElementUV);
			lMeshNode->LclRotation.Set(FbxVector4(-90.0, -90.0, 0.0)); //Right rotation
		}

	}
	FbxExporter* lExporter = FbxExporter::Create(lSdkManager, "");
	bool lExportStatus = lExporter->Initialize(dir, -1, lSdkManager->GetIOSettings());
	if (!lExportStatus) {
		printf("Call to FbxExporter::Initialize() failed.\n");
		printf("Error returned: %s\n\n", lExporter->GetStatus().GetErrorString());
		system("pause");
		return 0;
	}
	lExporter->Export(lScene);
	lExporter->Destroy();
	#pragma endregion

	#pragma region Free
	free(MeshName);
	free(lFilename);
	fclose(f);
	#pragma endregion
	if (command == false)
	{
		system("pause");
	}
	return 0;
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
