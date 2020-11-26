// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "PsDataBigInteger.generated.h"

using PsDataBigIntegerWordType = uint32;

UENUM(BlueprintType, Blueprintable)
enum class EPsDataBigIntegerConvertionType : uint8
{
	Default = 0,
	Bin = 2,
	Oct = 8,
	Dec = 10,
	Hex = 16,
};

USTRUCT(BlueprintType, Blueprintable)
struct PSDATAPLUGIN_API FPsDataShortBigInteger
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	int64 Base;

	UPROPERTY(BlueprintReadWrite)
	int32 PowerOfTen;

	FPsDataShortBigInteger();
	FPsDataShortBigInteger(int64 InBase, int32 InPowerOfTen);

	explicit FPsDataShortBigInteger(const FString& String, int32 NumDigits);

	FString ToScientificNotation() const;
	static FPsDataShortBigInteger FromString(const FString& String, int32 NumDigits);
};

USTRUCT(BlueprintType, Blueprintable)
struct PSDATAPLUGIN_API FPsDataBigInteger
{
	GENERATED_BODY()

private:
	/** Array of words */
	TArray<PsDataBigIntegerWordType> Words;

public:
	static const int32 NumBytesPerWord;
	static const int32 NumBitsPerWord;
	static const PsDataBigIntegerWordType MaxWordMask;
	static const PsDataBigIntegerWordType NegativeMask;

	static const FPsDataBigInteger Zero;
	static const FPsDataBigInteger One;
	static const FPsDataBigInteger Two;
	static const FPsDataBigInteger Ten;
	static const FPsDataBigInteger Hundred;
	static const FPsDataBigInteger Thousand;
	static const FPsDataBigInteger MaxInt32;
	static const FPsDataBigInteger MinInt32;
	static const FPsDataBigInteger MaxInt64;
	static const FPsDataBigInteger MinInt64;

	FPsDataBigInteger();
	FPsDataBigInteger(PsDataBigIntegerWordType Value);
	FPsDataBigInteger(const FPsDataShortBigInteger& Value);
	FPsDataBigInteger(int32 Value);
	FPsDataBigInteger(int64 Value);

	FPsDataBigInteger(const FPsDataBigInteger& Other);
	FPsDataBigInteger(FPsDataBigInteger&& Other);

	explicit FPsDataBigInteger(const FString& Value, EPsDataBigIntegerConvertionType ConvertionType = EPsDataBigIntegerConvertionType::Dec);
	explicit FPsDataBigInteger(const TArray<PsDataBigIntegerWordType>& InitialWords);
	explicit FPsDataBigInteger(TArray<PsDataBigIntegerWordType>&& InitialWords);

	/** Is negative */
	bool IsNegative() const;

	/** Is positive */
	bool IsPositive() const;

	/** Get actual number of words */
	int32 GetActualNumWords() const;

	/** Get number of words */
	int32 GetNumWords() const;

	/** Get word by index */
	PsDataBigIntegerWordType GetWord(int32 WordIndex) const;

	/** Get bit by index */
	bool GetBit(int32 BitIndex) const;

	/** Get highest non-zero bit index in word */
	int32 GetHighestNonZeroBitIndexInWord(int32 WordIndex) const;

	/** Get highest non-zero bit index */
	int32 GetHighestNonZeroBitIndex() const;

	/** Is zero */
	bool IsZero() const;

	/** Is even */
	bool IsEven() const;

	/** Get sign */
	int32 GetSign() const;

	/** this == Other */
	bool IsEqual(const FPsDataBigInteger& Other) const;

	/** this < Other */
	bool IsLess(const FPsDataBigInteger& Other) const;

	/** this <= Other */
	bool IsLessOrEqual(const FPsDataBigInteger& Other) const;

	/** this > Other */
	bool IsGreater(const FPsDataBigInteger& Other) const;

	/** this >= Other */
	bool IsGreaterOrEqual(const FPsDataBigInteger& Other) const;

	/** To string */
	FString ToString(EPsDataBigIntegerConvertionType ConvertionType = EPsDataBigIntegerConvertionType::Dec) const;

	/** To int32 */
	int32 ToInt32() const;

	/** To int64 */
	int64 ToInt64() const;

	/** To BigInteger short record */
	FPsDataShortBigInteger ToShortBigInteger(int32 NumDigits) const;

	/** Set bit by index */
	void SetBit(int32 Index, bool bNewValue);

	/** Set sign */
	void SetSign(int32 NewSign);

	/** Set sign and normalize */
	void SetSignAndNormalize(int32 NewSign);

	/** Allocate */
	void Allocate(int32 Num);

	/** Allocate extra word */
	void AllocateExtraWord();

	/** Initialize from word type */
	void Set(PsDataBigIntegerWordType Value);

	/** Initialize from int32 */
	void Set(int32 Value);

	/** Initialize from int64 */
	void Set(int64 Value);

	/** this = -this */
	void Negate();

	/** this = |this| */
	int32 Abs();

	/** this = |this| and normalize */
	int32 AbsAndNormalize();

	/** this += Other */
	void Add(const FPsDataBigInteger& Value);

	/** this -= Other */
	void Subtract(const FPsDataBigInteger& Value);

	/** this *= Factor */
	void Multiply(FPsDataBigInteger Factor);

	/** this /= Other */
	void Divide(FPsDataBigInteger Divisor);

	/** this /= Other with remainder */
	FPsDataBigInteger DivideWithRemainder(FPsDataBigInteger Divisor);

	/** this %= Other */
	void Modulo(const FPsDataBigInteger& Divisor);

	/** this <<= BitCount */
	void ShiftLeft(const int32 BitCount);

	/** this <<= 1 */
	void ShiftLeftByOne();

	/** this >>= BitCount */
	void ShiftRight(const int32 BitCount);

	/** this >>= 1 */
	void ShiftRightByOne();

	/** Bitwise OR */
	void BitwiseOr(const FPsDataBigInteger& Value);

	/** Bitwise AND */
	void BitwiseAnd(const FPsDataBigInteger& Value);

	/** Bitwise XOR */
	void BitwiseXor(const FPsDataBigInteger& Value);

	/** Bitwise NOT */
	void BitwiseNot();

	/** Normalize */
	void Normalize();

	/** Shrink */
	void Shrink();

	/** Reserve */
	void Reserve(int32 Num);

	bool operator<(const FPsDataBigInteger& Other) const;
	bool operator<=(const FPsDataBigInteger& Other) const;
	bool operator>(const FPsDataBigInteger& Other) const;
	bool operator>=(const FPsDataBigInteger& Other) const;
	bool operator==(const FPsDataBigInteger& Other) const;
	bool operator!=(const FPsDataBigInteger& Other) const;

	FPsDataBigInteger& operator=(const FPsDataBigInteger& Other);
	FPsDataBigInteger& operator=(FPsDataBigInteger&& Other);
	FPsDataBigInteger& operator=(int64 Other);
	FPsDataBigInteger& operator=(int32 Other);

	FPsDataBigInteger operator>>(int32 Count) const;
	FPsDataBigInteger& operator>>=(int32 Count);
	FPsDataBigInteger operator<<(int32 Count) const;
	FPsDataBigInteger& operator<<=(int32 Count);

	FPsDataBigInteger operator+(const FPsDataBigInteger& Value) const;
	FPsDataBigInteger& operator+=(const FPsDataBigInteger& Value);
	FPsDataBigInteger operator-(const FPsDataBigInteger& Value) const;
	FPsDataBigInteger& operator-=(const FPsDataBigInteger& Value);
	FPsDataBigInteger operator*(const FPsDataBigInteger& Factor) const;
	FPsDataBigInteger& operator*=(const FPsDataBigInteger& Factor);
	FPsDataBigInteger operator/(const FPsDataBigInteger& Divisor) const;
	FPsDataBigInteger& operator/=(const FPsDataBigInteger& Divisor);
	FPsDataBigInteger operator%(const FPsDataBigInteger& Divisor) const;
	FPsDataBigInteger& operator%=(const FPsDataBigInteger& Divisor);
	FPsDataBigInteger operator&(const FPsDataBigInteger& Value) const;
	FPsDataBigInteger& operator&=(const FPsDataBigInteger& Value);
	FPsDataBigInteger operator|(const FPsDataBigInteger& Value) const;
	FPsDataBigInteger& operator|=(const FPsDataBigInteger& Value);
	FPsDataBigInteger operator^(const FPsDataBigInteger& Value) const;
	FPsDataBigInteger& operator^=(const FPsDataBigInteger& Value);

	FPsDataBigInteger& operator++();
	FPsDataBigInteger& operator--();
	FPsDataBigInteger operator-() const;
	FPsDataBigInteger operator+() const;
	FPsDataBigInteger operator~() const;

	/** Compare two big integer */
	static int32 Compare(const FPsDataBigInteger& A, const FPsDataBigInteger& B);

	/** Pow */
	static FPsDataBigInteger Pow(FPsDataBigInteger Base, FPsDataBigInteger Exp);

	/** Sqrt */
	static FPsDataBigInteger Sqrt(FPsDataBigInteger Base);

	/** Random big integer */
	static FPsDataBigInteger Random(int32 NumBytes);

	/** Create big integer from string */
	static FPsDataBigInteger FromString(const FString& Value, EPsDataBigIntegerConvertionType ConvertionType = EPsDataBigIntegerConvertionType::Dec);

	/** Is scientific notation format */
	static bool IsScientificNotationFormat(const FString& Value);

	/** Is big integer format */
	static bool IsBigIntegerFormat(const FString& Value, EPsDataBigIntegerConvertionType ConvertionType = EPsDataBigIntegerConvertionType::Dec);

	/** Big integer test */
	static void Test();

private:
	/** Get extra word */
	PsDataBigIntegerWordType GetExtraWord() const;

	/** A << BitCount */
	void ShiftLeftInternal(const int32 Count);

	/** A << 1 */
	void ShiftLeftByOneInternal();

	/** A >> BitCount */
	void ShiftRightInternal(const int32 Count);

	/** A >> 1 */
	void ShiftRightByOneInternal();

	/** Deserialize from string */
	static FPsDataBigInteger Deserialize(const FString& Value, uint8 Divider);

	/** Serialize to string */
	static FString Serialize(const FPsDataBigInteger& Value, uint8 Divider);

public:
	/** Override Import/Export to not write out empty structs */
	bool ExportTextItem(FString& ValueStr, FPsDataBigInteger const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const;
	bool ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText);
};

template <>
struct TStructOpsTypeTraits<FPsDataBigInteger> : public TStructOpsTypeTraitsBase2<FPsDataBigInteger>
{
	enum
	{
		WithExportTextItem = true,
		WithImportTextItem = true
	};
};