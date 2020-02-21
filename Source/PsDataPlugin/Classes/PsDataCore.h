// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataFunctionLibrary.h"
#include "PsDataMemory.h"
#include "PsDataTraits.h"
#include "PsDataUtils.h"

#include "CoreMinimal.h"
#include "UObject/Package.h"

/***********************************
 * FDataReflection
 ***********************************/

namespace FDataReflectionTools
{
template <typename Type, int32 Hash>
struct FDProp;
struct FDMeta;
template <typename Type, class ReturnType, int32 Hash>
struct FDLinkBase;
} // namespace FDataReflectionTools

struct PSDATAPLUGIN_API FDataReflection
{
private:
	static TMap<UClass*, TMap<FString, const TSharedPtr<const FDataField>>> FieldsByName;
	static TMap<UClass*, TMap<int32, const TSharedPtr<const FDataField>>> FieldsByHash;
	static TMap<UClass*, TMap<FString, const TSharedPtr<const FDataLink>>> LinksByName;
	static TMap<UClass*, TMap<int32, const TSharedPtr<const FDataLink>>> LinksByHash;
	static TMap<FString, const TArray<FString>> SplittedPath;

	static TArray<UClass*> ClassQueue;
	static TArray<const char*> MetaCollection;

	static bool bCompiled;

protected:
	template <typename Type, int32 Hash>
	friend struct FDataReflectionTools::FDProp;
	friend struct FDataReflectionTools::FDMeta;
	template <typename Type, class ReturnType, int32 Hash>
	friend struct FDataReflectionTools::FDLinkBase;
	friend class UPsData;

	static void AddField(const char* CharName, int32 Hash, FAbstractDataTypeContext* Context);
	static void AddLink(const char* CharName, const char* CharPath, const char* CharReturnType, int32 Hash, bool bAbstract, bool bCollection);
	static void AddToQueue(UPsData* Instance);
	static void RemoveFromQueue(UPsData* Instance);
	static bool InQueue();
	static UClass* GetLastClassInQueue();
	static void PushMeta(const char* Meta);
	static void ClearMeta();
	static void Fill(UPsData* Instance);

public:
	static const TSharedPtr<const FDataField>& GetFieldByName(UClass* OwnerClass, const FString& Name);
	static const TSharedPtr<const FDataField>& GetFieldByHash(UClass* OwnerClass, int32 Hash);
	static const TMap<FString, const TSharedPtr<const FDataField>>& GetFields(const UClass* OwnerClass);
	static TSharedPtr<const FDataLink> GetLinkByName(UClass* OwnerClass, const FString& Name);
	static TSharedPtr<const FDataLink> GetLinkByHash(UClass* OwnerClass, int32 Hash);
	static const TMap<FString, const TSharedPtr<const FDataLink>>& GetLinks(UClass* OwnerClass);
	static bool HasClass(const UClass* OwnerClass);

	static void Compile();

	static const TArray<FString>& SplitPath(const FString& Path);
};

/***********************************
 * Macro with comma
 ***********************************/

#define COMMA ,

/***********************************
 * Private macro for describe function
 ***********************************/

#define _DFUNC(__Library__, __Type__, __StringType__)                                                                                    \
                                                                                                                                         \
	virtual TSharedPtr<const FDataFieldFunctions> GetUFunctions() const override                                                         \
	{                                                                                                                                    \
		static TSharedPtr<const FDataFieldFunctions> Functions(                                                                          \
			new FDataFieldFunctions(                                                                                                     \
				__Library__::StaticClass()->FindFunctionByName(FName(*FString::Printf(TEXT("Get%sProperty"), TEXT(#__StringType__)))),   \
				__Library__::StaticClass()->FindFunctionByName(FName(*FString::Printf(TEXT("Set%sProperty"), TEXT(#__StringType__)))))); \
		return Functions;                                                                                                                \
	}                                                                                                                                    \
                                                                                                                                         \
	virtual FAbstractDataMemory* AllocateMemory() const override                                                                         \
	{                                                                                                                                    \
		return new FDataMemory<__Type__>();                                                                                              \
	}                                                                                                                                    \
                                                                                                                                         \
	virtual const FString& GetCppType() const override                                                                                   \
	{                                                                                                                                    \
		return FDataReflectionTools::FType<__Type__>::Type();                                                                            \
	}                                                                                                                                    \
                                                                                                                                         \
	virtual const FString& GetCppContentType() const override                                                                            \
	{                                                                                                                                    \
		return FDataReflectionTools::FType<__Type__>::ContentType();                                                                     \
	}                                                                                                                                    \
                                                                                                                                         \
	virtual uint32 GetHash() const override                                                                                              \
	{                                                                                                                                    \
		static constexpr const uint32 Hash = FDataReflectionTools::FType<__Type__>::Hash();                                              \
		return Hash;                                                                                                                     \
	}                                                                                                                                    \
                                                                                                                                         \
	virtual ~FDataTypeContext() {}

/***********************************
 * Base context
 ***********************************/

template <typename T>
struct FDataTypeContext : public FAbstractDataTypeContext
{
	static_assert(FDataReflectionTools::TAlwaysFalse<T>::value, "Unsupported type");
};

/***********************************
 * int32 context
 ***********************************/

template <>
struct FDataTypeContext<int32> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, int32, Int);
};

template <>
struct FDataTypeContext<TArray<int32>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<int32>, IntArray);
};

template <>
struct FDataTypeContext<TMap<FString, int32>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA int32>, IntMap);
};

/***********************************
 * uint8 context
 ***********************************/

template <>
struct FDataTypeContext<uint8> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, uint8, Byte);
};

template <>
struct FDataTypeContext<TArray<uint8>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<uint8>, ByteArray);
};

template <>
struct FDataTypeContext<TMap<FString, uint8>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA uint8>, ByteMap);
};

/***********************************
 * int64 context
 ***********************************/

template <>
struct FDataTypeContext<int64> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, int64, Int64);
};

template <>
struct FDataTypeContext<TArray<int64>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<int64>, Int64Array);
};

template <>
struct FDataTypeContext<TMap<FString, int64>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA int64>, Int64Map);
};

/***********************************
 * float context
 ***********************************/

template <>
struct FDataTypeContext<float> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, float, Float);
};

template <>
struct FDataTypeContext<TArray<float>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<float>, FloatArray);
};

template <>
struct FDataTypeContext<TMap<FString, float>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA float>, FloatMap);
};

/***********************************
 * bool context
 ***********************************/

template <>
struct FDataTypeContext<bool> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, bool, Bool);
};

template <>
struct FDataTypeContext<TArray<bool>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<bool>, BoolArray);
};

template <>
struct FDataTypeContext<TMap<FString, bool>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA bool>, BoolMap);
};

/***********************************
 * String context
 ***********************************/

template <>
struct FDataTypeContext<FString> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, FString, String);
};

template <>
struct FDataTypeContext<TArray<FString>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<FString>, StringArray);
};

template <>
struct FDataTypeContext<TMap<FString, FString>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA FString>, StringMap);
};

/***********************************
 * FText context
 ***********************************/

template <>
struct FDataTypeContext<FText> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, FText, Text);
};

template <>
struct FDataTypeContext<TArray<FText>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<FText>, TextArray);
};

template <>
struct FDataTypeContext<TMap<FString, FText>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA FText>, TextMap);
};

/***********************************
 * FName context
 ***********************************/

template <>
struct FDataTypeContext<FName> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, FName, Name);
};

template <>
struct FDataTypeContext<TArray<FName>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<FName>, NameArray);
};

template <>
struct FDataTypeContext<TMap<FString, FName>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA FName>, NameMap);
};

/***********************************
 * Data context
 ***********************************/

template <typename T>
struct FDataTypeContext<T*> : public FAbstractDataTypeContext
{
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsData() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		if (FAbstractDataTypeContext::IsA(RightContext))
			return true;

		if (RightContext->IsArray() || RightContext->IsMap())
			return false;

		UClass* RClass = Cast<UClass>(RightContext->GetUE4Type());
		//if (RClass != nullptr && RClass->IsChildOf(T::StaticClass()))
		if (RClass != nullptr && RClass->IsChildOf(UPsData::StaticClass()))
			return true;
		return false;
	}

	_DFUNC(UPsDataFunctionLibrary, T*, Data);
};

template <typename T>
struct FDataTypeContext<TArray<T*>> : public FAbstractDataTypeContext
{
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsArray() const override
	{
		return true;
	}

	virtual bool IsData() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		if (FAbstractDataTypeContext::IsA(RightContext))
			return true;

		if (!RightContext->IsArray())
			return false;

		UClass* RClass = Cast<UClass>(RightContext->GetUE4Type());
		//if (RClass != nullptr && RClass->IsChildOf(T::StaticClass()))
		if (RClass != nullptr && RClass->IsChildOf(UPsData::StaticClass()))
			return true;
		return false;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<T*>, DataArray);
};

template <typename T>
struct FDataTypeContext<TMap<FString, T*>> : public FAbstractDataTypeContext
{
	static_assert(FDataReflectionTools::TIsPsData<T>::Value, "Pointer must be only UPsData");

	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsMap() const override
	{
		return true;
	}

	virtual bool IsData() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		if (FAbstractDataTypeContext::IsA(RightContext))
			return true;

		if (!RightContext->IsMap())
			return false;

		UClass* RClass = Cast<UClass>(RightContext->GetUE4Type());
		//if (RClass != nullptr && RClass->IsChildOf(T::StaticClass()))
		if (RClass != nullptr && RClass->IsChildOf(UPsData::StaticClass()))
			return true;
		return false;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA T*>, DataMap);
};

/***********************************
 * TSoftObjectPtr context
 ***********************************/

template <typename T>
struct FDataTypeContext<TSoftObjectPtr<T>> : public FAbstractDataTypeContext
{
	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}

	_DFUNC(UPsDataFunctionLibrary, TSoftObjectPtr<T>, SoftObject);
};

template <typename T>
struct FDataTypeContext<TArray<TSoftObjectPtr<T>>> : public FAbstractDataTypeContext
{
	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsArray() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<TSoftObjectPtr<T>>, SoftObjectArray);
};

template <typename T>
struct FDataTypeContext<TMap<FString, TSoftObjectPtr<T>>> : public FAbstractDataTypeContext
{
	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsMap() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA TSoftObjectPtr<T>>, SoftObjectMap);
};

/***********************************
 * TSoftClassPtr context
 ***********************************/

template <typename T>
struct FDataTypeContext<TSoftClassPtr<T>> : public FAbstractDataTypeContext
{
	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}

	_DFUNC(UPsDataFunctionLibrary, TSoftClassPtr<T>, SoftClass);
};

template <typename T>
struct FDataTypeContext<TArray<TSoftClassPtr<T>>> : public FAbstractDataTypeContext
{
	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsArray() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<TSoftClassPtr<T>>, SoftClassArray);
};

template <typename T>
struct FDataTypeContext<TMap<FString, TSoftClassPtr<T>>> : public FAbstractDataTypeContext
{
	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsMap() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		return true; // TODO: check type
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA TSoftClassPtr<T>>, SoftClassMap);
};

/***********************************
 * FLinearColor context
 ***********************************/

template <>
struct FDataTypeContext<FLinearColor> : public FAbstractDataTypeContext
{
	_DFUNC(UPsDataFunctionLibrary, FLinearColor, LinearColor);
};

template <>
struct FDataTypeContext<TArray<FLinearColor>> : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<FLinearColor>, LinearColorArray);
};

template <>
struct FDataTypeContext<TMap<FString, FLinearColor>> : public FAbstractDataTypeContext
{
	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA FLinearColor>, LinearColorMap);
};

/***********************************
 * Enum context
 ***********************************/

template <typename T>
struct FEnumDataTypeContext : public FAbstractDataTypeContext
{
	static_assert(std::is_enum<T>::value, "Only \"enum class : uint8\" can be described by DESCRIBE_ENUM macros");

	virtual UField* GetUE4Type() const override
	{
		return FindObject<UEnum>(ANY_PACKAGE, *FDataReflectionTools::FType<T>::ContentType());
	}

	virtual TSharedPtr<const FDataFieldFunctions> GetUFunctions() const override
	{
		static TSharedPtr<const FDataFieldFunctions> Functions(
			new FDataFieldFunctions(
				UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(TEXT("GetByteProperty"))),
				UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(TEXT("SetByteProperty")))));
		return Functions;
	}

	virtual FAbstractDataMemory* AllocateMemory() const override
	{
		return new FDataMemory<T>();
	}

	virtual const FString& GetCppType() const override
	{
		return FDataReflectionTools::FType<T>::Type();
	}

	virtual const FString& GetCppContentType() const override
	{
		return FDataReflectionTools::FType<T>::ContentType();
	}

	virtual uint32 GetHash() const override
	{
		static constexpr const int32 Hash = FDataReflectionTools::FType<T>::Hash();
		return Hash;
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
		static constexpr const int32 Hash = FDataReflectionTools::FType<uint8>::Hash();
		return Hash == RightContext->GetHash() || GetHash() == RightContext->GetHash();
	}
};

template <typename T>
struct FEnumDataTypeContext<TArray<T>> : public FAbstractDataTypeContext
{
	static_assert(std::is_enum<T>::value, "Only \"enum class : uint8\" can be described by DESCRIBE_ENUM macros");

	virtual UField* GetUE4Type() const override
	{
		return FindObject<UEnum>(ANY_PACKAGE, *FDataReflectionTools::FType<T>::ContentType());
	}

	virtual TSharedPtr<const FDataFieldFunctions> GetUFunctions() const override
	{
		static TSharedPtr<const FDataFieldFunctions> Functions(
			new FDataFieldFunctions(
				UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(TEXT("GetByteArrayProperty"))),
				UPsDataFunctionLibrary::StaticClass()->FindFunctionByName(FName(TEXT("SetByteArrayProperty")))));
		return Functions;
	}

	virtual FAbstractDataMemory* AllocateMemory() const override
	{
		return new FDataMemory<TArray<T>>();
	}

	virtual const FString& GetCppType() const override
	{
		return FDataReflectionTools::FType<TArray<T>>::Type();
	}

	virtual const FString& GetCppContentType() const override
	{
		return FDataReflectionTools::FType<TArray<T>>::ContentType();
	}

	virtual uint32 GetHash() const override
	{
		static constexpr const int32 Hash = FDataReflectionTools::FType<TArray<T>>::Hash();
		return Hash;
	}

	bool HasExtendedTypeCheck() const override
	{
		return true;
	}

	virtual bool IsArray() const override
	{
		return true;
	}

	virtual bool IsEnum() const override
	{
		return true;
	}

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		static constexpr const int32 Hash = FDataReflectionTools::FType<TArray<uint8>>::Hash();
		return Hash == RightContext->GetHash() || GetHash() == RightContext->GetHash();
	}
};

namespace FDataReflectionTools
{

template <typename T>
FDataTypeContext<T>& GetContext()
{
	static const TSharedPtr<FDataTypeContext<T>> Context(new FDataTypeContext<T>());
	return *Context.Get();
}

/***********************************
 * CHECK TYPE BY CONTEXT
 ***********************************/

template <typename T>
bool CheckType(FAbstractDataTypeContext* LeftContext, FAbstractDataTypeContext* RightContext)
{
	if (LeftContext == RightContext)
	{
		return true;
	}
	return LeftContext->IsA(RightContext);
}

/***********************************
 * GET PROPERTY VALUE BY FIELD
 ***********************************/

template <typename T>
bool GetByField(UPsData* Instance, const TSharedPtr<const FDataField>& Field, T*& OutValue)
{
	if (CheckType<T>(&GetContext<T>(), Field->Context))
	{
		return UnsafeGet(Instance, Field, OutValue);
	}
	else
	{
		check(false && "Can't cast property to T");
		OutValue = nullptr;
		return false;
	}
}

/***********************************
 * GET PROPERTY VALUE BY FIELD (for nested collections)
 ***********************************/

template <typename T>
bool GetByField(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<TArray<T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TArray<TMap<FString, T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, TArray<T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, const TSharedPtr<const FDataField>& Field, TMap<FString, TMap<FString, T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

/***********************************
 * GET PROPERTY VALUE BY HASH
 ***********************************/

template <typename T>
bool GetByHash(UPsData* Instance, int32 Hash, T*& OutValue)
{
	auto& Field = FDataReflection::GetFieldByHash(Instance->GetClass(), Hash);
	if (Field.IsValid())
	{
		return GetByField(Instance, Field, OutValue);
	}

	check(false && "Can't find property by hash");
	return false;
}

/***********************************
 * GET PROPERTY VALUE BY PATH
 ***********************************/

template <typename T>
bool GetByPath(UPsData* Instance, const TArray<FString>& Path, int32 PathOffset, int32 PathLength, T*& OutValue)
{
	const int32 Delta = PathLength - PathOffset;
	OutValue = nullptr;

	check(PathLength <= Path.Num());
	check(Delta > 0);

	auto& Field = FDataReflection::GetFieldByName(Instance->GetClass(), Path[PathOffset]);
	if (Field.IsValid())
	{
		if (Delta == 1)
		{
			return GetByField(Instance, Field, OutValue);
		}
		else if (Delta > 1)
		{
			const bool bArray = Field->Context->IsArray();
			const bool bMap = Field->Context->IsMap();
			const bool bData = Field->Context->IsData();

			if (!bArray && !bMap)
			{
				if (bData)
				{
					UPsData** DataPtr = nullptr;
					if (GetByPath<UPsData*>(Instance, Path, PathOffset, PathOffset + 1, DataPtr))
					{
						UPsData* Data = *DataPtr;
						if (Data)
						{
							return GetByPath<T>(Data, Path, PathOffset + 1, Path.Num(), OutValue);
						}
						else
						{
							check(false && "Can't use nullptr property");
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					check(false && "Can't use property without children");
					return false;
				}
			}
			else if (bArray)
			{
				if (bData)
				{
					TArray<UPsData*>* ArrayPtr = nullptr;
					if (GetByField(Instance, Field, ArrayPtr))
					{
						TArray<UPsData*>& Array = *ArrayPtr;
						const FString& StringArrayIndex = Path[PathOffset + 1];
						if (StringArrayIndex.IsNumeric())
						{
							const int32 ArrayIndex = FCString::Atoi(*StringArrayIndex);
							if (Array.IsValidIndex(ArrayIndex))
							{
								if (Array[ArrayIndex])
								{
									return GetByPath<T>(Array[ArrayIndex], Path, PathOffset + 2, Path.Num(), OutValue);
								}
								else
								{
									check(false && "Can't use nullptr property");
									return false;
								}
							}
							else
							{
								check(false && "Can't find property by index");
								return false;
							}
						}
						else
						{
							check(false && "Can't use property as index");
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					TArray<T>* ArrayPtr = nullptr;
					if (GetByField(Instance, Field, ArrayPtr))
					{
						TArray<T>& Array = *ArrayPtr;
						const FString& StringArrayIndex = Path[PathOffset + 1];
						if (StringArrayIndex.IsNumeric())
						{
							const int32 ArrayIndex = FCString::Atoi(*StringArrayIndex);
							if (Array.IsValidIndex(ArrayIndex))
							{
								OutValue = &Array[ArrayIndex];
								return true;
							}
							else
							{
								check(false && "Can't find property by index");
								return false;
							}
						}
						else
						{
							check(false && "Can't use property as index");
							return false;
						}
					}
					else
					{
						return false;
					}
				}
			}
			else if (bMap)
			{
				if (bData)
				{
					TMap<FString, UPsData*>* MapPtr = nullptr;
					if (GetByField(Instance, Field, MapPtr))
					{
						TMap<FString, UPsData*>& Map = *MapPtr;
						const FString& Key = Path[PathOffset + 1];
						UPsData** DataPtr = Map.Find(Key);
						if (DataPtr)
						{
							return GetByPath<T>(*DataPtr, Path, PathOffset + 2, Path.Num(), OutValue);
						}
						else
						{
							check(false && "Can't find property by name");
							return false;
						}
					}
					else
					{
						return false;
					}
				}
				else
				{
					TMap<FString, T>* MapPtr = nullptr;
					if (GetByField(Instance, Field, MapPtr))
					{
						TMap<FString, T>& Map = *MapPtr;
						const FString& Key = Path[PathOffset + 1];
						T* ValuePtr = Map.Find(Key);
						if (ValuePtr)
						{
							OutValue = ValuePtr;
							return true;
						}
						else
						{
							check(false && "Can't find property by name");
							return false;
						}
					}
					else
					{
						return false;
					}
				}
			}
		}
	}
	else
	{
		check(false && "Can't find property by name");
		return false;
	}

	return false;
}

/***********************************
 * GET PROPERTY VALUE BY PATH
 ***********************************/

template <typename T>
bool GetByPath(UPsData* Instance, const FString& Path, T*& OutValue)
{
	auto& Field = FDataReflection::GetFieldByName(Instance->GetClass(), Path);
	if (Field.IsValid())
	{
		return GetByField(Instance, Field, OutValue);
	}
	else
	{
		const auto& PathArray = FDataReflection::SplitPath(Path);
		if (PathArray.Num() > 1)
		{
			return GetByPath<T>(Instance, PathArray, 0, PathArray.Num(), OutValue);
		}
	}

	check(false && "Can't find property by name");
	OutValue = nullptr;
	return false;
}

/***********************************
 * GET PROPERTY VALUE BY NAME
 ***********************************/

template <typename T>
bool GetByName(UPsData* Instance, const FString& Name, T*& OutValue)
{
	auto& Field = FDataReflection::GetFieldByName(Instance->GetClass(), Name);
	if (Field.IsValid())
	{
		return GetByField(Instance, Field, OutValue);
	}
	//	else
	//	{
	//		TArray<FString> Path;
	//		Name.ParseIntoArray(Path, TEXT("."));
	//		if (Path.Num() > 1)
	//		{
	//			return GetByPath<T>(Instance, Path, 0, Path.Num(), OutValue);
	//		}
	//	}

	check(false && "Can't find property by name");
	OutValue = nullptr;
	return false;
}

/***********************************
 * SET PROPERTY VALUE BY FIELD
 ***********************************/

template <typename T>
void SetByField(UPsData* Instance, const TSharedPtr<const FDataField>& Field, typename FDataReflectionTools::TConstRef<T>::Type NewValue)
{
	if (Field->Meta.bStrict && !Instance->HasAnyFlags(EObjectFlags::RF_NeedInitialization))
	{
		check(false && "Can't set strict property");
		return;
	}

	if (CheckType<T>(Field->Context, &GetContext<T>()))
	{
		UnsafeSet<T>(Instance, Field, NewValue);
	}
	else
	{
		check(false && "Can't cast property to T");
	}
}

/***********************************
 * SET PROPERTY VALUE BY HASH
 ***********************************/

template <typename T>
void SetByHash(UPsData* Instance, int32 Hash, typename FDataReflectionTools::TConstRef<T>::Type NewValue)
{
	auto& Field = FDataReflection::GetFieldByHash(Instance->GetClass(), Hash);
	if (Field.IsValid())
	{
		SetByField<T>(Instance, Field, NewValue);
		return;
	}

	check(false && "Can't find property by hash");
}

/***********************************
 * SET PROPERTY VALUE BY NAME
 ***********************************/

template <typename T>
void SetByName(UPsData* Instance, const FString& Name, typename FDataReflectionTools::TConstRef<T>::Type NewValue)
{
	auto& Field = FDataReflection::GetFieldByName(Instance->GetClass(), Name);
	if (Field.IsValid())
	{
		SetByField<T>(Instance, Field, NewValue);
	}

	check(false && "Can't find property by name");
}
} // namespace FDataReflectionTools

/***********************************
 * Cast helper
 ***********************************/

namespace FDataReflectionTools
{
template <typename T>
struct FPsDataCastHelper
{
	static T* Cast(UPsData* Data)
	{
		if (Data != nullptr)
		{
			const bool bCanCast = Data->GetClass()->IsChildOf(T::StaticClass());
			if (bCanCast)
			{
				return static_cast<T*>(Data);
			}
			else
			{
				check(false && "Can't cast property to T");
			}
		}
		return nullptr;
	}

	static TArray<T*> Cast(TArray<UPsData*> Array)
	{
		TArray<T*> Result;
		for (auto Element : Array)
		{
			Result.Add(Cast(Element));
		}
		return Result;
	}
};
} // namespace FDataReflectionTools

/***********************************
 * FDProp
 ***********************************/

namespace FDataReflectionTools
{
template <typename Type, int32 Hash>
struct FDProp
{
	typedef Type PropType;
	static constexpr int32 PropHash = Hash;

	FDProp(const char* Name)
	{
		if (FDataReflection::InQueue())
		{
			FDataReflection::AddField(Name, Hash, &FDataReflectionTools::GetContext<Type>());
			FDataReflection::ClearMeta();
		}
	}

	FDProp(const FDProp&) = delete;
	FDProp(FDProp&&) = delete;
	FDProp& operator=(const FDProp&) = delete;
	FDProp& operator=(FDProp&&) = delete;
};
} // namespace FDataReflectionTools

/***********************************
 * FDMeta
 ***********************************/

namespace FDataReflectionTools
{
struct FDMeta
{
	FDMeta()
	{
	}

	FDMeta(const char* Meta)
	{
		if (FDataReflection::InQueue())
		{
			FDataReflection::PushMeta(Meta);
		}
	}

	FDMeta(const FDMeta&) = delete;
	FDMeta(FDMeta&&) = delete;
	FDMeta& operator=(const FDMeta&) = delete;
	FDMeta& operator=(FDMeta&&) = delete;
};
} // namespace FDataReflectionTools

/***********************************
 * FDLink
 ***********************************/

namespace FDataReflectionTools
{
template <class ReturnType, int32 Hash>
struct FDLinkHelper
{
	typedef typename TRemovePointer<ReturnType>::Type NonPointerReturnType;
	typedef typename TConstRef<NonPointerReturnType*, true>::Type CompleteReturnType;

	static CompleteReturnType Get(const UPsData* Instance)
	{
		return Cast<NonPointerReturnType>(UPsDataFunctionLibrary::GetDataByLinkHash(Instance, Hash));
	}

	static TArray<CompleteReturnType> GetAsArray(const UPsData* Instance)
	{
		TArray<CompleteReturnType> Result;
		for (UPsData* Data : UPsDataFunctionLibrary::GetDataArrayByLinkHash(Instance, Hash))
		{
			Result.Add(Cast<NonPointerReturnType>(Data));
		}
		return Result;
	}

	static bool IsEmpty(const UPsData* Instance)
	{
		return UPsDataFunctionLibrary::IsLinkEmpty(Instance, Hash);
	}
};

template <typename Type, class ReturnType, int32 Hash>
struct FDLinkBase
{
protected:
	const UPsData* Instance;

public:
	FDLinkBase(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract)
		: Instance(InInstance)
	{
		static_assert(FDataReflectionTools::TIsContainer<ReturnType>::Value, "ReturnType must be non-container type");
		if (FDataReflection::InQueue())
		{
			FDataReflection::AddLink(Name, Path, CharReturnType, Hash, bAbstract, !FDataReflectionTools::TIsContainer<Type>::Value);
			FDataReflection::ClearMeta();
		}
	}

	FDLinkBase(const char* Name, const char* CharReturnType, const UPsData* InInstance)
		: FDLinkBase(Name, "<ABSTRACT>", CharReturnType, InInstance, true)
	{
	}

	FDLinkBase(const FDLinkBase&) = delete;
	FDLinkBase(FDLinkBase&&) = delete;
	FDLinkBase& operator=(const FDLinkBase&) = delete;
	FDLinkBase& operator=(FDLinkBase&&) = delete;
};

template <typename Type, class ReturnType, int32 Hash>
struct FDLink : public FDLinkBase<Type, ReturnType, Hash>
{
public:
	FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false)
		: FDLinkBase<Type, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)
	{
		static_assert(FDataReflectionTools::TAlwaysFalse<Type>::value, "Unsupported link type");
	}

	FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)
		: FDLinkBase<Type, ReturnType, Hash>(Name, CharReturnType, InInstance)
	{
		static_assert(FDataReflectionTools::TAlwaysFalse<Type>::value, "Unsupported link type");
	}

	FDLink(const FDLink&) = delete;
	FDLink(FDLink&&) = delete;
	FDLink& operator=(const FDLink&) = delete;
	FDLink& operator=(FDLink&&) = delete;
};

template <class ReturnType, int32 Hash>
struct FDLink<FString, ReturnType, Hash> : public FDLinkBase<FString, ReturnType, Hash>
{
public:
	FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false)
		: FDLinkBase<FString, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)
	{
	}

	FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)
		: FDLinkBase<FString, ReturnType, Hash>(Name, CharReturnType, InInstance)
	{
	}

	typename FDLinkHelper<ReturnType, Hash>::CompleteReturnType Get() const
	{
		return FDLinkHelper<ReturnType, Hash>::Get(this->Instance);
	}

	bool IsEmpty() const
	{
		return FDLinkHelper<ReturnType, Hash>::IsEmpty(this->Instance);
	}

	FDLink(const FDLink&) = delete;
	FDLink(FDLink&&) = delete;
	FDLink& operator=(const FDLink&) = delete;
	FDLink& operator=(FDLink&&) = delete;
};

template <class ReturnType, int32 Hash>
struct FDLink<TArray<FString>, ReturnType, Hash> : public FDLinkBase<TArray<FString>, ReturnType, Hash>
{
public:
	FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false)
		: FDLinkBase<TArray<FString>, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)
	{
	}

	FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)
		: FDLinkBase<TArray<FString>, ReturnType, Hash>(Name, CharReturnType, InInstance)
	{
	}

	TArray<typename FDLinkHelper<ReturnType, Hash>::CompleteReturnType> Get() const
	{
		return FDLinkHelper<ReturnType, Hash>::GetAsArray(this->Instance);
	}

	bool IsEmpty() const
	{
		return FDLinkHelper<ReturnType, Hash>::IsEmpty(this->Instance);
	}

	FDLink(const FDLink&) = delete;
	FDLink(FDLink&&) = delete;
	FDLink& operator=(const FDLink&) = delete;
	FDLink& operator=(FDLink&&) = delete;
};

template <class ReturnType, int32 Hash>
struct FDLink<FName, ReturnType, Hash> : public FDLinkBase<FName, ReturnType, Hash>
{
public:
	FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false)
		: FDLinkBase<FName, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)
	{
	}

	FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)
		: FDLinkBase<FName, ReturnType, Hash>(Name, CharReturnType, InInstance)
	{
	}

	typename FDLinkHelper<ReturnType, Hash>::CompleteReturnType Get() const
	{
		return FDLinkHelper<ReturnType, Hash>::Get(this->Instance);
	}

	bool IsEmpty() const
	{
		return FDLinkHelper<ReturnType, Hash>::IsEmpty(this->Instance);
	}

	FDLink(const FDLink&) = delete;
	FDLink(FDLink&&) = delete;
	FDLink& operator=(const FDLink&) = delete;
	FDLink& operator=(FDLink&&) = delete;
};

template <class ReturnType, int32 Hash>
struct FDLink<TArray<FName>, ReturnType, Hash> : public FDLinkBase<TArray<FName>, ReturnType, Hash>
{
public:
	FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false)
		: FDLinkBase<TArray<FName>, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)
	{
	}

	FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)
		: FDLinkBase<TArray<FName>, ReturnType, Hash>(Name, CharReturnType, InInstance)
	{
	}

	TArray<typename FDLinkHelper<ReturnType, Hash>::CompleteReturnType> Get() const
	{
		return FDLinkHelper<ReturnType, Hash>::GetAsArray(this->Instance);
	}

	bool IsEmpty() const
	{
		return FDLinkHelper<ReturnType, Hash>::IsEmpty(this->Instance);
	}

	FDLink(const FDLink&) = delete;
	FDLink(FDLink&&) = delete;
	FDLink& operator=(const FDLink&) = delete;
	FDLink& operator=(FDLink&&) = delete;
};
} // namespace FDataReflectionTools

/***********************************
 * Private macros
 ***********************************/

#define _TOKENPASTE(x, y) x##y
#define _TOKENPASTE2(x, y) _TOKENPASTE(x, y)
#define _UNIQ(name) _TOKENPASTE2(__z##name, __LINE__)
#define _REFLECT(__Type__, __Name__)                                                   \
private:                                                                               \
	static constexpr const int32 _UNIQ(hash) = FDataReflectionTools::crc32(#__Name__); \
                                                                                       \
protected:                                                                             \
	const FDataReflectionTools::FDProp<__Type__, _UNIQ(hash)> _zprop##__Name__{#__Name__};
