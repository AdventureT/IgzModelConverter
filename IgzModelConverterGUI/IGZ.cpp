#include "IGZ.h"
namespace IGZ {
	enum game
	{
		NST,
		CTR,
		SKYLANDERS
	};

	enum type
	{
		ACTOR,
		MODEL,
		SKY
	};

	IGZ::IGZ(int gameNumber, int typeNumber, char* filename)
	{
		fileName = filename;
		storeGameAndType(gameNumber, typeNumber);
		readHeader();
		//storeBoneData();
		skipToMeshTableOffset();
		readMeshOffsets();
		FbxOutput();
		fclose(f);
	}

	void IGZ::storeGameAndType(int gameNumber, int typeNumber)
	{
		switch (gameNumber)
		{
		case 1:
			IGZgame = NST;
			break;
		case 2:
			IGZgame = CTR;
			break;
		case 3:
			IGZgame = SKYLANDERS;
			break;
		default:
			throw gcnew System::Exception(gcnew System::String("Unknown Game"));
		}
		switch (typeNumber)
		{
		case 1:
			IGZtype = ACTOR;
			break;
		case 2:
			IGZtype = MODEL;
			break;
		case 3:
			IGZtype = SKY;
			break;
		default:
			throw gcnew System::Exception(gcnew System::String("Unknown Type"));
		}
	}
	
	void IGZ::storeBoneData()
	{
		if (IGZtype == ACTOR)
		{
			fseek(f, 32 + dataOffset[1], SEEK_SET);
			subDataOffset = ReadLongLong(f) + dataOffset[1];
			_fseeki64(f, subDataOffset, SEEK_SET);
			long InfoOffset = ReadLong(f) + dataOffset[1]; //8864
			fseek(f, 4, SEEK_CUR);
			long ModelInfoOffset = ReadLong(f) + dataOffset[1]; //9296
			fseek(f, 4, SEEK_CUR);
			fseek(f, ModelInfoOffset, SEEK_SET);
			fseek(f, 40, SEEK_CUR);

			boneDataOffset = ReadLongLong(f) + dataOffset[1];
			long modelDataOffset = ReadLong(f) + dataOffset[1];
			fseek(f, 4, SEEK_CUR);
			long UnknownOffset2 = ReadLong(f) + dataOffset[1]; //108704
			fseek(f, 4, SEEK_CUR);
			if (ReadLong(f) != 0)
			{
				throw gcnew System::Exception(gcnew System::String("The IGZ has the type model not actor!"));
			}
			fseek(f, 4, SEEK_CUR);
			float BoundMinX = ReadFloat(f); //-67,74
			float BoundMinY = ReadFloat(f); //-167,33
			float BoundMinZ = ReadFloat(f); //-77,25
			float BoundMaxX = ReadFloat(f); //85,40

			float BoundMaxY = ReadFloat(f); //167,33
			float BoundMaxZ = ReadFloat(f); //230,58

			fseek(f, 32, SEEK_CUR);

			long BoneMatrixSize = ReadLong(f); //12160
			fseek(f, 4, SEEK_CUR);

			long BoneMatrixStart = ReadLong(f) + dataOffset[1]; //20184
			fseek(f, 20, SEEK_CUR);

			long BoneCount = ReadLong(f); //191
			fseek(f, 4, SEEK_CUR);

			long BoneHeaderSize = ReadLong(f); //1528
			fseek(f, 4, SEEK_CUR);
			long BoneHeaderStart = ReadLong(f) + dataOffset[1]; //9480

			fseek(f, BoneHeaderStart, SEEK_SET);

			for (int i = 0; i < BoneCount; i++)
			{
				long BoneOffset = ReadLong(f) + dataOffset[1]; //11008
				fseek(f, 4, SEEK_CUR);
				long BoneRet = ftell(f); //9488
				fseek(f, BoneOffset, SEEK_SET);
				fseek(f, 16, SEEK_CUR);
				long BoneNameID = ReadLong(f); //20
				std::string BoneName = tstrNameArray[BoneNameID];
				fseek(f, 4, SEEK_CUR);
				long BoneParent = ReadLong(f);
				long BoneNum = ReadLong(f);
				float BoneTX = ReadFloat(f);
				float BoneTY = ReadFloat(f);
				float BoneTZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				boneNames.push_back(BoneName);
				boneParents.push_back(BoneParent);
				fseek(f, BoneRet, SEEK_SET);
			}
			fseek(f, BoneMatrixStart, SEEK_SET);
			for (int i = 0; i < BoneCount; i++)
			{
				float m11 = 0; float m12 = 0; float m13 = 0; float m14 = 0;
				float m21 = 0; float m22 = 0; float m23 = 0; float m24 = 0;
				float m31 = 0; float m32 = 0; float m33 = 0; float m34 = 0;
				float m41 = 0; float m42 = 0; float m43 = 0; float m44 = 0;
				if (i == 0)
				{
					m11 = 1; m12 = 0; m13 = 0; m14 = 0;
					m21 = 0; m22 = 1; m23 = 0; m24 = 0;
					m31 = 0; m32 = 0; m33 = 1; m34 = 0;
					m41 = 0; m42 = 0; m43 = 0; m44 = 1;
				}
				else
				{
					m11 = ReadFloat(f); m12 = ReadFloat(f); m13 = ReadFloat(f); m14 = ReadFloat(f);
					m21 = ReadFloat(f); m22 = ReadFloat(f); m23 = ReadFloat(f); m24 = ReadFloat(f);
					m31 = ReadFloat(f); m32 = ReadFloat(f); m33 = ReadFloat(f); m34 = ReadFloat(f);
					m41 = ReadFloat(f); m42 = ReadFloat(f); m43 = ReadFloat(f); m44 = ReadFloat(f);
				}
				FbxVector4 m1 = FbxVector4(m11, m12, m13, m14);
				FbxVector4 m2 = FbxVector4(m21, m22, m23, m24);
				FbxVector4 m3 = FbxVector4(m31, m32, m33, m34);
				FbxVector4 m4 = FbxVector4(m41, m42, m43, m44);
				FbxMatrix tfm;
				tfm.SetRow(0, m1);
				tfm.SetRow(1, m1);
				tfm.SetRow(2, m1);
				tfm.SetRow(3, m1);
				tfm.Inverse();
				matrices.push_back(tfm);
			}

		}
	}

	void IGZ::skipToMeshTableOffset()
	{
		fseek(f, dataOffset[1], SEEK_SET);
		fseek(f, 32, SEEK_CUR);
		meshDataOffset = ReadLong(f) + dataOffset[1];
		fseek(f, meshDataOffset, SEEK_SET);

		int marker = 0;
		long marker2 = 0;

		unsigned long Umarker = 0;

		int start = 0;
		int fin = 0;

		if (IGZgame == SKYLANDERS)
		{
			if (IGZtype == ACTOR)
			{
				do
				{
					Umarker = ReadULong(f);
				} while (Umarker != 0xFFFFFFFF);
				start = ftell(f);
				do
				{
					marker2 = ReadLong(f);
				} while (marker2 != 904786743);
				fin = ftell(f);
				fseek(f, meshDataOffset, SEEK_SET);
				do
				{
					do
					{
						Umarker = ReadUShort(f);
					} while (Umarker != 34816);
					marker = ReadLong(f) + dataOffset[1];
				} while (marker <= start || marker >= fin);
				fseek(f, -4, SEEK_CUR);
			}
			else
			{
				do
				{
					marker = ReadShort(f);
				} while (marker == 0);
				do
				{
					marker = ReadUShort(f);
				} while (marker != 34816);
				fseek(f, 4, SEEK_CUR);
				marker = ReadShort(f);
				if (marker != 34816)
				{
					fseek(f, 40, SEEK_CUR);
					marker = ReadShort(f);
					if (marker != 34816)
					{
						fseek(f, -48, SEEK_CUR);
					}
					fseek(f, 14, SEEK_CUR);
					marker = ReadShort(f);
					if (marker != 34816)
					{
						fseek(f, -16, SEEK_CUR);
					}
				}
			}

		}
		else
		{
			if (IGZtype == ACTOR)
			{
				do
				{
					Umarker = ReadULong(f);
				} while (Umarker != 0xFFFFFFFF);
				start = ftell(f);
				do
				{
					marker2 = ReadLong(f);
				} while (marker2 != 904786743);
				fin = ftell(f);
				fseek(f, meshDataOffset, SEEK_SET);
				do
				{
					do
					{
						Umarker = ReadUShort(f);
					} while (Umarker != 272);
					marker = ReadLong(f) + dataOffset[1];
				} while (marker <= start || marker >= fin);
				fseek(f, -4, SEEK_CUR);
			}
			else
			{
				do
				{
					marker = ReadShort(f);
				} while (marker == 0);
				do
				{
					marker = ReadShort(f);
				} while (marker != 272);
				fseek(f, 4, SEEK_CUR);
				marker = ReadShort(f);
				if (marker != 272)
				{
					fseek(f, 40, SEEK_CUR);
					marker = ReadShort(f);
					if (marker != 272)
					{
						fseek(f, -48, SEEK_CUR);
					}
					fseek(f, 14, SEEK_CUR);
					marker = ReadShort(f);
					if (marker != 272)
					{
						fseek(f, -16, SEEK_CUR);
					}
				}
			}
		}
	}

	void IGZ::readHeader()
	{
		if (fopen_s(&f, fileName, "rb") != 0) // Security check
		{
			throw gcnew System::Exception(gcnew System::String("Can't open the igz file"));
		}
		fseek(f, 4, SEEK_CUR);
		igzVersion = ReadLong(f);
		fseek(f, 8, SEEK_CUR);
		for (int i = 0; i < 4; i++)
		{
			dataCount[i] = ReadLong(f);
			fseek(f, 4, SEEK_CUR);
			dataOffset[i] = ReadLong(f);
			fseek(f, 4, SEEK_CUR);
		}
	}

	void IGZ::readMeshOffsets()
	{
		meshTableOffset = ReadLongLong(f) + dataOffset[1];
		meshCount = ReadLong(f);
		_fseeki64(f, meshTableOffset, SEEK_SET);
		long long* meshDataOffsetArr = new long long[(int)meshCount];
		for (int i = 0; i < meshCount; i++)
		{
			meshDataOffsetArr[i] = ReadLongLong(f) + dataOffset[1];
		}

		pChar = strrchr(fileName, 92);

		if (pChar == NULL)				// If no backslash is found
			pChar = fileName;			// Reset pChar
		else
			pChar++;	// Skip the last backslash
		char* lFilename = new char[256];
		char* exportPath = (fileName);
		getDir(exportPath, dir);
		strncpy_s(MeshName, 256, pChar, strlen(pChar) - 4);
		strncpy_s(lFilename, 256, pChar, strlen(pChar) - 4);
		strcat_s(lFilename, 256, ".fbx");
		strcat_s(MeshName, 256, ".obj");
		strcat_s(dir, 256, lFilename);

		verticesCount = new int[(int)meshCount];
		faceIndicesCount = new int[(int)meshCount];
		noNormals = new bool[(int)meshCount + 1];

		int seekForward = 0;


		switch (IGZgame)
		{
		case NST:
			for (int n = 0; n < meshCount; n++)
			{
				_fseeki64(f, meshDataOffsetArr[n], SEEK_SET);

				fseek(f, 0x48, SEEK_CUR);
				vertDataOffset = ReadLongLong(f) + dataOffset[1];
				faceDataOffset = ReadLongLong(f) + dataOffset[1];
				_fseeki64(f, vertDataOffset, SEEK_SET);
				fseek(f, 0x20, SEEK_CUR); //always 0x20
				switch (IGZtype)
				{
				case SKY:
					subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, subDataOffset, SEEK_SET);
					fseek(f, 16, SEEK_CUR);
					vertCount = ReadLong(f);
					verticesCount[n] = vertCount;
					fseek(f, 28, SEEK_CUR);
					vertOffset = ReadShort(f) + dataOffset[3];
					fseek(f, 6, SEEK_CUR);
					subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
					fseek(f, 16, SEEK_CUR);
					vertexStride = ReadLong(f);
					_fseeki64(f, faceDataOffset, SEEK_SET);
					fseek(f, 0x20, SEEK_CUR);
					subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, subFaceDataOffset, SEEK_SET);
					fseek(f, 16, SEEK_CUR);
					faceCount = ReadLong(f);
					faceIndicesCount[n] = faceCount;
					fseek(f, 28, SEEK_CUR);
					faceOffset = ReadShort(f) + dataOffset[3];
					fseek(f, 6, SEEK_CUR);
					faceSize = ReadLong(f);
					_fseeki64(f, vertOffset, SEEK_SET);
					readMeshData(n);
					break;
				default:
					subVertDataOffset = ReadLongLong(f) + dataOffset[1];
					_fseeki64(f, subVertDataOffset, SEEK_SET);
					fseek(f, 16, SEEK_CUR);
					vertCount = ReadLong(f);
					verticesCount[n] = vertCount;
					fseek(f, 28, SEEK_CUR);
					vertOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					subVertDataOffset3 = ReadULongLong(f) + dataOffset[1];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
					fseek(f, 16, SEEK_CUR);
					vertexStride = ReadLong(f);
					_fseeki64(f, faceDataOffset, SEEK_SET);
					fseek(f, 0x20, SEEK_CUR);
					subFaceDataOffset = ReadLongLong(f) + dataOffset[1];
					_fseeki64(f, subFaceDataOffset, SEEK_SET);
					fseek(f, 16, SEEK_CUR);
					faceCount = ReadLong(f);
					faceIndicesCount[n] = faceCount;
					fseek(f, 28, SEEK_CUR);
					faceOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, vertOffset, SEEK_SET);
					readMeshData(n);
					break;
				}
			}
			break;
		case CTR:
			for (int n = 0; n < meshCount; n++)
			{
				_fseeki64(f, meshDataOffsetArr[n], SEEK_SET);

				fseek(f, 0x48, SEEK_CUR);
				vertDataOffset = ReadLongLong(f) + dataOffset[1];
				faceDataOffset = ReadLongLong(f) + dataOffset[1];
				_fseeki64(f, vertDataOffset, SEEK_SET);
				fseek(f, 0x20, SEEK_CUR); //always 0x20
				switch (IGZtype)
				{
				case SKY:
					subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, subDataOffset, SEEK_SET);
					fseek(f, 12, SEEK_CUR);
					vertCount = ReadLong(f);
					verticesCount[n] = vertCount;
					fseek(f, 24, SEEK_CUR);
					vertOffset = ReadShort(f) + dataOffset[3];
					fseek(f, 6, SEEK_CUR);
					subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
					fseek(f, 12, SEEK_CUR);
					vertexStride = ReadLong(f);
					_fseeki64(f, faceDataOffset, SEEK_SET);
					fseek(f, 0x20, SEEK_CUR);
					subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, subFaceDataOffset, SEEK_SET);
					fseek(f, 12, SEEK_CUR);
					faceCount = ReadLong(f);
					faceIndicesCount[n] = faceCount;
					fseek(f, 24, SEEK_CUR);
					if (ReadULongLong(f) >= 0x10000000) //This fixes T255_Skybox_Boreal
					{
						fseek(f, -8, SEEK_CUR);
						faceOffset = (ReadULongLong(f) - 0x10000000) + dataOffset[3];
					}
					else
					{
						faceOffset = ReadShort(f) + dataOffset[3];
					}
					fseek(f, 6, SEEK_CUR);
					faceSize = ReadLong(f);
					_fseeki64(f, vertOffset, SEEK_SET);
					readMeshData(n);
					break;
				default:
					subDataOffset = ReadLongLong(f) + dataOffset[1];
					_fseeki64(f, subDataOffset, SEEK_SET);
					fseek(f, 12, SEEK_CUR);
					vertCount = ReadLong(f);
					verticesCount[n] = vertCount;
					fseek(f, 24, SEEK_CUR);
					vertOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					subVertDataOffset3 = ReadULongLong(f) + dataOffset[1];
					_fseeki64(f, subVertDataOffset3, SEEK_SET);
					fseek(f, 12, SEEK_CUR);
					vertexStride = ReadLong(f);
					_fseeki64(f, faceDataOffset, SEEK_SET);
					fseek(f, 0x20, SEEK_CUR);
					subFaceDataOffset = ReadLongLong(f) + dataOffset[1];
					_fseeki64(f, subFaceDataOffset, SEEK_SET);
					fseek(f, 12, SEEK_CUR);
					faceCount = ReadLong(f);
					faceIndicesCount[n] = faceCount;
					fseek(f, 24, SEEK_CUR);
					faceOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
					_fseeki64(f, vertOffset, SEEK_SET);
					readMeshData(n);
					break;
				}
			}
			break;
		case SKYLANDERS:
			/*
			Important! Somehow Skylanders don't have any vertOffsets or faceOffsets
			That's because why I have to do vertStride * vertCount to get the length of 1 vertex data section
			Same like that with face data
			then just add the length with dataOffset[4]
			Sky also works when you select "model"
			*/
			for (int n = 0; n < meshCount; n++)
			{
				seekForward = 0;
				_fseeki64(f, meshDataOffsetArr[n], SEEK_SET);

				fseek(f, 0x48, SEEK_CUR);
				vertDataOffset = ReadLongLong(f) + dataOffset[1];
				faceDataOffset = ReadLongLong(f) + dataOffset[1];
				_fseeki64(f, vertDataOffset, SEEK_SET);
				fseek(f, 0x20, SEEK_CUR); //always 0x20

				subDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
				_fseeki64(f, subDataOffset, SEEK_SET);
				fseek(f, 12, SEEK_CUR);
				vertCount = ReadLong(f);
				verticesCount[n] = vertCount;
				fseek(f, 24, SEEK_CUR);
				subVertDataOffset3 = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
				_fseeki64(f, subVertDataOffset3, SEEK_SET);
				fseek(f, 12, SEEK_CUR);
				vertexStride = ReadLong(f);
				if (allVertFaceBlocks == 0)
				{
					vertOffset = dataOffset[3];
					vertBlockLength = vertCount * vertexStride;
					allVertFaceBlocks += vertBlockLength;
				}
				else
				{
					vertOffset = allVertFaceBlocks + dataOffset[3];
					short blank = 0;
					int current = 0;
					bool _break = false;
					unsigned short faceCheck1 = 0;
					unsigned short faceCheck2 = 0;

					fseek(f, 0, SEEK_END); // seek to end of file
					int size = ftell(f); // get current file pointer
					fseek(f, 0, SEEK_SET); // seek back to beginning of file

					current = ftell(f);
					_fseeki64(f, vertOffset, SEEK_SET);
					vertBlockLength = vertCount * vertexStride;
					if (vertBlockLength + vertOffset < size)
					{
						fseek(f, vertBlockLength, SEEK_CUR);
						do
						{
							faceCheck1 = ReadShort(f);
							faceCheck2 = ReadShort(f);
							if (faceCheck1 == 0 && faceCheck2 == 1) //Unsafe
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
				fseek(f, 0x20, SEEK_CUR);
				subFaceDataOffset = (ReadULongLong(f) - 0x8000000) + dataOffset[2];
				_fseeki64(f, subFaceDataOffset, SEEK_SET);
				fseek(f, 8, SEEK_CUR);
				faceSize = ReadLong(f);
				faceCount = ReadLong(f);
				faceIndicesCount[n] = faceCount;
				faceOffset = (allVertFaceBlocks + dataOffset[3]) + seekForward;
				faceBlockLength = faceCount * 2;
				allVertFaceBlocks += faceBlockLength;
				_fseeki64(f, vertOffset, SEEK_SET);
				readMeshData(n);
			}
			break;
		}
	}

	void IGZ::readMeshData(int n)
	{
		switch (vertexStride)
		{
		case 0x10:
		{
			noNormals[n] = true;
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x14:
		{
			noNormals[n] = true;
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x18:
		{
			noNormals[n] = true;
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 8, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x20:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 4, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x24:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 8, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x28:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 12, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x2C:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 12, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x30:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 20, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x34:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 20, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
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
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
				fseek(f, 28, SEEK_CUR);
			}
			break;
		}
		case 0x3c:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 32, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x40:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 32, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x44:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 40, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x48:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 44, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x4c:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 44, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x50:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 52, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x54:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 52, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x5c:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 64, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x60:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 64, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x88:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 108, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		case 0x8C:
		{
			for (int i = 0; i < vertCount; i++)
			{
				vertexX = ReadFloat(f);
				vertexY = ReadFloat(f);
				vertexZ = ReadFloat(f);
				normalX = ReadFloat(f);
				normalY = ReadFloat(f);
				normalZ = ReadFloat(f);
				fseek(f, 4, SEEK_CUR);
				uvU = ReadHalfFloat(f);
				uvV = ReadHalfFloat(f);
				fseek(f, 108, SEEK_CUR);

				vertices.push_back(vertexX * fscale);
				vertices.push_back(vertexY * fscale);
				vertices.push_back(vertexZ * fscale);
				normals.push_back(normalX);
				normals.push_back(normalY);
				normals.push_back(normalZ);
				uvs.push_back(uvU);
				uvs.push_back(1 - uvV);
			}
			break;
		}
		}


#pragma endregion

#pragma region Collecting faces
		_fseeki64(f, faceOffset, SEEK_SET);
		unsigned long faceA = 0;
		unsigned long faceB = 0;
		unsigned long faceC = 0;

		if (vertCount <= 65535)
		{
			for (int i = 0; i < (faceCount / 3); i++)
			{
				faceA = ReadUShort(f);
				faceB = ReadUShort(f);
				faceC = ReadUShort(f);

				faces.push_back(faceA);
				faces.push_back(faceB);
				faces.push_back(faceC);
			}
		}
		else
		{
			for (int i = 0; i < (faceCount / 3); i++)
			{
				faceA = ReadULong(f);
				faceB = ReadULong(f);
				faceC = ReadULong(f);

				faces.push_back(faceA);
				faces.push_back(faceB);
				faces.push_back(faceC);
			}
			ftell(f);
		}
	}

	void IGZ::FbxOutput()
	{
		if (meshCount == 0)
		{
			throw gcnew System::Exception(gcnew System::String("The Igz doesn't contain any mesh data"));
		}
		FbxManager* lSdkManager = FbxManager::Create();
		FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
		FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
		//FbxNode* lSkeletonRoot = FbxNode::Create(lScene, "Root");
		/*
		FbxNode* lRootNode = lScene->GetRootNode();
		FbxNode* lPatchNode = lScene->GetRootNode();
		FbxPatch* lPatch = FbxPatch::Create(lScene, "");

		lPatchNode->SetNodeAttribute(lPatch);
				*/
		lSdkManager->SetIOSettings(ios);
		int sumVertices = 0;
		int Viord = 0;
		int index = 0;
		int rememberI = 0;
		int remeberU = 0;
		int Niord = 0;
		if (IGZtype == ACTOR && _bones == true)
		{
			IhkPackFile* hdr = IhkPackFile::Create(GetAnimPath(gcnew String(fileName)));
			if (!hdr)
			{
				throw gcnew System::Exception(gcnew System::String("Can't open the hkx file"));
			}
			hkRootLevelContainer* rcont = hdr->GetRootLevelContainer();

			if (!rcont)
			{
				throw gcnew System::Exception(gcnew System::String("hkRootLevelContainer is null or empty"));
			}

			const hkaAnimationContainer* aniCont = rcont->GetVariant(0);
			const hkaAnimation* anim = aniCont->GetAnimation(0);
			for (int cAnnot = 0; cAnnot < anim->GetNumAnnotations(); cAnnot++)
			{
				boneNames.push_back(anim->GetAnnotation(cAnnot).get()->GetName());
			}
			std::string s;
			int i = 0;
			for (std::string& piece : boneNames)
			{
				if (i == 9)
				{
					s += "\n";
				}
				else
				{
					s += piece + "\t";
				}
				i++;
			}
			/*
			m_skeleton = (hkaSkeleton*)aniCont->GetSkeleton(1);
			lSkeletonRoot = CreateSkeleton(lScene, "Skeleton");
			lSkeletonRoot->PreRotation.Set(FbxVector4(-90, -90, 0));
			lRootNode->AddChild(lSkeletonRoot);
			*/
		}
		// Build the node tree.
		/*
		int currentBone = 0;
		const hkaSkeleton* skel = aniCont->GetSkeleton(1);
		std::vector<FbxNode*> fbxNodes;
		for (auto b : skel->FullBones())
		{
			std::string bName = b.name;
			FbxNode* node = FbxNode::Create(lScene, bName.c_str());
			bool found = false;
			for (int i = 0; i < fbxNodes.size(); i++)
			{
				if (!fbxNodes[i])
				{
					found = false;
					break;
				}
				std::string name = fbxNodes[i]->GetName();
				if (name == bName)
				{
					node = fbxNodes[i];
					found = true;
				}
			}
			if (!found)
			{
				node = FbxNode::Create(lScene, bName.c_str());
			}


			if (b.transform)
			{
				FbxAMatrix nodeTM = {};
				FbxAMatrix nodeTM2 = {};
				FbxQuaternion quat(b.transform->rotation.X, b.transform->rotation.Y, b.transform->rotation.Z);
				quat.Conjugate();
				nodeTM.SetR(quat.DecomposeSphericalXYZ());
				nodeTM.SetT(FbxVector4(b.transform->position.X, b.transform->position.Y, b.transform->position.Z));

				if (b.parentID > -1)
				{
					nodeTM2 = {};
					fbxNodes[b.parentID]->AddChild(node);
					nodeTM2.SetT(fbxNodes[b.parentID]->LclTranslation.Get());
					nodeTM2.SetR(fbxNodes[b.parentID]->LclRotation.Get());
					nodeTM *= nodeTM2;
				}
				else
					nodeTM *= lRootNode->EvaluateLocalTransform();
				node->LclRotation.Set(nodeTM.GetR());
				node->LclTranslation.Set(nodeTM.GetT());
			}
			if (currentBone == 0)
			{
				lRootNode->AddChild(node);
			}
			fbxNodes.push_back(node);
			currentBone++;
							}
		*/

		for (int i = 0; i < meshCount; i++)
		{
			char meshNode[8];
			snprintf(meshNode, sizeof meshNode, "Mesh_%d", i);
			char mesh[8];
			snprintf(mesh, sizeof mesh, "Mesh_%d", i);
			FbxNode* lMeshNode = FbxNode::Create(lScene, meshNode);
			FbxMesh* lMesh = FbxMesh::Create(lScene, mesh);
			lMeshNode->SetNodeAttribute(lMesh);
			FbxNode* lRootNode = lScene->GetRootNode();
			FbxNode* lPatchNode = lScene->GetRootNode();
			lRootNode->AddChild(lMeshNode);
			lMesh->InitControlPoints(verticesCount[i]);
			FbxVector4* lControlPoints = lMesh->GetControlPoints();
			FbxLayer* lLayer = lMesh->GetLayer(0);
			if (lLayer == NULL) {
				lMesh->CreateLayer();
				lLayer = lMesh->GetLayer(0);
			}
			
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
				for (int y = 0; y < (faceIndicesCount[i] / 3); y++)
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
				for (int y = 0; y < (faceIndicesCount[i] / 3); y++)
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
			throw gcnew System::Exception(gcnew System::String("Call to FbxExporter::Initialize() failed."));
		}
		lExporter->Export(lScene);
		lExporter->Destroy();
	}

	int IGZ::getDir(TCHAR* fullPath, TCHAR* dir)
	{
		const int buffSize = 1024;

		TCHAR buff[buffSize] = { 0 };
		int buffCounter = 0;
		int dirSymbolCounter = 0;

		for (unsigned int i = 0; i < strlen(fullPath); i++) {
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

	char* IGZ::GetAnimPath(System::String^ path)
	{
		std::string curPath = msclr::interop::marshal_as<std::string>(path);
		std::vector<std::string> subdirs;
		std::string base_filename;
		std::string root;
		do
		{
			root = curPath.substr(0, curPath.find_last_of("/\\"));
			base_filename = curPath.substr(curPath.find_last_of("/\\") + 1);
			if (base_filename == "actors")
			{
				break;
			}
			subdirs.push_back(base_filename);
			curPath = curPath.substr(0, curPath.find_last_of("/\\"));
		} while (base_filename != "actors");
		std::string hkxPath = root;
		hkxPath.append("\\anims");
		std::reverse(subdirs.begin(), subdirs.end());
		for (size_t i = 0; i < subdirs.size(); i++)
		{
			hkxPath.append("\\" + subdirs[i]);
		}
		std::string::size_type i = hkxPath.rfind('.', hkxPath.length());
		std::string newExt = "hka";
		if (i != std::string::npos) {
			hkxPath.replace(i + 1, newExt.length(), newExt);
		}
		System::String^ test2 = gcnew String(hkxPath.c_str());
		return (char*)(void*)Marshal::StringToHGlobalAnsi(test2);
	}

}
