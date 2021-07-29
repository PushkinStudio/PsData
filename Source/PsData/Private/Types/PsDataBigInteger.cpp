// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Types/PsDataBigInteger.h"

const int32 FPsDataBigInteger::NumBytesPerWord = sizeof(PsDataBigIntegerWordType);
const int32 FPsDataBigInteger::NumBitsPerWord = NumBytesPerWord * 8;
const PsDataBigIntegerWordType FPsDataBigInteger::MaxWordMask = ~static_cast<PsDataBigIntegerWordType>(0);
const PsDataBigIntegerWordType FPsDataBigInteger::NegativeMask = static_cast<PsDataBigIntegerWordType>(1) << (NumBitsPerWord - 1);

const FPsDataBigInteger FPsDataBigInteger::Zero(0);
const FPsDataBigInteger FPsDataBigInteger::One(1);
const FPsDataBigInteger FPsDataBigInteger::Two(2);
const FPsDataBigInteger FPsDataBigInteger::Ten(10);
const FPsDataBigInteger FPsDataBigInteger::Hundred(100);
const FPsDataBigInteger FPsDataBigInteger::Thousand(1000);
const FPsDataBigInteger FPsDataBigInteger::MaxInt32(MAX_int32);
const FPsDataBigInteger FPsDataBigInteger::MinInt32(MIN_int32);
const FPsDataBigInteger FPsDataBigInteger::MaxInt64(MAX_int64);
const FPsDataBigInteger FPsDataBigInteger::MinInt64(MIN_int64);

FPsDataShortBigInteger::FPsDataShortBigInteger()
	: Base(0)
	, PowerOfTen(0)
{
}

FPsDataShortBigInteger::FPsDataShortBigInteger(int64 InBase, int32 InPowerOfTen)
	: Base(InBase)
	, PowerOfTen(InPowerOfTen)
{
}

FPsDataShortBigInteger::FPsDataShortBigInteger(const FString& String, int32 NumDigits)
{
	*this = FromString(String, NumDigits);
}

FPsDataBigInteger::FPsDataBigInteger()
{
	Set(static_cast<PsDataBigIntegerWordType>(0));
}

FPsDataBigInteger::FPsDataBigInteger(PsDataBigIntegerWordType Value)
{
	Set(Value);
}

FPsDataBigInteger::FPsDataBigInteger(const FPsDataShortBigInteger& Value)
{
	Set(Value.Base);
	Multiply(Pow(Ten, Value.PowerOfTen));
}

FPsDataBigInteger::FPsDataBigInteger(int32 Value)
{
	Set(Value);
}

FPsDataBigInteger::FPsDataBigInteger(int64 Value)
{
	Set(Value);
}

FPsDataBigInteger::FPsDataBigInteger(const FPsDataBigInteger& Other)
{
	Words.Reset();
	Words.Append(Other.Words.GetData(), Other.GetActualNumWords());
}

FPsDataBigInteger::FPsDataBigInteger(FPsDataBigInteger&& Other) noexcept
{
	checkSlow(Other.Words.Num() > 0);
	Words = std::move(Other.Words);
}

FPsDataBigInteger::FPsDataBigInteger(const TArray<PsDataBigIntegerWordType>& InitialWords)
{
	check(InitialWords.Num() > 0);
	Words = InitialWords;
}

FPsDataBigInteger::FPsDataBigInteger(TArray<PsDataBigIntegerWordType>&& InitialWords)
{
	check(InitialWords.Num() > 0);
	Words = std::move(InitialWords);
}

FPsDataBigInteger::FPsDataBigInteger(const FString& Value, EPsDataBigIntegerConvertionType ConvertionType)
{
	*this = Deserialize(Value, static_cast<uint8>(ConvertionType));
}

bool FPsDataBigInteger::IsNegative() const
{
	const auto NumWords = Words.Num();
	checkSlow(NumWords > 0);
	return (Words[NumWords - 1] & NegativeMask) > 0;
}

bool FPsDataBigInteger::IsPositive() const
{
	return !IsNegative();
}

int32 FPsDataBigInteger::GetActualNumWords() const
{
	int32 WordIndex = Words.Num() - 1;
	if (IsNegative())
	{
		while (WordIndex > 0 && Words[WordIndex] == MaxWordMask && (Words[WordIndex - 1] & NegativeMask) > 0)
		{
			--WordIndex;
		}
	}
	else
	{
		while (WordIndex > 0 && Words[WordIndex] == 0 && (Words[WordIndex - 1] & NegativeMask) == 0)
		{
			--WordIndex;
		}
	}

	checkSlow(WordIndex >= 0);
	return WordIndex + 1;
}

int32 FPsDataBigInteger::GetNumWords() const
{
	return Words.Num();
}

PsDataBigIntegerWordType FPsDataBigInteger::GetWord(int32 WordIndex) const
{
	const auto NumWords = Words.Num();
	const auto Delta = (NumWords - 1) - WordIndex;
	if (WordIndex < 0)
	{
		return 0;
	}
	else if (WordIndex >= NumWords)
	{
		return GetExtraWord();
	}

	return Words[WordIndex];
}

bool FPsDataBigInteger::GetBit(int32 BitIndex) const
{
	const auto WordIndex = BitIndex / NumBitsPerWord;
	const auto WordBitIndex = BitIndex % NumBitsPerWord;
	return (GetWord(WordIndex) >> WordBitIndex) & 0x1;
}

int32 FPsDataBigInteger::GetHighestNonZeroBitIndexInWord(int32 WordIndex) const
{
	const auto Word = GetWord(WordIndex);
	int32 BitIndex = NumBitsPerWord - 1;
	while (BitIndex > 0 && (Word & (1 << BitIndex)) == 0)
	{
		--BitIndex;
	}
	return BitIndex;
}

int32 FPsDataBigInteger::GetHighestNonZeroBitIndex() const
{
	int32 WordIndex = Words.Num() - 1;
	while (WordIndex > 0 && Words[WordIndex] == 0)
	{
		--WordIndex;
	}

	const auto BitIndex = GetHighestNonZeroBitIndexInWord(WordIndex);
	return NumBitsPerWord * WordIndex + BitIndex;
}

bool FPsDataBigInteger::IsZero() const
{
	return Words[0] == 0 && GetActualNumWords() == 1;
}

bool FPsDataBigInteger::IsEven() const
{
	return (Words[0] & 0x1) == 0;
}

int32 FPsDataBigInteger::GetSign() const
{
	return IsNegative() ? -1 : 1;
}

bool FPsDataBigInteger::IsEqual(const FPsDataBigInteger& Other) const
{
	return Compare(*this, Other) == 0;
}

bool FPsDataBigInteger::IsLess(const FPsDataBigInteger& Other) const
{
	return Compare(*this, Other) < 0;
}

bool FPsDataBigInteger::IsLessOrEqual(const FPsDataBigInteger& Other) const
{
	return Compare(*this, Other) <= 0;
}

bool FPsDataBigInteger::IsGreater(const FPsDataBigInteger& Other) const
{
	return Compare(*this, Other) > 0;
}

bool FPsDataBigInteger::IsGreaterOrEqual(const FPsDataBigInteger& Other) const
{
	return Compare(*this, Other) >= 0;
}

FString FPsDataBigInteger::ToString(EPsDataBigIntegerConvertionType ConvertionType) const
{
	return Serialize(*this, static_cast<uint8>(ConvertionType));
}

int32 FPsDataBigInteger::ToInt32() const
{
	check(IsLessOrEqual(MaxInt32) && IsGreaterOrEqual(MinInt32));
	return static_cast<int32>(GetWord(0));
}

int64 FPsDataBigInteger::ToInt64() const
{
	check(IsLessOrEqual(MaxInt64) && IsGreaterOrEqual(MinInt64));
	return static_cast<int64>(GetWord(0)) + (static_cast<int64>(GetWord(1)) << NumBitsPerWord);
}

FPsDataShortBigInteger FPsDataBigInteger::ToShortBigInteger(int32 NumDigits) const
{
	check(NumDigits > 0 && NumDigits < 19);

	static const auto DigitShift = FMath::Log2(10);
	const auto MaxValue = Pow(Ten, NumDigits);

	auto Temp = *this;
	const auto ResultSign = Temp.Abs();
	const auto NumBitsInTemp = Temp.GetHighestNonZeroBitIndex() + 1;
	const auto MinNumDigitsInTemp = static_cast<int32>(static_cast<float>(NumBitsInTemp) / DigitShift);

	int32 PowerOfTen = FMath::Max(MinNumDigitsInTemp - NumDigits, 0);
	if (PowerOfTen > 0)
	{
		Temp.Divide(Pow(Ten, PowerOfTen));
	}

	while (Temp.IsGreaterOrEqual(MaxValue))
	{
		Temp.Divide(Ten);
		++PowerOfTen;
	}

	return {Temp.ToInt64() * ResultSign, PowerOfTen};
}

void FPsDataBigInteger::SetBit(int32 BitIndex, bool bNewValue)
{
	const auto WordIndex = BitIndex / NumBitsPerWord;
	const auto WordBitIndex = BitIndex % NumBitsPerWord;
	Allocate(WordIndex + 2);

	const PsDataBigIntegerWordType Mask = (1 << WordBitIndex);
	if (bNewValue)
	{
		Words[WordIndex] |= Mask;
	}
	else
	{
		Words[WordIndex] &= ~Mask;
	}
}

void FPsDataBigInteger::SetSign(int32 NewSign)
{
	const auto Sign = GetSign();
	const auto NewNormalizedSign = (NewSign < 0) ? -1 : 1;
	if (NewNormalizedSign != Sign)
	{
		Negate();
	}
}

void FPsDataBigInteger::SetSignAndNormalize(int32 NewSign)
{
	SetSign(NewSign);
	Normalize();
}

void FPsDataBigInteger::Allocate(int32 Num)
{
	const PsDataBigIntegerWordType NewWord = GetExtraWord();
	Words.Reserve(Num);
	while (Words.Num() < Num)
	{
		Words.Add(NewWord);
	}
}

void FPsDataBigInteger::AllocateExtraWord()
{
	Words.Add(GetExtraWord());
}

void FPsDataBigInteger::Set(PsDataBigIntegerWordType Value)
{
	Words.SetNum(1, false);
	Words[0] = Value;
}

void FPsDataBigInteger::Set(int32 Value)
{
	Words.SetNum(1, false);
	Words[0] = static_cast<PsDataBigIntegerWordType>(Value & MaxWordMask);
}

void FPsDataBigInteger::Set(int64 Value)
{
	Words.SetNum(2, false);
	Words[0] = static_cast<PsDataBigIntegerWordType>(Value & MaxWordMask);
	Words[1] = static_cast<PsDataBigIntegerWordType>(Value >> NumBitsPerWord);
}

void FPsDataBigInteger::Negate()
{
	BitwiseNot();
	Add(One);
}

int32 FPsDataBigInteger::Abs()
{
	if (IsNegative())
	{
		Negate();
		return -1;
	}

	return 1;
}

int32 FPsDataBigInteger::AbsAndNormalize()
{
	const auto Sign = Abs();
	Normalize();
	return Sign;
}

void FPsDataBigInteger::Add(const FPsDataBigInteger& Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords()) + 1;
	Allocate(NumWords);

	int64 Memory = 0;
	for (int32 i = 0; i < NumWords; ++i)
	{
		const int64 WordSum = static_cast<int64>(GetWord(i)) + static_cast<int64>(Value.GetWord(i)) + Memory;
		Words[i] = static_cast<PsDataBigIntegerWordType>(WordSum & MaxWordMask);
		Memory = (WordSum >> NumBitsPerWord);
	}

	Normalize();
}

void FPsDataBigInteger::Subtract(const FPsDataBigInteger& Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords()) + 1;
	Allocate(NumWords);

	int64 Memory = 0;
	for (int32 i = 0; i < NumWords; ++i)
	{
		const int64 WordSum = static_cast<int64>(GetWord(i)) - static_cast<int64>(Value.GetWord(i)) + Memory;
		Words[i] = static_cast<PsDataBigIntegerWordType>(WordSum & MaxWordMask);
		Memory = (WordSum >> NumBitsPerWord);
	}

	Normalize();
}

void FPsDataBigInteger::Divide(FPsDataBigInteger Divisor)
{
	check(!Divisor.IsZero());

	const auto ResultSign = AbsAndNormalize() * Divisor.AbsAndNormalize();
	auto Dividend = *this;
	*this = 0;

	const auto Comp = Compare(Dividend, Divisor);
	if (Comp < 0)
	{
		return;
	}
	else if (Comp == 0)
	{
		*this = ResultSign;
		return;
	}

	auto LShift = Dividend.GetHighestNonZeroBitIndex() - Divisor.GetHighestNonZeroBitIndex();
	Divisor.ShiftLeft(LShift);

	while (LShift > -1)
	{
		if (Dividend >= Divisor)
		{
			SetBit(LShift, true);
			Dividend.Subtract(Divisor);
		}

		Divisor.ShiftRightByOne();
		--LShift;
	}

	SetSignAndNormalize(ResultSign);
}

FPsDataBigInteger FPsDataBigInteger::DivideWithRemainder(FPsDataBigInteger Divisor)
{
	check(!Divisor.IsZero());

	const auto DividendSign = AbsAndNormalize();
	const auto ResultSign = DividendSign * Divisor.AbsAndNormalize();
	auto Dividend = *this;
	*this = 0;

	const auto Comp = Compare(Dividend, Divisor);
	if (Comp < 0)
	{
		if (DividendSign < 0)
		{
			Dividend.Negate();
		}
		return Dividend;
	}
	else if (Comp == 0)
	{
		*this = Dividend.GetSign() * Divisor.GetSign();
		return Zero;
	}

	auto LShift = Dividend.GetHighestNonZeroBitIndex() - Divisor.GetHighestNonZeroBitIndex();
	Divisor.ShiftLeft(LShift);

	while (LShift > -1)
	{
		if (Dividend >= Divisor)
		{
			SetBit(LShift, true);
			Dividend.Subtract(Divisor);
		}

		Divisor.ShiftRightByOne();
		--LShift;
	}

	SetSignAndNormalize(ResultSign);
	Dividend.SetSignAndNormalize(DividendSign);

	return Dividend;
}

void FPsDataBigInteger::Multiply(FPsDataBigInteger Factor)
{
	const auto ResultSign = AbsAndNormalize() * Factor.AbsAndNormalize();
	const auto NumWordsA = GetNumWords();
	const auto NumWordsB = Factor.GetNumWords();

	auto Base = *this;
	*this = 0;

	Base.Reserve(NumWordsA + NumWordsB);
	Reserve(NumWordsA + NumWordsB);

	const auto MaxBitIndex = Factor.GetHighestNonZeroBitIndex();
	int32 BitIndex = 0;
	int32 Shift = 0;
	while (BitIndex <= MaxBitIndex)
	{
		if (Factor.GetBit(BitIndex))
		{
			Base.ShiftLeft(Shift);
			Shift = 0;
			Add(Base);
		}

		++Shift;
		++BitIndex;
	}

	SetSignAndNormalize(ResultSign);
}

void FPsDataBigInteger::Modulo(const FPsDataBigInteger& Divisor)
{
	*this = DivideWithRemainder(Divisor);
}

void FPsDataBigInteger::ShiftLeft(const int32 BitCount)
{
	if (BitCount > 0)
	{
		ShiftLeftInternal(BitCount);
	}
	else if (BitCount < 0)
	{
		ShiftRightInternal(-BitCount);
	}
}

void FPsDataBigInteger::ShiftLeftByOne()
{
	ShiftLeftByOneInternal();
}

void FPsDataBigInteger::ShiftRight(const int32 BitCount)
{
	if (BitCount > 0)
	{
		ShiftRightInternal(BitCount);
	}
	else if (BitCount < 0)
	{
		ShiftLeftInternal(-BitCount);
	}
}

void FPsDataBigInteger::ShiftRightByOne()
{
	ShiftRightByOneInternal();
}

void FPsDataBigInteger::BitwiseOr(const FPsDataBigInteger& Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords());
	Allocate(NumWords);

	for (int32 i = 0; i < NumWords; ++i)
	{
		Words[i] = GetWord(i) | Value.GetWord(i);
	}
}

void FPsDataBigInteger::BitwiseAnd(const FPsDataBigInteger& Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords());
	Allocate(NumWords);

	for (int32 i = 0; i < NumWords; ++i)
	{
		Words[i] = GetWord(i) & Value.GetWord(i);
	}
}

void FPsDataBigInteger::BitwiseXor(const FPsDataBigInteger& Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords());
	Allocate(NumWords);

	for (int32 i = 0; i < NumWords; ++i)
	{
		Words[i] = GetWord(i) ^ Value.GetWord(i);
	}
}

void FPsDataBigInteger::BitwiseNot()
{
	const auto NumWords = Words.Num();
	for (int32 i = 0; i < NumWords; ++i)
	{
		Words[i] = ~Words[i];
	}
}

void FPsDataBigInteger::Normalize()
{
	Words.SetNum(GetActualNumWords(), false);
}

void FPsDataBigInteger::Shrink()
{
	Normalize();
	Words.Shrink();
}

void FPsDataBigInteger::Reserve(int32 Num)
{
	Words.Reserve(Num);
}

bool FPsDataBigInteger::operator<(const FPsDataBigInteger& Other) const
{
	return IsLess(Other);
}

bool FPsDataBigInteger::operator<=(const FPsDataBigInteger& Other) const
{
	return IsLessOrEqual(Other);
}

bool FPsDataBigInteger::operator>(const FPsDataBigInteger& Other) const
{
	return IsGreater(Other);
}

bool FPsDataBigInteger::operator>=(const FPsDataBigInteger& Other) const
{
	return IsGreaterOrEqual(Other);
}

bool FPsDataBigInteger::operator==(const FPsDataBigInteger& Other) const
{
	return IsEqual(Other);
}

bool FPsDataBigInteger::operator!=(const FPsDataBigInteger& Other) const
{
	return !IsEqual(Other);
}

FPsDataBigInteger& FPsDataBigInteger::operator=(const FPsDataBigInteger& Other)
{
	Words.Reset();
	Words.Append(Other.Words.GetData(), Other.GetActualNumWords());
	return *this;
}

FPsDataBigInteger& FPsDataBigInteger::operator=(FPsDataBigInteger&& Other) noexcept
{
	checkSlow(Other.Words.Num() > 0);
	Words = std::move(Other.Words);
	return *this;
}

FPsDataBigInteger& FPsDataBigInteger::operator=(int64 Other)
{
	Set(Other);
	return *this;
}

FPsDataBigInteger& FPsDataBigInteger::operator=(int32 Other)
{
	Set(Other);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator>>(int32 Count) const
{
	FPsDataBigInteger Copy = *this;
	Copy.ShiftRight(Count);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator>>=(int32 Count)
{
	ShiftRight(Count);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator<<(int32 Count) const
{
	FPsDataBigInteger Copy = *this;
	Copy.ShiftLeft(Count);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator<<=(int32 Count)
{
	ShiftLeft(Count);
	return *this;
}
FPsDataBigInteger FPsDataBigInteger::operator+(const FPsDataBigInteger& Value) const
{
	FPsDataBigInteger Copy = *this;
	Copy.Add(Value);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator+=(const FPsDataBigInteger& Value)
{
	Add(Value);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator-(const FPsDataBigInteger& Value) const
{
	FPsDataBigInteger Copy = *this;
	Copy.Subtract(Value);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator-=(const FPsDataBigInteger& Value)
{
	Subtract(Value);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator*(const FPsDataBigInteger& Factor) const
{
	FPsDataBigInteger Copy = *this;
	Copy.Multiply(Factor);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator*=(const FPsDataBigInteger& Factor)
{
	Multiply(Factor);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator/(const FPsDataBigInteger& Divisor) const
{
	FPsDataBigInteger Copy = *this;
	Copy.Divide(Divisor);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator/=(const FPsDataBigInteger& Divisor)
{
	Divide(Divisor);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator%(const FPsDataBigInteger& Divisor) const
{
	FPsDataBigInteger Copy = *this;
	Copy.Modulo(Divisor);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator%=(const FPsDataBigInteger& Divisor)
{
	Modulo(Divisor);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator&(const FPsDataBigInteger& Other) const
{
	FPsDataBigInteger Copy = *this;
	Copy.BitwiseAnd(Other);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator&=(const FPsDataBigInteger& Value)
{
	BitwiseAnd(Value);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator|(const FPsDataBigInteger& Value) const
{
	FPsDataBigInteger Copy = *this;
	Copy.BitwiseOr(Value);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator|=(const FPsDataBigInteger& Value)
{
	BitwiseOr(Value);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator^(const FPsDataBigInteger& Value) const
{
	FPsDataBigInteger Copy = *this;
	Copy.BitwiseXor(Value);
	return Copy;
}

FPsDataBigInteger& FPsDataBigInteger::operator^=(const FPsDataBigInteger& Value)
{
	BitwiseXor(Value);
	return *this;
}

FPsDataBigInteger& FPsDataBigInteger::operator++()
{
	Add(One);
	return *this;
}

FPsDataBigInteger& FPsDataBigInteger::operator--()
{
	Subtract(One);
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator-() const
{
	FPsDataBigInteger Copy = *this;
	Copy.Negate();
	return Copy;
}

FPsDataBigInteger FPsDataBigInteger::operator+() const
{
	return *this;
}

FPsDataBigInteger FPsDataBigInteger::operator~() const
{
	FPsDataBigInteger Copy = *this;
	Copy.BitwiseNot();
	return Copy;
}

int32 FPsDataBigInteger::Compare(const FPsDataBigInteger& A, const FPsDataBigInteger& B)
{
	const auto SignA = A.GetSign();
	const auto SignB = B.GetSign();
	const auto Sign = SignA * SignB;

	int32 WordIndex = FMath::Max(A.GetNumWords(), B.GetNumWords()) - 1;
	while (WordIndex >= 0)
	{
		const auto WordA = A.GetWord(WordIndex);
		const auto WordB = B.GetWord(WordIndex);
		if (WordA != WordB)
		{
			return (WordA < WordB) ? -Sign : Sign;
		}

		--WordIndex;
	}

	return 0;
}

FPsDataBigInteger FPsDataBigInteger::Pow(FPsDataBigInteger Base, FPsDataBigInteger Exp)
{
	if (Exp < Zero)
	{
		return 0;
	}

	const auto NewSign = Exp.IsEven() ? 1 : Base.GetSign();
	Base.AbsAndNormalize();

	auto Result = One;

	const auto MaxBitIndex = Exp.GetHighestNonZeroBitIndex();
	int32 BitIndex = 0;
	int32 Shift = 0;
	while (BitIndex <= MaxBitIndex)
	{
		if (Exp.GetBit(BitIndex))
		{
			Result.Multiply(Base);
		}

		Base.Multiply(Base);
		++BitIndex;
	}

	Result.SetSignAndNormalize(NewSign);
	return Result;
}

FPsDataBigInteger FPsDataBigInteger::Sqrt(FPsDataBigInteger Base)
{
	const auto Comp = Compare(Base, Zero);
	check(Comp >= 0);

	if (Comp == 0)
	{
		return Zero;
	}

	FPsDataBigInteger Result;
	FPsDataBigInteger Temp;

	Result.Reserve(Base.GetNumWords() / 2);
	Temp.Reserve(Base.GetNumWords() / 2);

	const auto MaxBitIndex = Base.GetHighestNonZeroBitIndex() & ~0x1;
	auto BitIndex = MaxBitIndex;
	while (BitIndex > -1)
	{
		Temp = Result;
		Result.ShiftRightByOne();

		Temp.SetBit(BitIndex, true);
		if (Base >= Temp)
		{
			Base -= Temp;
			Result.SetBit(BitIndex, true);
		}

		BitIndex -= 2;
	}

	return Result;
}

FPsDataBigInteger FPsDataBigInteger::Random(int32 NumBytes)
{
	const int32 NumWords = NumBytes / NumBytesPerWord;
	const int32 NumExtraBytes = NumBytes % NumBytesPerWord;

	FPsDataBigInteger Result;

	for (int32 i = 0; i < NumWords; ++i)
	{
		if (i > 0)
		{
			Result <<= NumBitsPerWord;
		}

		const int32 RandWord = static_cast<int32>(FMath::RandRange(0LL, static_cast<int64>(MaxWordMask)));
		Result += RandWord;
	}

	for (int32 i = 0; i < NumExtraBytes; ++i)
	{
		if (i != 0)
		{
			Result <<= 8;
		}

		const int32 RandByte = FMath::RandRange(0, 0xFF);
		Result += RandByte;
	}

	if (FMath::RandBool())
	{
		Result.Negate();
	}

	return Result;
}

FPsDataBigInteger FPsDataBigInteger::FromString(const FString& Value, EPsDataBigIntegerConvertionType ConvertionType)
{
	return FPsDataBigInteger(Value, ConvertionType);
}

PsDataBigIntegerWordType FPsDataBigInteger::GetExtraWord() const
{
	return IsNegative() ? MaxWordMask : 0;
}

void FPsDataBigInteger::ShiftLeftInternal(const int32 Count)
{
	checkSlow(Count > 0);

	const auto WordsShift = Count / NumBitsPerWord;
	const auto LShift = Count % NumBitsPerWord;
	const auto NumWords = Words.Num() + 1;

	Words.Reserve(NumWords + WordsShift);

	if (LShift > 0)
	{
		const auto RShift = NumBitsPerWord - LShift;
		Allocate(NumWords);

		int32 i = NumWords;
		while (--i >= 0)
		{
			Words[i] = ((GetWord(i - 1) >> RShift) | (GetWord(i) << LShift));
		}
	}

	Words.InsertZeroed(0, WordsShift);
	Normalize();
}

void FPsDataBigInteger::ShiftLeftByOneInternal()
{
	const auto NumWords = Words.Num() + 1;
	const auto RShift = NumBitsPerWord - 1;
	Allocate(NumWords);

	int32 i = NumWords;
	while (--i >= 0)
	{
		Words[i] = ((GetWord(i - 1) >> RShift) | (GetWord(i) << 1));
	}

	Normalize();
}

void FPsDataBigInteger::ShiftRightInternal(const int32 Count)
{
	checkSlow(Count > 0);

	const auto WordsShift = Count / NumBitsPerWord;
	const auto RShift = Count % NumBitsPerWord;

	AllocateExtraWord();
	Words.RemoveAt(0, FMath::Min(WordsShift, Words.Num() - 1), false);

	if (RShift > 0)
	{
		const auto LShift = NumBitsPerWord - RShift;
		const auto NumWords = Words.Num();
		int32 i = 0;
		while (i < NumWords)
		{
			Words[i] = ((GetWord(i) >> RShift) | (GetWord(i + 1) << LShift));
			++i;
		}
	}

	Normalize();
}

void FPsDataBigInteger::ShiftRightByOneInternal()
{
	const auto LShift = NumBitsPerWord - 1;
	const auto NumWords = Words.Num();
	int32 i = 0;
	while (i < NumWords)
	{
		Words[i] = ((GetWord(i) >> 1) | (GetWord(i + 1) << LShift));
		++i;
	}

	Normalize();
}

/**
 * Serialize / Deserialize
 */

namespace Utils
{
bool IsDot(TCHAR Char)
{
	switch (Char)
	{
	case '.': return true;
	case ',': return true;
	default: return false;
	}
}

PsDataBigIntegerWordType CharToWord(TCHAR Char)
{
	const char c = static_cast<char>(Char);
	if (c >= '0' && c <= '9')
	{
		return c - '0';
	}
	else if (c >= 'a' && c <= 'z')
	{
		return 10 + (c - 'a');
	}
	else if (c >= 'A' && c <= 'Z')
	{
		return 10 + (c - 'A');
	}

	return FPsDataBigInteger::MaxWordMask;
}

TCHAR WordToChar(PsDataBigIntegerWordType Value)
{
	if (Value >= 0 && Value <= 9)
	{
		return '0' + Value;
	}
	else if (Value >= 10 && Value <= 36)
	{
		return 'a' + (Value - 10);
	}

	checkNoEntry();
	return '?';
}

bool IsNumber(const TCHAR* Array, int32 Size, uint8 Divider, bool bSkipDot = false)
{
	if (!FMath::IsPowerOfTwo(Divider))
	{
		const auto Char = *Array;
		if (Char == '-' || Char == '+')
		{
			Array += 1;
			Size -= 1;
		}
	}

	int32 NumDotForSkip = (bSkipDot ? 1 : 0);
	while (Size > 0)
	{
		const auto Char = *Array;
		Array += 1;
		Size -= 1;

		if (NumDotForSkip > 0 && IsDot(Char))
		{
			--NumDotForSkip;
			continue;
		}
		const auto Digit = CharToWord(Char);
		if (Digit >= Divider)
		{
			return false;
		}
	}

	return true;
}

int64 Pow64(int64 Base, int64 Pow)
{
	int64 Result = 1;
	while (Pow > 0)
	{
		if (Pow & 0x1)
		{
			Result = Result * Base;
		}
		Base = Base * Base;
		Pow >>= 1;
	}
	return Result;
}

int32 FindChar(const TCHAR* Array, int32 Size, char Char1)
{
	int32 i = 0;
	while (i < Size)
	{
		const auto Char = Array[i];
		if (Char == Char1)
		{
			return i;
		}
		++i;
	}

	return INDEX_NONE;
}

int32 FindChar(const TCHAR* Array, int32 Size, char Char1, char Char2)
{
	int32 i = 0;
	while (i < Size)
	{
		const auto Char = Array[i];
		if (Char == Char1 || Char == Char2)
		{
			return i;
		}
		++i;
	}

	return INDEX_NONE;
}

uint8 ShiftForDivider(uint8 Divider)
{
	uint8 i = 0;
	while (Divider >>= 1)
		++i;
	return i;
}
} // namespace Utils

namespace Deserialize
{
FPsDataBigInteger PowerOfTwoDivider(const TCHAR* Array, int32 Size, uint8 Divider)
{
	check(Size > 0);

	const auto DigitShift = Utils::ShiftForDivider(Divider);
	const auto WordNumShifts = FPsDataBigInteger::NumBitsPerWord / DigitShift;
	const auto NumWords = (Size / WordNumShifts) + 1;

	TArray<PsDataBigIntegerWordType> Buffer;
	Buffer.AddUninitialized(NumWords);

	for (int32 WordIndex = 0; WordIndex < NumWords; ++WordIndex)
	{
		PsDataBigIntegerWordType Word = 0;
		auto WordShift = 0;
		while (WordShift < FPsDataBigInteger::NumBitsPerWord && Size > 0)
		{
			const auto Digit = Utils::CharToWord(Array[--Size]);
			checkSlow(Digit < Divider);
			Word |= (Digit << WordShift);
			WordShift += DigitShift;
		}

		Buffer[WordIndex] = Word;
	}

	return FPsDataBigInteger(std::move(Buffer));
}

FPsDataBigInteger AnyDividerForUnsigned(const TCHAR* Array, int32 Size, uint8 Divider, bool bSkipDot = false)
{
	const auto DigitShift = FMath::Log2(Divider);
	const auto WordNumShifts = FPsDataBigInteger::NumBitsPerWord / DigitShift;
	const auto NumWords = (Size / WordNumShifts) + 1;
	const auto LastCharIndex = Size - 1;

	FPsDataBigInteger Result;
	Result.Reserve(NumWords);

	int64 Value = 0;
	int64 Factor = 1;
	int32 NumDotForSkip = (bSkipDot ? 1 : 0);

	for (int32 i = 0; i < Size; ++i)
	{
		const auto Char = Array[i];
		if (NumDotForSkip > 0 && Utils::IsDot(Char))
		{
			--NumDotForSkip;
			continue;
		}

		const auto Digit = Utils::CharToWord(Char);
		checkSlow(Digit < Divider);
		Value *= Divider;
		Value += Digit;
		Factor *= Divider;

		if ((Factor * Divider) >= MAX_int32 || i == LastCharIndex)
		{
			Result *= Factor;
			Result += Value;
			Value = 0;
			Factor = 1;
		}
	}

	return Result;
}

FPsDataBigInteger ScientificNotation(const TCHAR* Array, int32 Size, int32 ePos)
{
	FPsDataBigInteger E;
	const auto ESign = Array[ePos + 1];
	if (ESign == '+' || ESign == '-')
	{
		E = AnyDividerForUnsigned(&Array[ePos + 2], Size - ePos - 2, 10);
	}
	else
	{
		E = AnyDividerForUnsigned(&Array[ePos + 1], Size - ePos - 1, 10);
	}

	const auto DotPos = Utils::FindChar(Array, ePos, '.', ',');
	if (DotPos == INDEX_NONE)
	{
		const auto V = AnyDividerForUnsigned(Array, ePos, 10);
		return V * FPsDataBigInteger::Pow(FPsDataBigInteger::Ten, E);
	}
	else
	{
		const auto V = AnyDividerForUnsigned(Array, ePos, 10, true);
		E -= ePos - DotPos - 1;
		if (E.IsZero())
		{
			return V;
		}
		else if (E > 0)
		{
			return V * FPsDataBigInteger::Pow(FPsDataBigInteger::Ten, E);
		}
		else
		{
			E.Negate();
			return V / FPsDataBigInteger::Pow(FPsDataBigInteger::Ten, E);
		}
	}
}

FPsDataBigInteger AnyDivider(const TCHAR* Array, int32 Size, uint8 Divider)
{
	const bool bNegative = (Array[0] == '-');
	if (bNegative)
	{
		Array += +1;
		Size -= 1;
	}

	check(Size > 0);

	FPsDataBigInteger Result;

	if (Divider == 10)
	{
		const auto ePos = Utils::FindChar(Array, Size, 'e', 'E');
		if (ePos != INDEX_NONE)
		{
			Result = ScientificNotation(Array, Size, ePos);
		}
		else
		{
			Result = AnyDividerForUnsigned(Array, Size, Divider);
		}
	}
	else
	{
		Result = AnyDividerForUnsigned(Array, Size, Divider);
	}

	if (bNegative)
	{
		Result.Negate();
	}

	return Result;
}
} // namespace Deserialize

FPsDataBigInteger FPsDataBigInteger::Deserialize(const FString& Value, uint8 Divider)
{
	if (Divider < 2)
	{
		Divider = 10;
	}

	if (Value.Len() == 0)
	{
		return 0;
	}
	else if (FMath::IsPowerOfTwo(Divider))
	{
		return Deserialize::PowerOfTwoDivider(Value.GetCharArray().GetData(), Value.Len(), Divider);
	}
	else
	{
		return Deserialize::AnyDivider(Value.GetCharArray().GetData(), Value.Len(), Divider);
	}
}

namespace Serialize
{
FString PowerOfTwoDivider(const FPsDataBigInteger& Value, uint8 Divider)
{
	const auto DigitShift = Utils::ShiftForDivider(Divider);
	const auto WordNumShifts = FPsDataBigInteger::NumBitsPerWord / DigitShift;
	const auto StepMask = Divider - 1;
	const auto NumWords = Value.GetActualNumWords();
	const auto BufferSize = NumWords * WordNumShifts;

	TArray<TCHAR> Buffer;
	Buffer.AddUninitialized(BufferSize);
	auto BufferPtr = Buffer.GetData();
	int32 BufferIndex = BufferSize;
	for (int32 WordIndex = 0; WordIndex < NumWords; ++WordIndex)
	{
		PsDataBigIntegerWordType Word = Value.GetWord(WordIndex);
		for (int32 ShiftIndex = 0; ShiftIndex < WordNumShifts; ++ShiftIndex)
		{
			BufferPtr[--BufferIndex] = Utils::WordToChar(Word & StepMask);
			Word >>= DigitShift;
		}
	}

	int32 TrimmedBufferSize = BufferSize;
	while (BufferPtr[0] == '0' && TrimmedBufferSize > 1)
	{
		BufferPtr = BufferPtr + 1;
		--TrimmedBufferSize;
	}

	return FString(TrimmedBufferSize, BufferPtr);
}

FString AnyDividerForUnsigned(const FPsDataBigInteger& Value, uint8 Divider)
{
	if (Value.IsZero())
	{
		return TEXT("0");
	}

	const auto DigitShift = FMath::Log2(Divider);
	const auto WordNumFactor = FPsDataBigInteger::NumBitsPerWord / DigitShift;
	const auto BigDivider = Utils::Pow64(Divider, WordNumFactor);
	const auto NumWords = Value.GetNumWords();
	const auto BufferSize = (NumWords * WordNumFactor) + 1;

	TArray<TCHAR> Buffer;
	Buffer.AddUninitialized(BufferSize);
	const auto BufferPtr = Buffer.GetData();
	int32 BufferIndex = BufferSize;
	FPsDataBigInteger Remainder = Value;
	while (!Remainder.IsZero())
	{
		int32 Num = 0;
		PsDataBigIntegerWordType Part = 0;
		if (Remainder.GetNumWords() > 1)
		{
			Part = Remainder.DivideWithRemainder(BigDivider).GetWord(0);
			Num = WordNumFactor;
		}
		else
		{
			Part = Remainder.GetWord(0);
			Remainder = 0;
			Num = 0;
		}

		while (Part > 0 || Num > 0)
		{
			BufferPtr[--BufferIndex] = Utils::WordToChar(Part % Divider);
			Part /= Divider;
			--Num;
		}
	}

	return FString(Buffer.Num() - BufferIndex, &BufferPtr[BufferIndex]);
}

FString AnyDivider(const FPsDataBigInteger& Value, uint8 Divider)
{
	const auto bNeagitive = Value.IsNegative();
	if (bNeagitive)
	{
		return TEXT("-") + AnyDividerForUnsigned(-Value, Divider);
	}
	else
	{
		return AnyDividerForUnsigned(Value, Divider);
	}
}
} // namespace Serialize

FString FPsDataBigInteger::Serialize(const FPsDataBigInteger& Value, uint8 Divider)
{
	if (Divider < 2)
	{
		Divider = 10;
	}

	if (Value.IsZero())
	{
		return TEXT("0");
	}
	else
	{
		const bool bPowerOfTwo = FMath::IsPowerOfTwo(Divider);
		if (bPowerOfTwo)
		{
			return Serialize::PowerOfTwoDivider(Value, Divider);
		}
		else
		{
			return Serialize::AnyDivider(Value, Divider);
		}
	}
}

FString FPsDataShortBigInteger::ToScientificNotation() const
{
	const bool bBaseNegative = Base < 0;
	const auto BaseString = Serialize::AnyDividerForUnsigned(FMath::Abs(Base), 10);
	const auto NewPowerOfTen = PowerOfTen + BaseString.Len() - 1;
	const auto bNewPowerOfTenNegative = NewPowerOfTen < 0;
	const auto PowerOfTenString = Serialize::AnyDividerForUnsigned(FMath::Abs(NewPowerOfTen), 10);

	FString Result;
	Result.Reserve(BaseString.Len() + PowerOfTenString.Len() + 3);

	if (bBaseNegative)
	{
		Result += "-";
	}

	Result += BaseString[0];

	if (BaseString.Len() > 1)
	{
		Result += '.';
		Result.Append(&BaseString[1], BaseString.Len() - 1);
	}

	Result.AppendChar('E');

	if (bNewPowerOfTenNegative)
	{
		Result += '-';
	}
	else
	{
		Result += '+';
	}

	Result.Append(PowerOfTenString);

	return Result;
}

FPsDataShortBigInteger FPsDataShortBigInteger::FromString(const FString& String, int32 NumDigits)
{
	return Deserialize::AnyDivider(String.GetCharArray().GetData(), String.Len(), 10).ToShortBigInteger(NumDigits);
}

bool FPsDataBigInteger::IsScientificNotationFormat(const FString& Value)
{
	const auto Size = Value.Len();
	const auto Buffer = Value.GetCharArray().GetData();

	const auto ePos = Utils::FindChar(Buffer, Size, 'E', 'e');
	if (ePos != INDEX_NONE)
	{
		if (Utils::IsNumber(Buffer, ePos, 10, true) && Utils::IsNumber(&Buffer[ePos + 1], Size - ePos - 1, 10))
		{
			return true;
		}
	}

	return false;
}

bool FPsDataBigInteger::IsBigIntegerFormat(const FString& Value, EPsDataBigIntegerConvertionType ConvertionType)
{
	const auto Divider = static_cast<uint8>(ConvertionType);
	if (Utils::IsNumber(Value.GetCharArray().GetData(), Value.Len(), Divider))
	{
		return true;
	}
	else if (Divider == 10 && IsScientificNotationFormat(Value))
	{
		return true;
	}

	return false;
}

bool FPsDataBigInteger::ExportTextItem(FString& ValueStr, FPsDataBigInteger const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
	ValueStr = ToString();
	return true;
}

bool FPsDataBigInteger::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	int32 Size = 0;
	auto Cursor = Buffer;
	while (*Cursor != '\0')
	{
		const auto Digit = Utils::CharToWord(*Cursor);
		if (Digit >= 10)
		{
			return false;
		}
		Cursor += 1;
		Size++;
	}

	if (Size > 0)
	{
		*this = Deserialize::AnyDivider(Buffer, Size, 10);
		return true;
	}

	return false;
}

/**
 * Test
 */

#if !UE_BUILD_SHIPPING

namespace TestUtils
{

struct TestCaseVariables
{
	static bool UseInt64ForDumpBigInt;

	TMap<FName, FString> Properties;

	void Register(FName PropertyName, bool Value)
	{
		Properties.Add(PropertyName, Value ? TEXT("True") : TEXT("False"));
	}

	void Register(FName PropertyName, int64 Value)
	{
		Properties.Add(PropertyName, FString::Printf(TEXT("%lld"), Value));
	}

	void Register(FName PropertyName, const FPsDataBigInteger& Value)
	{
		if (UseInt64ForDumpBigInt)
		{
			Properties.Add(PropertyName, FString::Printf(TEXT("%lld"), Value.ToInt64()));
		}
		else
		{
			Properties.Add(PropertyName, Value.ToString());
		}
	}

	void Register(FName PropertyName, const FString& Value)
	{
		Properties.Add(PropertyName, Value);
	}

	void Print()
	{
		for (const auto& Pair : Properties)
		{
			UE_LOG(LogTemp, Warning, TEXT("  %s: %s"), *Pair.Key.ToString(), *Pair.Value);
		}
	}
};

bool TestCaseVariables::UseInt64ForDumpBigInt = true;

struct TestCase
{
private:
	FString Name;
	int32 Count;

public:
	TestCaseVariables Input;
	TestCaseVariables Output;

	TestCase(const FString& InName)
	{
		Count = 0;
		Name = InName;
		UE_LOG(LogTemp, Warning, TEXT("Test %s started"), *Name);
	}

	~TestCase()
	{
		UE_LOG(LogTemp, Warning, TEXT("Test %s completed. Cases passed: %d"), *Name, Count);
	}

	void Inc()
	{
		++Count;
		if (Count % 100 == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("  passed: %d"), Count);
		}
	}

	int32 GetCounter()
	{
		return Count;
	}

	void PrintDump()
	{
		UE_LOG(LogTemp, Warning, TEXT(" Inputs:"));
		Input.Print();

		UE_LOG(LogTemp, Warning, TEXT(" Outputs:"));
		Output.Print();
	}
};

uint8 RandByte()
{
	return (FMath::Rand() % 0xFF);
}

int32 RandInt()
{
	return (RandByte() | (RandByte() << 8) | (RandByte() << 16) | (RandByte() << 24));
}

int64 RandInt64()
{
	return static_cast<int64>(RandInt()) | (static_cast<int64>(RandInt()) << 32);
}

FString RandString(int32 Size, int32 Divider)
{
	FString Str;
	const int32 LenOfString = Size + ((FMath::Rand() % Size) - (Size / 2));
	for (int32 j = 0; j < LenOfString; ++j)
	{
		Str += Utils::WordToChar(FMath::Rand() % Divider);
	}

	while (Str[0] == '0')
	{
		Str.RemoveAt(0);
	}

	return Str;
}

template <typename T>
void AddToArray(TArray<T>& Array, int32 Count, const std::initializer_list<int64>& Values)
{
	for (int32 i = 0; i < Count; ++i)
	{
		for (const int64& Value : Values)
		{
			Array.Add(Value);
		}
	}
}

void AddToArray(TArray<FString>& Array, int32 Count, int32 Divider, const std::initializer_list<int64>& Values)
{
	for (int32 i = 0; i < Count; ++i)
	{
		for (const auto Value : Values)
		{
			if (Divider == 16)
			{
				Array.Add(FString::Printf(TEXT("%x"), Value));
			}
			else if (Divider == 10)
			{
				Array.Add(FString::Printf(TEXT("%lld"), Value));
			}
			else
			{
				checkNoEntry();
			}
		}
	}
}

template <typename T>
void Shuffle(TArray<T>& Array)
{
	for (int32 i = 0; i < 10; ++i)
	{
		Array.Sort([](const T& A, const T& B) {
			return FMath::RandBool();
		});
	}
}

} // namespace TestUtils

#define TEST_CASE_START(Name) \
	TestUtils::TestCase _TestCase(TEXT(#Name));

#define OVERFLOW_PROTECTION_FOR_INT64(Property, Size) \
	Property = Property >> Size;                      \
	_TestCase.Input.Register(#Property, Property);

#define TEST_CASE_ADD(Property, Value) \
	auto Property = Value;             \
	_TestCase.Input.Register(#Property, Property);

#define TEST_CASE_ADD_BIG(Property, Value)    \
	auto Property = FPsDataBigInteger(Value); \
	_TestCase.Input.Register(#Property, Property);

#define TEST_CASE(A, B)                                                                                          \
	{                                                                                                            \
		_TestCase.Inc();                                                                                         \
		auto _RB = (B);                                                                                          \
		auto _RA = (A);                                                                                          \
		if (_RB != _RA)                                                                                          \
		{                                                                                                        \
			UE_LOG(LogTemp, Warning, TEXT("--------- Test failed! Case #%d ---------"), _TestCase.GetCounter()); \
			_TestCase.Output.Register(#A, _RA);                                                                  \
			_TestCase.Output.Register(#B, _RB);                                                                  \
			_TestCase.PrintDump();                                                                               \
			check(false);                                                                                        \
		}                                                                                                        \
	}

#endif // !UE_BUILD_SHIPPING

void FPsDataBigInteger::Test()
{
#if !UE_BUILD_SHIPPING
	const int32 NumOfCases = 500;

	TArray<int64> Int64s;
	TArray<FPsDataBigInteger> BigInts;
	TArray<FString> HexStrings;
	TArray<FString> DecStrings;

	for (int32 i = 0; i < NumOfCases; ++i)
	{
		Int64s.Add(FMath::RandBool() ? TestUtils::RandInt64() : TestUtils::RandInt());
		BigInts.Add(Random(200 * FMath::FRand()));
		HexStrings.Add(TestUtils::RandString(100, 16));
		DecStrings.Add(TestUtils::RandString(100, 10));
	}

	std::initializer_list<int64> InterestingCases = {MAX_int32, MIN_int32, MAX_int64, MIN_int64, 0, 1, 2, -1, -2};

	TestUtils::AddToArray(Int64s, 20, InterestingCases);
	TestUtils::AddToArray(BigInts, 20, InterestingCases);
	TestUtils::AddToArray(HexStrings, 20, 16, InterestingCases);
	TestUtils::AddToArray(DecStrings, 20, 10, InterestingCases);

	TestUtils::Shuffle(Int64s);
	TestUtils::Shuffle(BigInts);
	TestUtils::Shuffle(HexStrings);
	TestUtils::Shuffle(DecStrings);

	{
		TEST_CASE_START("Comparison");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, Int64s[i + 1]);
			TEST_CASE_ADD_BIG(BigA, A);
			TEST_CASE_ADD_BIG(BigB, B);

			TEST_CASE(A == B, BigA == BigB);
			TEST_CASE(A != B, BigA != BigB);
			TEST_CASE(A >= B, BigA >= BigB);
			TEST_CASE(A <= B, BigA <= BigB);
			TEST_CASE(A > B, BigA > BigB);
			TEST_CASE(A < B, BigA < BigB);
		}
	}

	{
		TEST_CASE_START("Bit-wise OR & AND");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, Int64s[i + 1]);
			TEST_CASE_ADD_BIG(BigA, A);
			TEST_CASE_ADD_BIG(BigB, B);

			TEST_CASE(A | B, BigA | BigB);
			TEST_CASE(A & B, BigA & BigB);
		}
	}

	{
		TEST_CASE_START("Bit-wise NOT");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD_BIG(BigA, A);

			TEST_CASE(~A, ~BigA);
		}
	}

	{
		TEST_CASE_START("Inverting sign");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 1);
			TEST_CASE_ADD_BIG(BigA, A);

			TEST_CASE(+A, +BigA);
			TEST_CASE(-A, -BigA);
		}
	}

	{
		TEST_CASE_START("Increment");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 1);
			TEST_CASE_ADD_BIG(BigA, A);

			TEST_CASE(++A, ++BigA);
		}
	}

	{
		TEST_CASE_START("Decrement");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 1);
			TEST_CASE_ADD_BIG(BigA, A);

			TEST_CASE(--A, --BigA);
		}
	}

	{
		TEST_CASE_START("Add and subtract");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, Int64s[i + 1]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 2);
			OVERFLOW_PROTECTION_FOR_INT64(B, 2);
			TEST_CASE_ADD_BIG(BigA, A);
			TEST_CASE_ADD_BIG(BigB, B);

			TEST_CASE(A + B, BigA + BigB);
			TEST_CASE(A - B, BigA - BigB);
		}
	}

	{
		TEST_CASE_START("Binary left shift by 1");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 1);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftLeftByOne();
			TEST_CASE(A << 1, BigA);
		}
	}

	{
		TEST_CASE_START("Binary left shift by 5");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 5);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftLeft(5);
			TEST_CASE(A << 5, BigA);
		}
	}

	{
		TEST_CASE_START("Binary left shift by 32");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 32);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftLeft(32);
			TEST_CASE(A << 32, BigA);
		}
	}

	{
		TEST_CASE_START("Binary left shift by 33");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 33);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftLeft(33);
			TEST_CASE(A << 33, BigA);
		}
	}

	{
		TEST_CASE_START("Binary right shift by 1");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftRightByOne();
			TEST_CASE(A >> 1, BigA);
		}
	}

	{
		TEST_CASE_START("Binary right shift by 5");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftRight(5);
			TEST_CASE(A >> 5, BigA);
		}
	}

	{
		TEST_CASE_START("Binary right shift by 32");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftRight(32);
			TEST_CASE(A >> 32, BigA);
		}
	}

	{
		TEST_CASE_START("Binary right shift by 33");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD_BIG(BigA, A);

			BigA.ShiftRight(33);
			TEST_CASE(A >> 33, BigA);
		}
	}

	{
		TEST_CASE_START("Multiplication");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, Int64s[i + 1]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 32);
			OVERFLOW_PROTECTION_FOR_INT64(B, 32);
			TEST_CASE_ADD_BIG(BigA, A);
			TEST_CASE_ADD_BIG(BigB, B);

			TEST_CASE(A * B, BigA * BigB);
		}
	}

	{
		TEST_CASE_START("Division");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, Int64s[i + 1]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 1);
			OVERFLOW_PROTECTION_FOR_INT64(B, 1);

			if (B == 0)
			{
				continue;
			}

			TEST_CASE_ADD_BIG(BigA, A);
			TEST_CASE_ADD_BIG(BigB, B);

			TEST_CASE(A / B, BigA / BigB);
		}
	}

	{
		TEST_CASE_START("Modulo");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, Int64s[i + 1]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 1);
			OVERFLOW_PROTECTION_FOR_INT64(B, 1);

			if (B == 0)
			{
				continue;
			}

			TEST_CASE_ADD_BIG(BigA, A);
			TEST_CASE_ADD_BIG(BigB, B);

			TEST_CASE(A % B, BigA % BigB);
		}
	}

	{
		TEST_CASE_START("Power");
		for (int32 i = 0; i < Int64s.Num() - 1; ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			OVERFLOW_PROTECTION_FOR_INT64(A, 32);
			TEST_CASE_ADD_BIG(BigA, A);

			TEST_CASE(A * A, FPsDataBigInteger::Pow(BigA, 2));
		}
	}

	{
		TEST_CASE_START("Hex deserialize->serialize");
		for (int32 i = 0; i < HexStrings.Num(); ++i)
		{
			const auto HexConvertion = EPsDataBigIntegerConvertionType::Hex;
			TEST_CASE_ADD(StrA, HexStrings[i]);
			TEST_CASE_ADD(StrB, FPsDataBigInteger(StrA, HexConvertion).ToString(HexConvertion));

			TEST_CASE(StrA, StrB);
		}
	}

	{
		TEST_CASE_START("Dec deserialize->serialize");
		for (int32 i = 0; i < DecStrings.Num(); ++i)
		{
			TEST_CASE_ADD(StrA, DecStrings[i]);
			TEST_CASE_ADD(StrB, FPsDataBigInteger(StrA).ToString());

			TEST_CASE(StrA, StrB);
		}
	}

	{
		TEST_CASE_START("Deserialize to scientific notations");
		for (int32 i = 0; i < Int64s.Num(); ++i)
		{
			TEST_CASE_ADD(A, Int64s[i]);
			TEST_CASE_ADD(B, FString::Printf(TEXT("%e"), static_cast<double>(A)));
			TEST_CASE_ADD(C, static_cast<int64>(FCString::Atod(*B)));
			TEST_CASE_ADD(D, FPsDataBigInteger(B).ToInt64());

			TEST_CASE(C, D);
		}
	}

	TestUtils::TestCaseVariables::UseInt64ForDumpBigInt = false;

	/*
	{
		TEST_CASE_START("Hex serialize->deserialize");
		for (int32 i = 0; i < BigInts.Num(); ++i)
		{
			TEST_CASE_ADD(BigA, BigInts[i]);
			TEST_CASE_ADD(BigB, FPsDataBigInteger(BigA.ToString(EPsDataBigIntegerConvertionType::Hex), EPsDataBigIntegerConvertionType::Hex));

			TEST_CASE(BigA, BigB);
		}
	}
	*/

	{
		TEST_CASE_START("Dec serialize->deserialize");
		for (int32 i = 0; i < BigInts.Num(); ++i)
		{
			TEST_CASE_ADD(BigA, BigInts[i]);
			TEST_CASE_ADD(BigB, FPsDataBigInteger(BigA.ToString()));

			TEST_CASE(BigA, BigB);
		}
	}

	{
		TEST_CASE_START("Scientific notations serialize->deserialize");
		for (int32 i = 0; i < BigInts.Num(); ++i)
		{
			int32 NumDigits = FMath::Max(TestUtils::RandByte() % 19, 1);
			TEST_CASE_ADD(A, BigInts[i]);
			TEST_CASE_ADD(B, A.ToShortBigInteger(NumDigits).ToScientificNotation());
			TEST_CASE_ADD(C, FPsDataBigInteger(B).ToShortBigInteger(NumDigits).ToScientificNotation());

			TEST_CASE(B, C);
		}
	}

	{
		TEST_CASE_START("Big integer power");
		for (int32 i = 0; i < BigInts.Num(); ++i)
		{
			TEST_CASE_ADD(BigA, BigInts[i]);

			TEST_CASE(BigA * BigA * BigA, FPsDataBigInteger::Pow(BigA, 3));
		}
	}

	{
		TEST_CASE_START("Big integer sqrt");
		for (int32 i = 0; i < BigInts.Num(); ++i)
		{
			TEST_CASE_ADD(BigA, BigInts[i]);

			if (BigA.IsNegative())
			{
				continue;
			}

			TEST_CASE(BigA, FPsDataBigInteger::Sqrt(BigA * BigA));
		}
	}

	{
		TEST_CASE_START("Big integer great test");
		for (int32 i = 0; i < BigInts.Num() - 2; ++i)
		{
			TEST_CASE_ADD(A, BigInts[i]);
			TEST_CASE_ADD(B, BigInts[i + 1]);
			TEST_CASE_ADD(C, BigInts[i + 2]);

			if ((A * B * C) == 0)
			{
				continue;
			}

			B.Abs();

			TEST_CASE(FPsDataBigInteger::Pow(A, 2) * FPsDataBigInteger::Sqrt(B * B) - A - C, (((((A * B * C * A * B * C) / C) / A) / B) / C) * A - (A + C));
		}
	}
#endif
}