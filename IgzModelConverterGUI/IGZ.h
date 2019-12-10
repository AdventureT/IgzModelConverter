#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fbxsdk.h>
//#include <filesystem>
#include "umHalf.h"
#include "windows.h"
#include <vcclr.h>
#include <shellapi.h>
//#include <HavokApi.hpp> //For skeleton and animation
#include <msclr\marshal_cppstd.h>
//#include "EulerAngels.h"
//#include "MathHelper.h"
#include "Reader.h"

namespace IGZ
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Runtime::InteropServices;
	using namespace System::Collections::Generic;
	using namespace std;
	using namespace System::IO;

	//namespace fs = std::experimental::filesystem;
	using namespace msclr::interop;
	class IGZ : Reader
	{

	private:
		FILE* f;
		long igzVersion = 0;
		long* dataOffset = new long[4];
		long* dataCount = new long[4];
		long* dataSize = new long[4];
		long long subDataOffset = 0;
		long meshDataOffset = 0;
		long long boneDataOffset = 0;
		long long boneDataTableOffset = 0;
		long long boneTableOffset = 0;
		long long boneOffset = 0;
		long boneCount = 0;
		long long meshTableOffset = 0;
		long meshCount = 0;
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
		half huvU = 0.0F;
		half huvV = 0.0F;
		float uvU = 0;
		float uvV = 0;
		long long subFaceDataOffset = 0;
		const char* pChar = new char[256];
		char* MeshName = new char[256];
		int faceCount = 0;
		int faceSize = 0;
		unsigned long long faceOffset = 0;

		long chunkOffset = 0;
		long chunkTag;
		long chunkDataCount = 0;
		long chunkLength = 0;
		long chunkDataOffset = 0;
		long chunkEnd = 0;

		int matCount = 0;

		bool firstTime = false;

		long vertBlockLength = 0;
		long faceBlockLength = 0;

		long allVertFaceBlocks = 0;

		int* verticesCount;
		int* faceIndicesCount;
		bool* noNormals;

		std::vector<float> vertices;
		std::vector<float> normals;
		std::vector<float> uvs;
		std::vector<unsigned long> faces;

		char* fileName = new char[256];

		std::vector<std::string> tstrNameArray;
		std::vector<std::string> boneNames;
		std::vector<long> boneParents;
		std::vector<FbxAMatrix> matrices;

		enum game IGZgame;
		enum type IGZtype;

		bool _bones = false;
		//hkaSkeleton* m_skeleton;

		TCHAR  dir[1024] = { 0 };
	public:
		enum Endian
		{
			LITTLE,
			BIG
		};
		Endian endian;
		IGZ::IGZ::Endian getEndian();
		IGZ(int game, int type, char* fileName, Endian e); //Constructor
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
			vector<long long> idenifier;
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
		FbxScene* lScene;
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

		void storeGameAndType(int gameNumber, int typeNumber);
		void storeBoneData();
		void skipToMeshTableOffset();
		void readHeader();
		void readMeshOffsets();
		void readMeshData(int n);
		void FbxOutput();
		char* GetAnimPath(System::String^ path);
		int getDir(TCHAR* fullPath, TCHAR* dir);
		void ReadTSTR();
		void ReadTDEP();
		//FbxNode* addHavokBone(FbxNode* parent_node, unsigned int parent_index, vector<FbxNode*>& skeleton_bones, const hkaSkeleton* skeleton);
		//FbxNode* addHavokSkeleton(vector<FbxNode*>& skeleton_bones, const hkaSkeleton* skeleton);
};
}
