/*	Binary data reader class,
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

#include "bincore.hpp"

class _BinCoreIn
{
public:
	typedef std::istream StreamType;
protected:
	_BinCoreIn() : localseek(0), BaseStream(nullptr){}
	size_t localseek;
	StreamType *BaseStream;
	static const std::ios_base::openmode MODE = std::ios::binary | std::ios::in;
public:
	ES_FORCEINLINE const size_t Tell(const bool relative = true) const
	{
		return static_cast<size_t>(BaseStream->tellg()) - (relative ? localseek : 0);
	}

	ES_FORCEINLINE void Seek(const size_t position, const std::ios_base::seekdir vay = std::ios_base::beg, const bool relative = true) const
	{
		BaseStream->seekg(position + ((relative && !vay) ? localseek : 0), vay);
	}

	ES_FORCEINLINE void Skip(const size_t length) const { Seek(length, std::ios_base::cur); }

	template<typename T> ES_FORCEINLINE void Skip() const { Seek(sizeof(T), std::ios_base::cur); }
};

class BinReader : public _BinCore<_BinCoreIn>
{
	size_t savepos;

	BinReader() : savepos(0) {}

public:

	template<class T> BinReader(const UniString<T> &filePath) : BinReader() { _Open(filePath.c_str()); }
	BinReader(StreamType &instream) : BinReader() { SetStream(instream); }
	BinReader(const char *filePath) : BinReader() { _Open(filePath); }
	BinReader(const wchar_t *filePath) : BinReader() { _Open(filePath); }

	ES_FORCEINLINE size_t SavePos() { return savepos = Tell(); }
	ES_FORCEINLINE void RestorePos() { Seek(savepos); }

	ES_FORCEINLINE void ReadBuffer(char *buffer, size_t size) 
	{ 
		BaseStream->read(buffer, size); 

#ifdef ES_ENCRYPTION_DEFINED
		if (enc && enc->Valid())
			enc->Encode(buffer, size);
#endif
	}
#pragma warning(push)
#pragma warning(disable: 4100)
	// Will read any std container using std::allocator class, eg. vector, basic_string, etc..
	// swapType : will force not to swap endianess, when used with class that does not have SwapEndian method or is not defined for structural swap
	template<
		class _containerClass,
		class T = typename _containerClass::value_type
	> const void ReadContainer(_containerClass &input, size_t numitems, _e_swapEndian) const
	{
		input.resize(numitems);

		if (!numitems)
			return;

		const size_t elesize = sizeof(T);
		const size_t arrsize = elesize*numitems;
		BaseStream->read(reinterpret_cast<char*>(const_cast<T*>(input.begin().operator->())), arrsize);

#ifdef ES_ENCRYPTION_DEFINED
		if (enc && enc->Valid())
			enc->Encode(reinterpret_cast<char*>(const_cast<T*>(input.begin().operator->())), arrsize);
#endif

#ifdef ES_ENDIAN_DEFINED
		if (swapEndian && swapType && elesize > 1)
			for (T &a : input)
				FByteswapper(a);
#endif
	}

	// Will read any std container using std::allocator class, eg. vector, basic_string, etc..
	// Will read number of items first
	// swapType : will force not to swap endianess, when used with class that does not have SwapEndian method or is not defined for structural swap
	template<class _countType = int, class _containerClass> const void ReadContainer(_containerClass &input, EndianSwap swapType = SWAP) const
	{
		_countType numElements;
		Read(numElements);
		ReadContainer(input, numElements, swapType);
	}

	// Will read buffer until 0
	template<class _containerClass> const void ReadString(_containerClass &input) const
	{
		typename _containerClass::value_type tmp;
		while ((Read(tmp),tmp) != 0 && !BaseStream->eof())
			input.push_back(tmp);
	}

	// swapType : will force not to swap endianess, when used with class that does not have SwapEndian method or is not defined for structural swap
	template<
		typename T
	> ES_FORCEINLINE void Read(T &value, const size_t size = sizeof(T), _e_swapEndian) const
	{
		BaseStream->read(reinterpret_cast<char*>(&value), size);

#ifdef ES_ENCRYPTION_DEFINED
		if (enc && enc->Valid())
			enc->Encode(reinterpret_cast<char*>(&value), size);
#endif

#ifdef ES_ENDIAN_DEFINED
		if (swapEndian && swapType && size > 1)
			FByteswapper(value);
#endif
	}
#pragma warning(pop)
};