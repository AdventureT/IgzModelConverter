#include "IGZTEX.h"

IGZTEX::IGZTEX::IGZTEX(char* fileName,int e)
{
	endian = e;
	fileNameTex = fileName;
	readTextureFile();
}

void IGZTEX::IGZTEX::readHeader()
{
	if (fopen_s(&f, fileNameTex, "rb") != 0) // Security check
	{
		throw gcnew System::Exception(gcnew System::String("Can't open the igz file"));
	}
	fseek(f, 4, SEEK_CUR);
	igzVersion = ReadLong(f, endian);
	fseek(f, 8, SEEK_CUR);
	for (int i = 0; i < 4; i++)
	{
		dataCount[i] = ReadLong(f, endian);
		fseek(f, 4, SEEK_CUR);
		dataOffset[i] = ReadLong(f, endian);
		dataSize[i] = ReadLong(f, endian);
	}
}

void IGZTEX::IGZTEX::readTextureFile()
{
	readHeader();
	fseek(f, dataOffset[0], SEEK_SET);
	readTags();
	fseek(f, 72, SEEK_CUR);
	short width = ReadShort(f, endian);
	short height = ReadShort(f, endian);
	fseek(f, 28, SEEK_CUR);
	long fileSize = ReadLong(f, endian);
	fseek(f, 84, SEEK_CUR);
	std::string texturePath = fileNameTex;
	std::string folder = texturePath.substr(0, texturePath.find_last_of("/\\"));
	std::string::size_type i = texturePath.rfind('.', texturePath.length());
	std::string newExt = "dds";
	if (i != std::string::npos) {
		texturePath.replace(i + 1, newExt.length(), newExt);
	}
	FILE* texF;
	if (fopen_s(&texF, texturePath.c_str(), "wb") != 0) // Security check
	{
		throw gcnew System::Exception(gcnew System::String("Can't open the igz file"));
	}
	byte ddsHeader[12] = { 68,68,83,32,124,00,00,00,7,16,8,00 };
	byte zeros[52];
	for (int i = 0; i < 52; i++)
	{
		zeros[i] = 00;
	}
	fwrite(&ddsHeader, 12, 1, texF);
	fwrite(&height, 4, 1, texF);
	fwrite(&width, 4, 1, texF);
	fwrite(&fileSize, 4, 1, texF);
	fwrite(&zeros, 52, 1, texF);
	byte format[52] = { 32,00,00,00,04,00,00,00,68,88,84,49,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,16,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00 };
	switch (exid.idenifier[0])
	{
		case 0x9D3B06CD: //DDS (PC) DXT1
			fwrite(&format, 52, 1, texF);
			break;
		case 0x398888DA: //DDS (PC) DXT5
			format[11] = 53;
			fwrite(&format, 52, 1, texF);
			break;
		case 0x78B94718: //DDS (PC) ATI2
			format[8] = 65;
			format[9] = 84;
			format[10] = 73;
			format[11] = 50;
			fwrite(&format, 52, 1, texF);
			break;
		case 0xDE084699: //DDS (PC) RGBA32
			format[4] = 65;
			format[8] = 00;
			format[9] = 00;
			format[10] = 00;
			format[11] = 32;
			format[15] = 255;
			format[20] = 255;
			format[25] = 255;
			format[30] = 255;
			format[31] = 8;
			format[32] = 16;
			format[33] = 64;
			//byte format3[52] = { 32,00,00,00,65,00,00,00,00,00,00,32,00,00,00,255,00,00,00,00,255,00,00,00,00,255,00,00,00,00,255,8,16,64,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00 };
			fwrite(&format, 52, 1, texF);
			break;
		case 0x1B282851: //DDS (Switch) DXT1
			fwrite(&format, 52, 1, texF);
			break;
		case 0x37456ECD: //DDS (Switch) DXT5
			format[11] = 53;
			fwrite(&format, 52, 1, texF);
			break;
		case 0xD0124568: //DDS (Switch) ATI2
			format[8] = 65;
			format[9] = 84;
			format[10] = 73;
			format[11] = 50;
			fwrite(&format, 52, 1, texF);
			break;
		case 0x8EBE8CF2: //DDS (Switch) RGBA32
			format[4] = 65;
			format[8] = 00;
			format[9] = 00;
			format[10] = 00;
			format[11] = 32;
			format[15] = 255;
			format[20] = 255;
			format[25] = 255;
			format[30] = 255;
			format[31] = 8;
			format[32] = 16;
			format[33] = 64;
			fwrite(&format, 52, 1, texF);
			break;
	}
	for (int i = 0; i < fileSize; i++)
	{
		byte b = ReadUByte(f, endian);
		fwrite(&b, 1, 1, texF);
	}
	fclose(f);
	fclose(texF);
}

void IGZTEX::IGZTEX::readTags()
{
	for (int i = 0; i < dataCount[0]; i++)
	{
		long tagOffset = ftell(f);
		std::string tag = ReadString(f,4);
		if (tag == "TSTR")
		{
			tstr.count = ReadLong(f, endian);
			tstr.size = ReadLong(f, endian);
			tstr.start = ReadLong(f, endian);
			fseek(f, tstr.start + tagOffset, SEEK_SET);
			std::string test;
			for (int i = 0; i < tstr.count; i++)
			{
				tstr.text.push_back(ReadString(f));
				do
				{
					test = ReadString(f,1);
					if (test[0] != '\0')
					{
						fseek(f, -1, SEEK_CUR);
					}
				} while (test[0] == '\0');
			}
		}
		else if (tag == "TMET")
		{
			tmet.count = ReadLong(f, endian);
			tmet.size = ReadLong(f, endian);
			tmet.start = ReadLong(f, endian);
			fseek(f, tmet.start + tagOffset, SEEK_SET);
			std::string test;
			for (int i = 0; i < tmet.count; i++)
			{
				tmet.text.push_back(ReadString(f));
				do
				{
					test = ReadString(f, 1);
					if (test[0] != '\0')
					{
						fseek(f, -1, SEEK_CUR);
					}
				} while (test[0] == '\0');
			}
		}
		else if (tag == "MTSZ")
		{
			mtsz.count = ReadLong(f, endian);
			mtsz.size = ReadLong(f, endian);
			mtsz.start = ReadLong(f, endian);
			fseek(f, mtsz.start + tagOffset, SEEK_SET);
			for (int i = 0; i < mtsz.count; i++)
			{
				mtsz.unknown.push_back(ReadLong(f, endian));
			}
		}
		else if (tag == "EXID")
		{
			exid.count = ReadLong(f, endian);
			exid.size = ReadLong(f, endian);
			exid.start = ReadLong(f, endian);
			fseek(f, exid.start + tagOffset, SEEK_SET);
			for (int i = 0; i < exid.count; i++)
			{
				exid.idenifier.push_back(ReadLong(f, endian));
				fseek(f, 4, SEEK_CUR);
			}
		}
		else if (tag == "RVTB")
		{
			rvtb.count = ReadLong(f, endian);
			rvtb.size = ReadLong(f, endian);
			rvtb.start = ReadLong(f, endian);
			fseek(f, rvtb.size + tagOffset, SEEK_SET);
		}
		else if (tag == "RSTT")
		{
			rstt.count = ReadLong(f, endian);
			rstt.size = ReadLong(f, endian);
			rstt.start = ReadLong(f, endian);
			fseek(f, rstt.size + tagOffset, SEEK_SET);
		}
		else if (tag == "ROFS")
		{
			rofs.count = ReadLong(f, endian);
			rofs.size = ReadLong(f, endian);
			rofs.start = ReadLong(f, endian);
			fseek(f, rofs.size + tagOffset, SEEK_SET);
		}
		else if (tag == "REXT")
		{
			rext.count = ReadLong(f, endian);
			rext.size = ReadLong(f, endian);
			rext.start = ReadLong(f, endian);
			fseek(f, rext.size + tagOffset, SEEK_SET);
		}
		else if (tag == "ROOT")
		{
			root.count = ReadLong(f, endian);
			root.size = ReadLong(f, endian);
			root.start = ReadLong(f, endian);
			fseek(f, root.size + tagOffset, SEEK_SET);
		}
		else if (tag == "ONAM")
		{
			onam.count = ReadLong(f, endian);
			onam.size = ReadLong(f, endian);
			onam.start = ReadLong(f, endian);
			fseek(f, onam.size + tagOffset, SEEK_SET);
		}
	}
}


