// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

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

FPsDataBigInteger::FPsDataBigInteger(const FString& Value)
{
	if (auto Result = PsDataTools::Numbers::ToNumber<FPsDataBigInteger>(PsDataTools::ToStringView(Value)))
	{
		*this = *Result;
	}
	else
	{
		UE_LOG(LogDataUtils, Fatal, TEXT("Can't deserialize \"%s\" to FPsDataBigInteger"), *Value);
	}
}

FPsDataBigInteger::FPsDataBigInteger(const char* Value)
{
	if (auto Result = PsDataTools::Numbers::ToNumber<FPsDataBigInteger>(PsDataTools::ToStringView(Value)))
	{
		*this = *Result;
	}
	else
	{
		UE_LOG(LogDataUtils, Fatal, TEXT("Can't deserialize \"%s\" to FPsDataBigInteger"), *Value);
	}
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

FString FPsDataBigInteger::ToString() const
{
	return PsDataTools::Numbers::ToString(*this);
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

void FPsDataBigInteger::Add(FPsDataBigInteger Value)
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

void FPsDataBigInteger::Subtract(FPsDataBigInteger Value)
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

void FPsDataBigInteger::Modulo(FPsDataBigInteger Divisor)
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

void FPsDataBigInteger::BitwiseOr(FPsDataBigInteger Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords());
	Allocate(NumWords);

	for (int32 i = 0; i < NumWords; ++i)
	{
		Words[i] = GetWord(i) | Value.GetWord(i);
	}
}

void FPsDataBigInteger::BitwiseAnd(FPsDataBigInteger Value)
{
	const auto NumWords = FMath::Max(GetNumWords(), Value.GetNumWords());
	Allocate(NumWords);

	for (int32 i = 0; i < NumWords; ++i)
	{
		Words[i] = GetWord(i) & Value.GetWord(i);
	}
}

void FPsDataBigInteger::BitwiseXor(FPsDataBigInteger Value)
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
		return Zero;
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

FPsDataBigInteger FPsDataBigInteger::FromString(const FString& Value)
{
	if (auto Result = PsDataTools::Numbers::ToNumber<FPsDataBigInteger>(PsDataTools::ToStringView(Value)))
	{
		return *Result;
	}

	UE_LOG(LogDataUtils, Warning, TEXT("Can't deserialize \"%s\" to FPsDataBigInteger"), *Value);

	return Zero;
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

FString FPsDataShortBigInteger::ToScientificNotation() const
{
	TArray<TCHAR> Result;
	PsDataTools::Numbers::ToString(Base, Result);
	Result.Add('e');
	PsDataTools::Numbers::ToString(PowerOfTen, Result);
	return FString(std::move(Result));
}

FPsDataShortBigInteger FPsDataShortBigInteger::FromString(const FString& String, int32 NumDigits)
{
	return FPsDataBigInteger(String).ToShortBigInteger(NumDigits);
}

bool FPsDataBigInteger::ExportTextItem(FString& ValueStr, FPsDataBigInteger const& DefaultValue, UObject* Parent, int32 PortFlags, UObject* ExportRootScope) const
{
	ValueStr = ToString();
	return true;
}

bool FPsDataBigInteger::ImportTextItem(const TCHAR*& Buffer, int32 PortFlags, UObject* Parent, FOutputDevice* ErrorText)
{
	auto Result = PsDataTools::Numbers::ToNumber<FPsDataBigInteger>(PsDataTools::ToStringView(Buffer));
	if (Result)
	{
		*this = *Result;
		return true;
	}

	return false;
}