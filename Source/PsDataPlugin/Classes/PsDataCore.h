// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataFunctionLibrary.h"
#include "PsDataMemory.h"
#include "PsDataTraits.h"
#include "PsDataUtils.h"

#include "CoreMinimal.h"

/***********************************
 * FDataReflection
 ***********************************/

namespace FDataReflectionTools
{
template <class Class, typename Type>
struct FDprop;
struct FMeta;
} // namespace FDataReflectionTools

struct PSDATAPLUGIN_API FDataReflection
{
private:
	static TMap<UClass*, TMap<FString, TSharedPtr<const FDataField>>> FieldsByName;
	static TMap<UClass*, TMap<int32, TSharedPtr<const FDataField>>> FieldsByHash;

	static TArray<UClass*> ClassQueue;
	static TArray<const char*> MetaCollection;

	static bool bCompiled;

protected:
	template <class Class, typename Type>
	friend struct FDataReflectionTools::FDprop;
	friend struct FDataReflectionTools::FMeta;
	friend class UPsData;

	static void AddField(UClass* OwnerClass, const FString& Name, int32 Hash, FAbstractDataTypeContext* Context);
	static void AddToQueue(UPsData* Instance);
	static void RemoveFromQueue(UPsData* Instance);
	static bool InQueue(UClass* StaticClass);
	static bool InQueue();
	static UClass* GetLastClassInQueue();
	static void PushMeta(const char* Meta);
	static void ClearMeta();
	static void Fill(UPsData* Instance);

public:
	static TSharedPtr<const FDataField> GetFieldByName(UClass* OwnerClass, const FString& Name);
	static TSharedPtr<const FDataField> GetFieldByHash(UClass* OwnerClass, int32 Hash);
	static const TMap<FString, TSharedPtr<const FDataField>>& GetFields(const UClass* StaticClass);
	static bool HasClass(const UClass* StaticClass);

	static void Compile();
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
 * Data context
 ***********************************/

template <typename T>
struct FDataTypeContext<T*> : public FAbstractDataTypeContext
{
	static_assert(std::is_base_of<UPsData, T>::value, "Pointer must be only UPsData");

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
	static_assert(std::is_base_of<UPsData, T>::value, "Pointer must be only UPsData");

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
	static_assert(std::is_base_of<UPsData, T>::value, "Pointer must be only UPsData");

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
	static_assert(std::is_base_of<UObject, T>::value, "T must be only UObject");

	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	_DFUNC(UPsDataFunctionLibrary, TSoftObjectPtr<T>, SoftObject);
};

template <typename T>
struct FDataTypeContext<TArray<TSoftObjectPtr<T>>> : public FAbstractDataTypeContext
{
	static_assert(std::is_base_of<UObject, T>::value, "T must be only UObject");

	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsArray() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TArray<TSoftObjectPtr<T>>, SoftObjectArray);
};

template <typename T>
struct FDataTypeContext<TMap<FString, TSoftObjectPtr<T>>> : public FAbstractDataTypeContext
{
	static_assert(std::is_base_of<UObject, T>::value, "T must be only UObject");

	virtual UField* GetUE4Type() const override
	{
		return T::StaticClass();
	}

	virtual bool IsMap() const override
	{
		return true;
	}

	_DFUNC(UPsDataFunctionLibrary, TMap<FString COMMA TSoftObjectPtr<T>>, SoftObjectMap);
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
 * Enum context
 ***********************************/

template <typename T>
struct FEnumDataTypeContext : public FAbstractDataTypeContext
{
	static_assert(std::is_enum<T>::value, "Only \"enum class : uint8\" can be describe by DESCRIBE_ENUM macros");

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

	virtual bool IsA(const FAbstractDataTypeContext* RightContext) const override
	{
		static constexpr const int32 Hash = FDataReflectionTools::FType<T>::Hash();
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
bool GetByField(UPsData* Instance, TSharedPtr<const FDataField> Field, T*& OutValue)
{
	if (CheckType<T>(&GetContext<T>(), Field->Context))
	{
		return UnsafeGet(Instance, Field, OutValue);
	}
	else
	{
		OutValue = nullptr;
		UE_LOG(LogData, Fatal, TEXT("Property %s::%s has type \"%s\" can't cast to \"%s\""), *Instance->GetClass()->GetName(), *Field->Name, *Field->Context->GetCppType(), *GetContext<T>().GetCppType());
		return false;
	}
}

/***********************************
 * GET PROPERTY VALUE BY FIELD (for nested collections)
 ***********************************/

template <typename T>
bool GetByField(UPsData* Instance, TSharedPtr<const FDataField> Field, TArray<TArray<T>>*& OutValue)
{
	OutValue = nullptr;
	UE_LOG(LogData, Fatal, TEXT("Unsupported type"));
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, TSharedPtr<const FDataField> Field, TArray<TMap<FString, T>>*& OutValue)
{
	OutValue = nullptr;
	UE_LOG(LogData, Fatal, TEXT("Unsupported type"));
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, TSharedPtr<const FDataField> Field, TMap<FString, TArray<T>>*& OutValue)
{
	OutValue = nullptr;
	UE_LOG(LogData, Fatal, TEXT("Unsupported type"));
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, TSharedPtr<const FDataField> Field, TMap<FString, TMap<FString, T>>*& OutValue)
{
	OutValue = nullptr;
	UE_LOG(LogData, Fatal, TEXT("Unsupported type"));
	return false;
}

/***********************************
 * GET PROPERTY VALUE BY HASH
 ***********************************/

template <typename T>
bool GetByHash(UPsData* Instance, int32 Hash, T*& OutValue)
{
	auto Field = FDataReflection::GetFieldByHash(Instance->GetClass(), Hash);
	if (Field.IsValid())
	{
		return GetByField(Instance, Field, OutValue);
	}

	UE_LOG(LogData, Error, TEXT("Can't find property in %s by hash: 0x%08x"), *Instance->GetClass()->GetName(), Hash);
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

	auto Find = FDataReflection::GetFields(Instance->GetClass()).Find(Path[PathOffset]);
	if (Find)
	{
		auto Field = *Find;
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
							UE_LOG(LogData, Error, TEXT("Property %s::%s is null"), *Instance->GetClass()->GetName(), *Field->Name);
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
					UE_LOG(LogData, Error, TEXT("Property %s::%s doesn't contain children"), *Instance->GetClass()->GetName(), *Field->Name);
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
									UE_LOG(LogData, Error, TEXT("Property %s::%s[%d] is null"), *Instance->GetClass()->GetName(), *Field->Name, ArrayIndex);
									return false;
								}
							}
							else
							{
								UE_LOG(LogData, Error, TEXT("Property %s::%s[%d] is not found"), *Instance->GetClass()->GetName(), *Field->Name, ArrayIndex);
								return false;
							}
						}
						else
						{
							UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] index is not valid"), *Instance->GetClass()->GetName(), *Field->Name, *StringArrayIndex);
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
								UE_LOG(LogData, Error, TEXT("Property %s::%s[%d] is not found"), *Instance->GetClass()->GetName(), *Field->Name, ArrayIndex);
								return false;
							}
						}
						else
						{
							UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] index is not valid"), *Instance->GetClass()->GetName(), *Field->Name, *StringArrayIndex);
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
							UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] is not found"), *Instance->GetClass()->GetName(), *Field->Name, *Key);
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
							UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] is not found"), *Instance->GetClass()->GetName(), *Field->Name, *Key);
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
		UE_LOG(LogData, Error, TEXT("Property %s::%s is not found"), *Instance->GetClass()->GetName(), *Path[PathOffset]);
		return false;
	}

	return false;
}

/***********************************
 * GET PROPERTY VALUE BY NAME
 ***********************************/

template <typename T>
bool GetByName(UPsData* Instance, const FString& Name, T*& OutValue)
{
	auto Find = FDataReflection::GetFields(Instance->GetClass()).Find(Name);
	if (Find)
	{
		return GetByField(Instance, *Find, OutValue);
	}
	else
	{
		TArray<FString> Path;
		Name.ParseIntoArray(Path, TEXT("."));
		if (Path.Num() > 1)
		{
			return GetByPath<T>(Instance, Path, 0, Path.Num(), OutValue);
		}
	}

	OutValue = nullptr;

	UE_LOG(LogData, Error, TEXT("Property %s::%s is not found"), *Instance->GetClass()->GetName(), *Name);
	return false;
}

/***********************************
 * SET PROPERTY VALUE BY FIELD
 ***********************************/

template <typename T>
void SetByField(UPsData* Instance, TSharedPtr<const FDataField> Field, typename FDataReflectionTools::TConstRef<T>::Type NewValue)
{
	if (Field->Meta.bStrict && !Instance->HasAnyFlags(EObjectFlags::RF_NeedInitialization))
	{
		UE_LOG(LogData, Error, TEXT("Can't set strict %s::%s property"), *Instance->GetClass()->GetName(), *Field->Name);
		return;
	}

	if (CheckType<T>(Field->Context, &GetContext<T>()))
	{
		UnsafeSet<T>(Instance, Field, NewValue);
	}
	else
	{
		UE_LOG(LogData, Fatal, TEXT("Property %s::%s has type (%s) can't cast from (%s)"), *Instance->GetClass()->GetName(), *Field->Name, *Field->Context->GetCppType(), *GetContext<T>().GetCppType());
	}
}

/***********************************
 * SET PROPERTY VALUE BY HASH
 ***********************************/

template <typename T>
void SetByHash(UPsData* Instance, int32 Hash, typename FDataReflectionTools::TConstRef<T>::Type NewValue)
{
	auto Field = FDataReflection::GetFieldByHash(Instance->GetClass(), Hash);
	if (Field.IsValid())
	{
		SetByField<T>(Instance, Field, NewValue);
		return;
	}

	UE_LOG(LogData, Error, TEXT("Can't find property in %s by hash: 0x%08x"), *Instance->GetClass()->GetName(), Hash);
}

/***********************************
 * SET PROPERTY VALUE BY NAME
 ***********************************/

template <typename T>
void SetByName(UPsData* Instance, const FString& Name, typename FDataReflectionTools::TConstRef<T>::Type NewValue)
{
	auto Find = FDataReflection::GetFields(Instance->GetClass()).Find(Name);
	if (Find)
	{
		SetByField<T>(Instance, *Find, NewValue);
	}
	else
	{
		UE_LOG(LogData, Error, TEXT("Property %s::%s is not found"), *Instance->GetClass()->GetName(), *Name);
	}
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
		if (Data != nullptr && Data->GetClass()->IsChildOf(T::StaticClass()))
		{
			return static_cast<T*>(Data);
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
template <class Class, typename Type>
struct FDprop
{
	FDprop(const char* Name, const int32 Hash)
	{
		if (FDataReflection::InQueue(Class::StaticClass()))
		{
			FDataReflection::AddField(Class::StaticClass(), FString(Name), Hash, &FDataReflectionTools::GetContext<Type>());
			FDataReflection::ClearMeta();
		}
	}
};
} // namespace FDataReflectionTools

/***********************************
 * FDMeta
 ***********************************/

namespace FDataReflectionTools
{
struct FMeta
{
	FMeta(const char* Meta)
	{
		if (FDataReflection::InQueue())
		{
			FDataReflection::PushMeta(Meta);
		}
	}
};
} // namespace FDataReflectionTools

/***********************************
 * Private macros
 ***********************************/

#define _TOKENPASTE(x, y) x##y
#define _TOKENPASTE2(x, y) _TOKENPASTE(x, y)
#define _UNIC(name) _TOKENPASTE2(__z##name, __LINE__)
#define _REFLECT(__Class__, __Type__, __Name__)                                        \
private:                                                                               \
	static constexpr const int32 _UNIC(hash) = FDataReflectionTools::crc32(#__Name__); \
	FDataReflectionTools::FDprop<__Class__, __Type__> _UNIC(prop) = FDataReflectionTools::FDprop<__Class__, __Type__>(#__Name__, _UNIC(hash));
