// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "PsDataCustomThunk.h"

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PsData_Enum.generated.h"

UCLASS(meta = (CustomThunkTemplates = "FCustomThunkTemplates_PsData"))
class PSDATA_API UPsDataEnumLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	/** Get property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetProperty(UPsData* Target, int32 Index, int32& Out);

	/** Set property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetProperty(UPsData* Target, int32 Index, int32 Value);

	/** Get array property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<int32>& Out);

	/** Set array property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetArrayProperty(UPsData* Target, int32 Index, const TArray<int32>& Value);

	/** Get map property */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, int32>& Out);

	/** Set map property */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataIn = "Value"))
	static void SetMapProperty(UPsData* Target, int32 Index, const TMap<FString, int32>& Value);

	/** Get link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetLinkValue(UPsData* Target, int32 Index, int32& Out);

	/** Get array link value */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "PsData|Data", meta = (PsDataTarget = "Target", PsDataIndex = "Index", PsDataOut = "Out"))
	static void GetArrayLinkValue(UPsData* Target, int32 Index, TArray<int32>& Out);

	DECLARE_FUNCTION(execGetProperty);
	DECLARE_FUNCTION(execSetProperty);
	DECLARE_FUNCTION(execGetArrayProperty);
	DECLARE_FUNCTION(execSetArrayProperty);
	DECLARE_FUNCTION(execGetMapProperty);
	DECLARE_FUNCTION(execSetMapProperty);
	DECLARE_FUNCTION(execGetLinkValue);
	DECLARE_FUNCTION(execGetArrayLinkValue);

public:
	static void TypeSerialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const uint8& Value);
	static uint8 TypeDeserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, const uint8& Value);

private:
	static FString* GetEnumString(UEnum* Enum, uint8 Value);
	static uint8* GetEnumValue(UEnum* Enum, const FString& String);

	static TMap<UEnum*, TMap<uint8, FString>> EnumValueToString;
	static TMap<UEnum*, TMap<FString, uint8>> EnumStringToValue;
};

/***********************************
 * Enum context
 ***********************************/
namespace PsDataTools
{
template <typename T>
UEnum* FindUEnum()
{
	static auto Enum = FindObject<UEnum>(ANY_PACKAGE, *FType<T>::ContentType());
	return Enum;
}

template <typename T>
struct FEnumDataTypeContext : public TDataTypeContextExtended<T, UPsDataEnumLibrary>
{
	static_assert(std::is_enum<T>::value && sizeof(T) == 1, "Only \"enum class : uint8\" can be described by DESCRIBE_ENUM macros");

	virtual UField* GetUEType() const override
	{
		return FindUEnum<T>();
	}

	bool HasExtendedTypeCheck() const override
	{
		return true;
	}

	virtual bool IsEnum() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		static constexpr const uint32 Hash0 = FType<uint8>::Hash();
		static constexpr const uint32 Hash1 = FType<T>::Hash();
		const uint32 RContextHash = RightContext->GetHash();
		return Hash0 == RContextHash || Hash1 == RContextHash;
	}
};

template <typename T>
struct FEnumDataTypeContext<TArray<T>> : public TDataTypeContextExtended<TArray<T>, UPsDataEnumLibrary>
{
	static_assert(std::is_enum<T>::value, "Only \"enum class : uint8\" can be described by DESCRIBE_ENUM macros");

	virtual UField* GetUEType() const override
	{
		return FindUEnum<T>();
	}

	bool HasExtendedTypeCheck() const override
	{
		return true;
	}

	virtual bool IsEnum() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		static constexpr const uint32 Hash0 = FType<TArray<uint8>>::Hash();
		static constexpr const uint32 Hash1 = FType<TArray<T>>::Hash();
		const uint32 RContextHash = RightContext->GetHash();
		return Hash0 == RContextHash || Hash1 == RContextHash;
	}
};

template <typename T>
struct FEnumDataTypeContext<TMap<FString, T>> : public TDataTypeContextExtended<TMap<FString, T>, UPsDataEnumLibrary>
{
	static_assert(std::is_enum<T>::value, "Only \"enum class : uint8\" can be described by DESCRIBE_ENUM macros");

	virtual UField* GetUEType() const override
	{
		return FindUEnum<T>();
	}

	bool HasExtendedTypeCheck() const override
	{
		return true;
	}

	virtual bool IsEnum() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		static constexpr const uint32 Hash0 = FType<TMap<FString, uint8>>::Hash();
		static constexpr const uint32 Hash1 = FType<TMap<FString, T>>::Hash();
		const uint32 RContextHash = RightContext->GetHash();
		return Hash0 == RContextHash || Hash1 == RContextHash;
	}
};
} // namespace PsDataTools

/***********************************
 * Macro DESCRIBE_ENUM
 ***********************************/

#define DESCRIBE_ENUM(__Type__)                                                                                                                \
	namespace PsDataTools                                                                                                                      \
	{                                                                                                                                          \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TTypeToString<__Type__>                                                                                                             \
	{                                                                                                                                          \
		static FString ToString(__Type__ Value)                                                                                                \
		{                                                                                                                                      \
			return FindUEnum<__Type__>()->GetNameStringByValue(static_cast<int64>(Value));                                                     \
		}                                                                                                                                      \
	};                                                                                                                                         \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TDataTypeContext<__Type__> : public FEnumDataTypeContext<__Type__>                                                                  \
	{                                                                                                                                          \
		virtual ~TDataTypeContext() {}                                                                                                         \
	};                                                                                                                                         \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TDataTypeContext<TArray<__Type__>> : public FEnumDataTypeContext<TArray<__Type__>>                                                  \
	{                                                                                                                                          \
		virtual ~TDataTypeContext() {}                                                                                                         \
	};                                                                                                                                         \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TDataTypeContext<TMap<FString, __Type__>> : public FEnumDataTypeContext<TMap<FString, __Type__>>                                    \
	{                                                                                                                                          \
		virtual ~TDataTypeContext() {}                                                                                                         \
	};                                                                                                                                         \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TTypeDefault<__Type__>                                                                                                              \
	{                                                                                                                                          \
		static const __Type__ GetDefaultValue() { return static_cast<__Type__>(0); }                                                           \
	};                                                                                                                                         \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TTypeSerializer<__Type__>                                                                                                           \
	{                                                                                                                                          \
		static void Serialize(const UPsData* const Instance, const FDataField* Field, FPsDataSerializer* Serializer, const __Type__ Value)     \
		{                                                                                                                                      \
			UPsDataEnumLibrary::TypeSerialize(Instance, Field, Serializer, static_cast<uint8>(Value));                                         \
		}                                                                                                                                      \
	};                                                                                                                                         \
                                                                                                                                               \
	template <>                                                                                                                                \
	struct TTypeDeserializer<__Type__>                                                                                                         \
	{                                                                                                                                          \
		static __Type__ Deserialize(const UPsData* const Instance, const FDataField* Field, FPsDataDeserializer* Deserializer, __Type__ Value) \
		{                                                                                                                                      \
			return static_cast<__Type__>(UPsDataEnumLibrary::TypeDeserialize(Instance, Field, Deserializer, static_cast<uint8>(Value)));       \
		}                                                                                                                                      \
	};                                                                                                                                         \
	} // namespace PsDataTools
