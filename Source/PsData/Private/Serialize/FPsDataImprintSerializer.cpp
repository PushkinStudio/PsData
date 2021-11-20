// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "Serialize/FPsDataImprintSerializer.h"

#include "PsData.h"

/***********************************
 * FPsDataImprintChild
 ***********************************/

FPsDataImprintChild::FPsDataImprintChild(int32 InOffset, const UPsData* InData)
	: Offset(InOffset)
	, Data(InData)
{
}

int32 FPsDataImprintChild::GetOffsets() const
{
	return Offset;
}

const UPsData* FPsDataImprintChild::GetData() const
{
	return Data;
}

/***********************************
 * FPsDataImprint
 ***********************************/

FPsDataImprint::FPsDataImprint()
	: bValid(false)
	, Buffer(MakeShared<FPsDataBufferOutputStream>())
{
}

void FPsDataImprint::Prepare()
{
	check(Buffer->Size() > 0);

	FPsDataMD5OutputStream OutputStream;
	OutputStream.WriteBuffer(Buffer->GetBuffer());
	Hash = OutputStream.GetHash();

	Children.StableSort([](const FPsDataImprintChild& A, const FPsDataImprintChild& B) {
		return A.GetData()->GetFullKey() < B.GetData()->GetFullKey();
	});

	bValid = true;
}

void FPsDataImprint::Reset()
{
	Buffer->Reset();
	Children.Reset();
	Hash.Reset();

	bValid = false;
}

bool FPsDataImprint::IsSet() const
{
	return bValid;
}

TSharedRef<FPsDataBufferOutputStream> FPsDataImprint::GetBufferOutputStream()
{
	return Buffer;
}

const TArray<uint8>& FPsDataImprint::GetBuffer() const
{
	return Buffer->GetBuffer();
}

TArray<FPsDataImprintChild>& FPsDataImprint::GetChildren()
{
	return Children;
}

const TArray<FPsDataImprintChild>& FPsDataImprint::GetChildren() const
{
	return Children;
}

int32 FPsDataImprint::NumChildren() const
{
	return Children.Num();
}

bool FPsDataImprint::HasChildren() const
{
	return NumChildren() > 0;
}

const FPsDataMD5Hash& FPsDataImprint::GetHash() const
{
	return Hash.GetValue();
}

/***********************************
 * FPsDataImprintBinarySerializer
 ***********************************/

uint32 FPsDataImprintBinarySerializer::Concatenate(TSharedRef<FPsDataBufferOutputStream>& OutputStream, const UPsData* Data)
{
	const auto& Imprint = PsDataTools::FPsDataFriend::GetImprint(Data);
	if (Imprint.HasChildren())
	{
		TArray<uint32> ChildrenOffsets;
		ChildrenOffsets.Reserve(Imprint.NumChildren());

		for (auto Child : Imprint.GetChildren())
		{
			const auto Offset = Concatenate(OutputStream, Child.GetData());
			ChildrenOffsets.Add(Offset);
		}

		const auto ResultOffset = OutputStream->Size();
		TArray<uint8> BufferCopy = Imprint.GetBuffer();
		auto BufferCopyPtr = BufferCopy.GetData();
		const auto& Children = Imprint.GetChildren();
		for (int32 i = 0; i < ChildrenOffsets.Num(); ++i)
		{
			const auto ChildOffset = ChildrenOffsets[i];
			const auto BufferOffset = Children[i].GetOffsets();

			if (BufferCopyPtr[BufferOffset] == 0)
			{
				BufferCopyPtr[BufferOffset] = EBinaryTokens::Redirect;
				BufferCopyPtr[BufferOffset + 1] = static_cast<uint8>(ChildOffset >> 24);
				BufferCopyPtr[BufferOffset + 2] = static_cast<uint8>(ChildOffset >> 16);
				BufferCopyPtr[BufferOffset + 3] = static_cast<uint8>(ChildOffset >> 8);
				BufferCopyPtr[BufferOffset + 4] = static_cast<uint8>(ChildOffset);
			}
			else
			{
				checkNoEntry();
			}
		}

		OutputStream->WriteBuffer(std::move(BufferCopy));
		OutputStream->WriteUint8(EBinaryTokens::RedirectEnd);
		return ResultOffset;
	}
	else
	{
		const auto ResultOffset = OutputStream->Size();
		OutputStream->WriteBuffer(Imprint.GetBuffer());
		OutputStream->WriteUint8(EBinaryTokens::RedirectEnd);
		return ResultOffset;
	}
}

FPsDataImprintBinarySerializer::FPsDataImprintBinarySerializer(FPsDataImprint* InImprint)
	: FPsDataBinarySerializer(InImprint->GetBufferOutputStream())
	, Imprint(InImprint)
{
}

void FPsDataImprintBinarySerializer::WriteValue(const UPsData* Value)
{
	if (Value == nullptr)
	{
		OutputStream->WriteUint8(EBinaryTokens::Value_null);
	}
	else
	{
		Imprint->GetChildren().Add({OutputStream->Size(), Value});
		OutputStream->WriteUint8(0);
		OutputStream->WriteUint32(0);
	}
}

/***********************************
 * FPsDataImprintBinaryDeserializer
 ***********************************/

FPsDataImprintBinaryDeserializer::FPsDataImprintBinaryDeserializer(TSharedRef<FPsDataBufferInputStream> InInputStream, int32 InOffset)
	: FPsDataBinaryDeserializer(InInputStream)
{
	InInputStream->SetPosition(InOffset);
}

uint8 FPsDataImprintBinaryDeserializer::ReadToken()
{
	const auto Token = FPsDataBinaryDeserializer::ReadToken();
	if (TryToRedirect(Token))
	{
		return ReadToken();
	}

	return Token;
}

bool FPsDataImprintBinaryDeserializer::TryToRedirect(uint8 Token)
{
	if (Token == EBinaryTokens::Redirect)
	{
		const auto Offset = InputStream->ReadUint32();
		ReturnPositions.Push(InputStream->GetPosition());
		InputStream->SetPosition(Offset);
		return true;
	}
	else if (Token == EBinaryTokens::RedirectEnd && ReturnPositions.Num() > 0)
	{
		InputStream->SetPosition(ReturnPositions.Pop());
		return true;
	}

	return false;
}

/***********************************
 * FPsDataImprintBinaryConvertor
 ***********************************/

FPsDataImprintBinaryConvertor::FPsDataImprintBinaryConvertor(FPsDataImprintBinaryDeserializer* InDeserializer)
	: Deserializer(InDeserializer)
{
}

void FPsDataImprintBinaryConvertor::Convert(FPsDataSerializer* Serializer)
{
	bool bRunning = true;
	const auto InputStream = Deserializer->GetInputStream();
	while (bRunning)
	{
		const auto Token = Deserializer->ReadToken();
		InputStream->ShiftBack();

		switch (Token)
		{
		case EBinaryTokens::KeyBegin:
			ReadKey(Serializer);
			break;
		case EBinaryTokens::KeyEnd:
			PopKey(Serializer);
			break;
		case EBinaryTokens::ArrayBegin:
			ReadArray(Serializer);
			break;
		case EBinaryTokens::ArrayEnd:
			PopArray(Serializer);
			break;
		case EBinaryTokens::ObjectBegin:
			ReadObject(Serializer);
			break;
		case EBinaryTokens::ObjectEnd:
			PopObject(Serializer);
			break;
		case EBinaryTokens::Value_uint8:
			ReadValue<uint8>(Serializer);
			break;
		case EBinaryTokens::Value_int32:
			ReadValue<int32>(Serializer);
			break;
		case EBinaryTokens::Value_int64:
			ReadValue<int64>(Serializer);
			break;
		case EBinaryTokens::Value_float:
			ReadValue<float>(Serializer);
			break;
		case EBinaryTokens::Value_bool:
			ReadValue<bool>(Serializer);
			break;
		case EBinaryTokens::Value_FString:
			ReadValue<FString>(Serializer);
			break;
		case EBinaryTokens::Value_FName:
			ReadValue<FName>(Serializer);
			break;
		case EBinaryTokens::Value_null:
			ReadNull(Serializer);
			break;
		default:
			bRunning = false;
		}
	}
}

void FPsDataImprintBinaryConvertor::ReadKey(FPsDataSerializer* Serializer)
{
	FString Key;
	Deserializer->ReadKey(Key);
	Serializer->WriteKey(Key);
	Keys.Push(Key);
}

void FPsDataImprintBinaryConvertor::ReadArray(FPsDataSerializer* Serializer)
{
	Deserializer->ReadArray();
	Serializer->WriteArray();
}

void FPsDataImprintBinaryConvertor::ReadObject(FPsDataSerializer* Serializer)
{
	Deserializer->ReadObject();
	Serializer->WriteObject();
}

void FPsDataImprintBinaryConvertor::PopKey(FPsDataSerializer* Serializer)
{
	if (Keys.Num() > 0)
	{
		const auto Key = Keys.Pop();
		Deserializer->PopKey(Key);
		Serializer->PopKey(Key);
	}
	else
	{
		checkNoEntry();
	}
}

void FPsDataImprintBinaryConvertor::PopArray(FPsDataSerializer* Serializer)
{
	Deserializer->PopArray();
	Serializer->PopArray();
}

void FPsDataImprintBinaryConvertor::PopObject(FPsDataSerializer* Serializer)
{
	Deserializer->PopObject();
	Serializer->PopObject();
}

void FPsDataImprintBinaryConvertor::ReadNull(FPsDataSerializer* Serializer)
{
	Deserializer->ReadToken();
	Serializer->WriteValue(nullptr);
}
