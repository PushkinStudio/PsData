// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "Serialize/PsDataSerialization.h"

#include "PsData.h"

/***********************************
 * FPsDataAllocator
 ***********************************/

FPsDataAllocator::FPsDataAllocator(UClass* InClass, UPsData* InParent)
	: Class(InClass)
	, Parent(InParent)
{
}

FPsDataAllocator::FPsDataAllocator(UField* InField, UPsData* InParent)
	: Class(CastChecked<UClass>(InField))
	, Parent(InParent)
{
}

FPsDataAllocator::FPsDataAllocator()
	: Class(nullptr)
	, Parent(nullptr)
{
}

UPsData* FPsDataAllocator::operator()() const
{
	if (Class != nullptr)
	{
		return NewObject<UPsData>(Parent, Class);
	}
	else
	{
		checkNoEntry();
		return nullptr;
	}
}

/***********************************
 * FPsDataSerializer
 ***********************************/

FPsDataSerializer::FPsDataSerializer()
	: bWriteDefaults(false)
{
}

void FPsDataSerializer::WriteValue(double Value)
{
	WriteValue(static_cast<float>(Value));
}

/***********************************
 * FPsDataDeserializer
 ***********************************/

FPsDataDeserializer::FPsDataDeserializer()
{
}

bool FPsDataDeserializer::ReadValue(double& OutValue)
{
	float Value = 0.f;
	if (ReadValue(Value))
	{
		OutValue = Value;
		return true;
	}

	return false;
}

EPsDataDeserializedValueType FPsDataDeserializer::NextValueType()
{
	return EPsDataDeserializedValueType::None;
}

namespace PsDataTools
{
template <typename T>
bool TranslateValue(FPsDataDeserializer* Deserializer, FPsDataSerializer* Serializer)
{
	T Value{};
	if (Deserializer->ReadValue(Value))
	{
		Serializer->WriteValue(Value);
		return true;
	}

	return false;
}
} // namespace PsDataTools

bool FPsDataDeserializer::Translate(FPsDataSerializer* Serializer)
{
	using namespace PsDataTools;

	switch (NextValueType())
	{
	case EPsDataDeserializedValueType::Null:
	{
		UPsData* Data = nullptr;
		if (ReadValue(Data, {}) && Data == nullptr)
		{
			Serializer->WriteValue(static_cast<const UPsData*>(nullptr));
			return true;
		}

		return false;
	}

	case EPsDataDeserializedValueType::Object:
	{
		if (!ReadObject())
		{
			return false;
		}

		Serializer->WriteObject();

		bool bSuccess = true;
		FString Key;
		while (bSuccess && ReadKey(Key))
		{
			Serializer->WriteKey(Key);
			bSuccess = Translate(Serializer);
			PopKey(Key);
			Serializer->PopKey(Key);
		}

		PopObject();
		Serializer->PopObject();
		return bSuccess;
	}

	case EPsDataDeserializedValueType::Array:
	{
		if (!ReadArray())
		{
			return false;
		}

		Serializer->WriteArray();

		bool bSuccess = true;
		while (bSuccess && ReadIndex())
		{
			bSuccess = Translate(Serializer);
			PopIndex();
		}

		PopArray();
		Serializer->PopArray();
		return bSuccess;
	}

	case EPsDataDeserializedValueType::Int32:
		return TranslateValue<int32>(this, Serializer);
	case EPsDataDeserializedValueType::Int64:
		return TranslateValue<int64>(this, Serializer);
	case EPsDataDeserializedValueType::Uint8:
		return TranslateValue<uint8>(this, Serializer);
	case EPsDataDeserializedValueType::Float:
		return TranslateValue<float>(this, Serializer);
	case EPsDataDeserializedValueType::Double:
		return TranslateValue<double>(this, Serializer);
	case EPsDataDeserializedValueType::Boolean:
		return TranslateValue<bool>(this, Serializer);

	case EPsDataDeserializedValueType::String:
		// The binary format keeps FName apart from FString; both come out as strings
		return TranslateValue<FString>(this, Serializer) || TranslateValue<FName>(this, Serializer);

	case EPsDataDeserializedValueType::None:
	default:
		return false;
	}
}
