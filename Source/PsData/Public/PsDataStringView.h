// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

namespace PsDataTools
{

namespace CRC32
{

constexpr unsigned int crc32_table[] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D};

constexpr unsigned int Compute(const char* Data, int32 Len, uint32 CRC)
{
	CRC = CRC ^ 0xFFFFFFFFU;
	for (int i = 0; i < Len; i++)
	{
		CRC = crc32_table[*Data ^ (CRC & 0xFF)] ^ (CRC >> 8);
		Data++;
	}
	CRC = CRC ^ 0xFFFFFFFFU;
	return CRC;
}

constexpr unsigned int Compute(const char* Data, uint32 CRC)
{
	CRC = CRC ^ 0xFFFFFFFFU;
	while (*Data)
	{
		CRC = crc32_table[*Data ^ (CRC & 0xFF)] ^ (CRC >> 8);
		Data++;
	}
	CRC = CRC ^ 0xFFFFFFFFU;
	return CRC;
}

constexpr unsigned int Compute(const TCHAR* Data, int32 Len, uint32 CRC)
{
	CRC = CRC ^ 0xFFFFFFFFU;
	for (int i = 0; i < Len; i++)
	{
		CRC = crc32_table[(*Data & 0xFF) ^ (CRC & 0xFF)] ^ (CRC >> 8);
		Data++;
	}
	CRC = CRC ^ 0xFFFFFFFFU;
	return CRC;
}

constexpr unsigned int Compute(const TCHAR* Data, uint32 CRC)
{
	CRC = CRC ^ 0xFFFFFFFFU;
	while (*Data)
	{
		CRC = crc32_table[(*Data & 0xFF) ^ (CRC & 0xFF)] ^ (CRC >> 8);
		Data++;
	}
	CRC = CRC ^ 0xFFFFFFFFU;
	return CRC;
}

} // namespace CRC32

namespace Utils
{
template <typename T>
constexpr T Clamp(T Value, T Min, T Max)
{
	return (Value < Min) ? Min : (Value > Max) ? Max
											   : Value;
}

template <typename T>
constexpr int32 GetSize(const T* Data)
{
	int32 i = 0;
	while (*Data++)
	{
		i++;
	}

	return i;
}

template <typename T>
constexpr T ToLowerCase(T C)
{
	const T Delta = 'Z' - 'z';
	if (C >= 'A' && C <= 'Z')
	{
		return C - Delta;
	}
	return C;
}

template <bool bIgnoreCase = false, typename T, typename K>
constexpr bool Equal(const T* A, const K* B)
{
	while (*A || *B)
	{
		if (bIgnoreCase)
		{
			if (ToLowerCase(*A) != ToLowerCase(*B))
			{
				return false;
			}

			++A;
			++B;
		}
		else
		{
			if (*A++ != *B++)
			{
				return false;
			}
		}
	}
	return true;
}

template <bool bIgnoreCase = false, typename T, typename K>
constexpr bool Equal(const T* A, int32 ASize, const K* B, int32 BSize)
{
	if (ASize != BSize)
	{
		return false;
	}

	while (ASize > 0)
	{
		if (bIgnoreCase)
		{
			if (ToLowerCase(*A) != ToLowerCase(*B))
			{
				return false;
			}

			++A;
			++B;
		}
		else
		{
			if (*A++ != *B++)
			{
				return false;
			}
		}

		--ASize;
	}

	return true;
}

template <bool bIgnoreCase = false, typename T, typename K>
constexpr int32 Find(const T* A, int32 ASize, const K* B, int32 BSize)
{
	int32 Index = 0;
	while (ASize >= BSize)
	{
		if (Equal<bIgnoreCase>(A++, BSize, B, BSize))
		{
			return Index;
		}

		++Index;
		--ASize;
	}

	return INDEX_NONE;
}

template <bool bIgnoreCase = false, typename T, typename K>
constexpr int32 Find(const T* A, int32 ASize, K C)
{
	if (bIgnoreCase)
	{
		C = ToLowerCase(C);
	}

	int32 Index = 0;
	while (Index < ASize)
	{
		if (bIgnoreCase)
		{
			if (ToLowerCase(A[Index]) == C)
			{
				return Index;
			}
		}
		else
		{
			if (A[Index] == C)
			{
				return Index;
			}
		}

		++Index;
	}

	return INDEX_NONE;
}
} // namespace Utils

template <typename T>
struct TDataStringView
{
	constexpr TDataStringView()
		: Source(nullptr)
		, Size(0)
	{
	}

	constexpr TDataStringView(const T* Data)
		: Source(Data)
		, Size(Utils::GetSize(Data))
	{
	}

	constexpr TDataStringView(const T* Data, int Len)
		: Source(Data)
		, Size(Len)
	{
	}

	template <bool bIgnoreCase = false, typename K>
	constexpr bool Equal(const TDataStringView<K>& Other) const
	{
		return Utils::Equal<bIgnoreCase>(Source, Size, Other.GetData(), Other.Len());
	}

	template <bool bIgnoreCase = false, typename K>
	constexpr bool Equal(const K* Other) const
	{
		return Utils::Equal<bIgnoreCase>(Source, Size, Other, Utils::GetSize(Other));
	}

	template <bool bIgnoreCase = false, typename K>
	constexpr int Find(const TDataStringView<K>& Other) const
	{
		return Utils::Find<bIgnoreCase>(Source, Size, Other.Source, Other.Size);
	}

	template <bool bIgnoreCase = false, typename K>
	constexpr int Find(const K* Other) const
	{
		return Utils::Find<bIgnoreCase>(Source, Size, Other, Utils::GetSize(Other));
	}

	constexpr int FindByChar(T Char) const
	{
		return Utils::Find(Source, Size, Char);
	}

	template <typename PredicateType>
	constexpr int FindByPredicate(PredicateType Predicate) const
	{
		for (int32 i = 0; i < Size; ++i)
		{
			const T Char = Source[i];
			if (Predicate(Char))
			{
				return i;
			}
		}
		return INDEX_NONE;
	}

	constexpr TDataStringView Left(int32 CharCount) const
	{
		return TDataStringView(Source, Utils::Clamp(CharCount, 0, Size));
	}

	constexpr void LeftInline(int32 CharCount)
	{
		*this = Left(CharCount);
	}

	constexpr TDataStringView LeftChop(int32 Position) const
	{
		return TDataStringView(Source, Utils::Clamp(Size - Position, 0, Size));
	}

	constexpr void LeftChopInline(int32 Position)
	{
		*this = LeftChop(Position);
	}

	constexpr TDataStringView Right(int32 CharCount) const
	{
		const int NewLen = Utils::Clamp(CharCount, 0, Size);
		return TDataStringView(Source + (Size - NewLen), NewLen);
	}

	constexpr void RightInline(int32 CharCount)
	{
		*this = Right(CharCount);
	}

	constexpr TDataStringView RightChop(int32 Position) const
	{
		const int NewLen = Utils::Clamp(Size - Position, 0, Size);
		return TDataStringView(Source + (Size - NewLen), NewLen);
	}

	constexpr void RightChopInline(int32 Position)
	{
		*this = RightChop(Position);
	}

	constexpr TDataStringView Mid(int32 Position, int32 CharCount) const
	{
		Position = Utils::Clamp(Position, 0, Size);
		CharCount = Utils::Clamp(CharCount, 0, Size - Position);
		return TDataStringView(Source + Position, CharCount);
	}

	constexpr void MidInline(int32 Position, int32 CharCount)
	{
		*this = Mid(Position, CharCount);
	}

	constexpr TDataStringView LeftByChar(T Char) const
	{
		const auto CharIndex = FindByChar(Char);
		if (CharIndex == INDEX_NONE)
		{
			return TDataStringView(Source, Size);
		}

		return Left(CharIndex);
	}

	constexpr void LeftByCharInline(T Char)
	{
		*this = LeftByChar(Char);
	}

	constexpr TDataStringView RightByChar(T Char) const
	{
		const auto CharIndex = FindByChar(Char);
		if (CharIndex == INDEX_NONE)
		{
			return TDataStringView();
		}

		return RightChop(CharIndex + 1);
	}

	constexpr void RightByCharInline(T Char)
	{
		*this = RightByChar(Char);
	}

	template <typename PredicateType>
	constexpr TDataStringView LeftByPredicate(PredicateType Predicate) const
	{
		const auto CharIndex = FindByPredicate(Predicate);
		if (CharIndex == INDEX_NONE)
		{
			return TDataStringView(Source, Size);
		}

		return Left(CharIndex);
	}

	template <typename PredicateType>
	constexpr void LeftByPredicateInline(PredicateType Predicate)
	{
		*this = LeftByPredicate(Predicate);
	}

	template <typename PredicateType>
	constexpr TDataStringView RightByPredicate(PredicateType Predicate) const
	{
		const auto CharIndex = FindByPredicate(Predicate);
		if (CharIndex == INDEX_NONE)
		{
			return TDataStringView();
		}

		return RightChop(CharIndex);
	}

	template <typename PredicateType>
	constexpr void RightByByPredicateInline(PredicateType Predicate)
	{
		*this = RightByPredicate(Predicate);
	}

	constexpr TDataStringView TrimLeft() const
	{
		int32 Count = 0;
		while (Size > Count && Source[Count] == ' ')
		{
			Count++;
		}
		return TDataStringView(Source + Count, Size - Count);
	}

	constexpr void TrimLeftInline()
	{
		*this = TrimLeft();
	}

	constexpr TDataStringView TrimRight() const
	{
		int32 Count = 0;
		while (Size > Count && Source[Size - Count - 1] == ' ')
		{
			Count++;
		}
		return TDataStringView(Source, Size - Count);
	}

	constexpr void TrimRightInline()
	{
		*this = TrimRight();
	}

	constexpr TDataStringView Trim() const
	{
		return TrimLeft().TrimRight();
	}

	constexpr void TrimInline()
	{
		*this = Trim();
	}

	constexpr TDataStringView TrimQuotes() const
	{
		if (Size >= 2)
		{
			auto Quote = Source[0];
			if (Quote == '\'' || Quote == '"')
			{
				if (Source[Size - 1] == Quote)
				{
					return TDataStringView(Source + 1, Size - 2);
				}
			}
		}

		return TDataStringView(Source, Size);
	}

	constexpr void TrimQuotesInline()
	{
		*this = TrimQuotes();
	}

	constexpr const T* GetData() const
	{
		return Source;
	}

	constexpr int32 Len() const
	{
		return Size;
	}

	constexpr int32 GetHash() const
	{
		return CRC32::Compute(Source, Size, 0);
	}

	constexpr bool IsEmpty() const
	{
		return Size == 0;
	}

	constexpr void operator=(const TDataStringView& Other)
	{
		Source = Other.Source;
		Size = Other.Size;
	}

	constexpr T operator[](int32 Index) const
	{
		return Source[Index];
	}

	template <typename K>
	constexpr bool operator==(const TDataStringView<K>& Other) const
	{
		return Equal(Other);
	}

	template <typename K>
	constexpr bool operator==(const K* Other) const
	{
		return Equal(Other);
	}

	template <typename K>
	constexpr bool operator!=(const TDataStringView<K>& Other) const
	{
		return !Equal(Other);
	}

	template <typename K>
	constexpr bool operator!=(const K* Other) const
	{
		return !Equal(Other);
	}

private:
	const T* Source;
	int Size;
};

using FDataStringViewChar = TDataStringView<char>;
using FDataStringViewTCHAR = TDataStringView<TCHAR>;

constexpr FDataStringViewChar ToStringView(const char* Data)
{
	return FDataStringViewChar(Data);
}

constexpr FDataStringViewTCHAR ToStringView(const TCHAR* Data)
{
	return FDataStringViewTCHAR(Data);
}

FORCEINLINE FDataStringViewTCHAR ToStringView(const FString& String)
{
	return FDataStringViewTCHAR(String.GetCharArray().GetData(), String.Len());
}

FORCEINLINE FString ToString(const FDataStringViewChar& StringView)
{
	return FString(StringView.Len(), StringView.GetData());
}

FORCEINLINE FString ToString(const FDataStringViewTCHAR& StringView)
{
	return FString(StringView.Len(), StringView.GetData());
}
} // namespace PsDataTools

inline bool operator==(const PsDataTools::FDataStringViewTCHAR& StringView, const FString& String)
{
	return StringView.Equal(PsDataTools::ToStringView(String));
}

inline bool operator==(const FString& String, const PsDataTools::FDataStringViewTCHAR& StringView)
{
	return StringView.Equal(PsDataTools::ToStringView(String));
}

inline uint32 GetTypeHash(const PsDataTools::FDataStringViewTCHAR& StringView)
{
	return GetTypeHash(PsDataTools::ToString(StringView));
}

template <typename T>
constexpr int32 GetStaticTypeHash(const PsDataTools::TDataStringView<T>& View)
{
	return View.GetHash();
}

template <typename T>
constexpr int32 GetStaticTypeHash(const T* View)
{
	return GetStaticTypeHash(PsDataTools::TDataStringView<T>(View));
}
