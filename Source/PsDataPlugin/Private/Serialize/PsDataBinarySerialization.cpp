// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataBinarySerialization.h"

#include "PsData.h"
#include "PsDataUtils.h"

/***********************************
 * Buffer
 ***********************************/

void Reserve(TArray<char>& Buffer)
{
	const int32 Allocation = 10 * 1024;
	if (Buffer.GetSlack() < 1024)
	{
		Buffer.Reserve(Buffer.Num() + Allocation);
	}
}

template <typename T>
void Write(TArray<char>& Buffer, const T& Data)
{
	const uint32 Size = sizeof(T);
	char* Bytes = (char*)&Data;
	for (int32 i = 0; i < Size; ++i)
	{
		Buffer.Add(Bytes[i]);
	}
}

template <>
void Write<FString>(TArray<char>& Buffer, const FString& Data)
{
	const int32 MaxSize = (Data.Len() + 1) * 4;
	if (Buffer.GetSlack() <= MaxSize)
	{
		Buffer.Reserve(Buffer.Num() + MaxSize);
	}
	ANSICHAR Subbuffer[4];
	for (int32 i = 0; i < Data.Len(); ++i)
	{
		int32 Count = FTCHARToUTF8_Convert::Utf8FromCodepoint(Data[i], Subbuffer, sizeof(Subbuffer));
		for (int32 j = 0; j < Count; ++j)
		{
			Buffer.Add(Subbuffer[j]);
		}
	}
	Write(Buffer, int32(0)); // end
}

/***********************************
 * FBinaryDataSerializer
 ***********************************/

FPsDataBinarySerializer::FPsDataBinarySerializer()
	: FPsDataSerializer()
{
	Reserve(Buffer);
}

const TArray<char>& FPsDataBinarySerializer::GetBuffer() const
{
	return Buffer;
}

/***********************************
 * int32
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, int32 Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data);
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		Write(Buffer, Data[i]);
	}
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (auto& Pair : Data)
	{
		Write(Buffer, Pair.Key);
		Write(Buffer, Pair.Value);
	}
}

/***********************************
 * uint8
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, uint8 Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data);
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		Write(Buffer, Data[i]);
	}
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (auto& Pair : Data)
	{
		Write(Buffer, Pair.Key);
		Write(Buffer, Pair.Value);
	}
}

/***********************************
 * float
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, float Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data);
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<float>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		Write(Buffer, Data[i]);
	}
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (auto& Pair : Data)
	{
		Write(Buffer, Pair.Key);
		Write(Buffer, Pair.Value);
	}
}

/***********************************
 * string
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const FString& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data);
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		Write(Buffer, Data[i]);
	}
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (auto& Pair : Data)
	{
		Write(Buffer, Pair.Key);
		Write(Buffer, Pair.Value);
	}
}

/***********************************
 * bool
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, bool Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, static_cast<char>(Data));
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		Write(Buffer, static_cast<char>(Data[i]));
	}
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (auto& Pair : Data)
	{
		Write(Buffer, Pair.Key);
		Write(Buffer, static_cast<char>(Pair.Value));
	}
}

/***********************************
 * Data
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, UPsData* Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Data->DataSerialize(this);
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (int32 i = 0; i < Data.Num(); ++i)
	{
		Data[i]->DataSerialize(this);
	}
}

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data)
{
	Reserve(Buffer);
	Write(Buffer, Field->Hash);
	Write(Buffer, Data.Num());
	for (auto& Pair : Data)
	{
		Write(Buffer, Pair.Key);
		Pair.Value->DataSerialize(this);
	}
}

/***********************************
 * Custom
 ***********************************/

void FPsDataBinarySerializer::Serialize(const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data)
{
	Reserve(Buffer);
	if (Field.IsValid())
	{
		Write(Buffer, Field->Hash);
	}
	if (Data.IsValid() && !Data->IsNull())
	{
		switch (Data->Type)
		{
		case EJson::String:
		{
			Write(Buffer, char(1));
			Write(Buffer, Data->AsString());
			break;
		}
		case EJson::Number:
		{
			Write(Buffer, char(2));
			Write(Buffer, Data->AsNumber());
			break;
		}
		case EJson::Boolean:
		{
			Write(Buffer, char(3));
			Write(Buffer, Data->AsBool());

			break;
		}
		case EJson::Array:
		{
			Write(Buffer, char(4));
			const auto& JsonArray = Data->AsArray();
			Write(Buffer, JsonArray.Num());
			for (int32 i = 0; i < JsonArray.Num(); ++i)
			{
				Serialize(nullptr, JsonArray[i]);
			}
			break;
		}
		case EJson::Object:
		{
			Write(Buffer, char(5));
			const auto& JsonObject = Data->AsObject();
			Write(Buffer, JsonObject->Values.Num());
			for (auto Pair : JsonObject->Values)
			{
				Write(Buffer, Pair.Key);
				Serialize(nullptr, Pair.Value);
			}
			break;
		}
		default:
			Write(Buffer, char(0));
			break;
		}
	}
	else
	{
		Write(Buffer, char(0));
	}
}

/***********************************
 * FBinaryDataDeserializer
 ***********************************/

FPsDataBinaryDeserializer::FPsDataBinaryDeserializer()
	: FPsDataDeserializer()
{
}

bool FPsDataBinaryDeserializer::Has(const TSharedPtr<const FDataField>& Field)
{
	return false;
}

/***********************************
 * int32
 ***********************************/

int32 FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, int32 Data, UClass* DataClass)
{
	return 0;
}

TArray<int32> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<int32>& Data, UClass* DataClass)
{
	return TArray<int32>();
}

TMap<FString, int32> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, int32>& Data, UClass* DataClass)
{
	return TMap<FString, int32>();
}

/***********************************
 * uint8
 ***********************************/

uint8 FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, uint8 Data, UClass* DataClass)
{
	return 0;
}

TArray<uint8> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<uint8>& Data, UClass* DataClass)
{
	return TArray<uint8>();
}

TMap<FString, uint8> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, uint8>& Data, UClass* DataClass)
{
	return TMap<FString, uint8>();
}

/***********************************
 * float
 ***********************************/

float FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, float Data, UClass* DataClass)
{
	return 0.f;
}

TArray<float> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<float>& Data, UClass* DataClass)
{
	return TArray<float>();
}

TMap<FString, float> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, float>& Data, UClass* DataClass)
{
	return TMap<FString, float>();
}

/***********************************
 * string
 ***********************************/

FString FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const FString& Data, UClass* DataClass)
{
	return TEXT("");
}

TArray<FString> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<FString>& Data, UClass* DataClass)
{
	return TArray<FString>();
}

TMap<FString, FString> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, FString>& Data, UClass* DataClass)
{
	return TMap<FString, FString>();
}

/***********************************
 * bool
 ***********************************/

bool FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, bool Data, UClass* DataClass)
{
	return false;
}

TArray<bool> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<bool>& Data, UClass* DataClass)
{
	return TArray<bool>();
}

TMap<FString, bool> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, bool>& Data, UClass* DataClass)
{
	return TMap<FString, bool>();
}

/***********************************
 * Data
 ***********************************/

UPsData* FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, UPsData* Data, UClass* DataClass)
{
	return nullptr;
}

TArray<UPsData*> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TArray<UPsData*>& Data, UClass* DataClass)
{
	return TArray<UPsData*>();
}

TMap<FString, UPsData*> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, const TMap<FString, UPsData*>& Data, UClass* DataClass)
{
	return TMap<FString, UPsData*>();
}

/***********************************
 * Custom
 ***********************************/

TSharedPtr<FJsonValue> FPsDataBinaryDeserializer::Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TSharedPtr<FJsonValue> Data, UClass* DataClass)
{
	return TSharedPtr<FJsonValue>();
}
