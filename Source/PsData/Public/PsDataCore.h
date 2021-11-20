// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataField.h"
#include "PsDataFunctionLibrary.h"
#include "PsDataProperty.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"
#include "UObject/Package.h"

namespace PsDataTools
{

struct PSDATA_API FClassFields
{
	FClassFields();
	~FClassFields();

	void AddField(FDataField* Field);
	void AddLink(FDataLink* Link);
	void Sort();

	const TArray<FDataField*>& GetFieldsList();
	const TArray<const FDataField*>& GetFieldsList() const;

	FDataField* GetMutableField(const FDataField* Field);

	const TArray<FDataLink*>& GetLinksList();
	const TArray<const FDataLink*>& GetLinksList() const;

	const FDataField* GetFieldByHash(int32 Hash) const;
	const FDataField* GetFieldByName(const FString& Name) const;
	const FDataField* GetFieldByAlias(const FString& Alias) const;
	const FDataField* GetFieldByIndex(int32 Index) const;

	const FDataField* GetFieldByHashChecked(int32 Hash) const;
	const FDataField* GetFieldByNameChecked(const FString& Name) const;
	const FDataField* GetFieldByAliasChecked(const FString& Alias) const;
	const FDataField* GetFieldByIndexChecked(int32 Index) const;

	bool HasFieldWithHash(int32 Hash) const;
	bool HasFieldWithName(const FString& Name) const;
	bool HasFieldWithAlias(const FString& Alias) const;
	bool HasFieldWithIndex(int32 Index) const;

	int32 GetNumFields() const;

	const FDataLink* GetLinkByHash(int32 Hash) const;
	const FDataLink* GetLinkByName(const FString& Name) const;

	const FDataLink* GetLinkByHashChecked(int32 Hash) const;
	const FDataLink* GetLinkByNameChecked(const FString& Name) const;

	int32 GetNumLinks() const;

private:
	TArray<FDataField*> FieldsList;
	TArray<const FDataField*> ConstFieldsList;
	TMap<FString, const FDataField*> FieldsByName;
	TMap<FString, const FDataField*> FieldsByAlias;
	TMap<int32, const FDataField*> FieldsByHash;

	TArray<FDataLink*> LinkList;
	TArray<const FDataLink*> ConstLinkList;
	TMap<FString, const FDataLink*> LinksByName;
	TMap<int32, const FDataLink*> LinksByHash;
};

struct PSDATA_API FDataReflection
{
private:
	static TMap<UClass*, FClassFields> FieldsByClass;
	static TMap<FString, const TArray<FString>> SplittedPath;
	static TArray<const char*> MetaCollection;

	static bool bCompiled;

public:
	static void InitField(const char* CharName, int32 Hash, FAbstractDataTypeContext* Context, FDataField*& Field, UPsData* Instance, FAbstractDataProperty* Property);
	static void InitLink(const char* CharName, const char* CharPath, const char* CharReturnType, int32 Hash, bool bAbstract, bool bCollection, UPsData* Instance);
	static void InitMeta(const char* Meta);

	static void PreConstruct(UPsData* Instance);
	static void PostConstruct(UPsData* Instance);

	static const FClassFields* GetFieldsByClass(const UClass* Class);
	static bool HasClass(const UClass* OwnerClass);

	static void Compile();

	static const TArray<FString>& SplitPath(const FString& Path);
};

/***********************************
 * Base context
 ***********************************/

template <typename T>
struct FDataTypeContext : public FAbstractDataTypeContext
{
	static_assert(TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, class L>
struct FDataTypeContextExtended : public FAbstractDataTypeContext
{
	virtual bool IsArray() const override
	{
		return TIsContainer<T>::Array;
	}

	virtual bool IsMap() const override
	{
		return TIsContainer<T>::Map;
	}

	virtual FDataFieldFunctions GetUFunctions() const override
	{
		constexpr auto Type = TIsContainer<T>::Value ? EDataFieldType::VALUE : (TIsContainer<T>::Array ? EDataFieldType::ARRAY : EDataFieldType::MAP);
		return {L::StaticClass(), Type};
	}

	virtual FString GetCppType() const override
	{
		return FType<T>::Type();
	}

	virtual FString GetCppContentType() const override
	{
		return FType<T>::ContentType();
	}

	virtual uint32 GetHash() const override
	{
		constexpr uint32 Hash = FType<T>::Hash();
		return Hash;
	}

	virtual ~FDataTypeContextExtended()
	{
	}
};

template <typename T>
UClass* GetClass()
{
#if UE_BUILD_SHIPPING
	static const auto Class = FindObjectChecked<UClass>(ANY_PACKAGE, &((*FType<T>::ContentType())[1]));
	return Class;
#else
	return FindObjectChecked<UClass>(ANY_PACKAGE, &((*FType<T>::ContentType())[1]));
#endif
}

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
#if !UE_BUILD_SHIPPING
	if (LeftContext == RightContext)
	{
		return true;
	}

	return LeftContext->IsA(RightContext);
#else
	return true;
#endif // UE_BUILD_SHIPPING
}

/***********************************
 * GET PROPERTY VALUE BY FIELD
 ***********************************/

template <typename T>
bool GetByField(UPsData* Instance, const FDataField* Field, T*& OutValue)
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
bool GetByField(UPsData* Instance, const FDataField* Field, TArray<TArray<T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, const FDataField* Field, TArray<TMap<FString, T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, const FDataField* Field, TMap<FString, TArray<T>>*& OutValue)
{
	checkNoEntry();
	OutValue = nullptr;
	return false;
}

template <typename T>
bool GetByField(UPsData* Instance, const FDataField* Field, TMap<FString, TMap<FString, T>>*& OutValue)
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
	auto Field = FDataReflection::GetFieldsByClass(Instance->GetClass())->GetFieldByHash(Hash);
	if (Field)
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

	auto Field = FDataReflection::GetFieldsByClass(Instance->GetClass())->GetFieldByName(Path[PathOffset]);
	if (Field)
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
	auto Field = FDataReflection::GetFieldsByClass(Instance->GetClass())->GetFieldByName(Path);
	if (Field)
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
	auto Field = FDataReflection::GetFieldsByClass(Instance->GetClass())->GetFieldByName(Name);
	if (Field)
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
void SetByField(UPsData* Instance, const FDataField* Field, typename TConstRef<T>::Type NewValue)
{
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
void SetByHash(UPsData* Instance, int32 Hash, typename TConstRef<T>::Type NewValue)
{
	auto Field = PsDataTools::FDataReflection::GetFieldsByClass(Instance->GetClass())->GetFieldByHash(Hash);
	if (Field)
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
void SetByName(UPsData* Instance, const FString& Name, typename TConstRef<T>::Type NewValue)
{
	auto Field = FDataReflection::GetFieldsByClass(Instance->GetClass())->GetFieldByName(Name);
	if (Field)
	{
		SetByField<T>(Instance, Field, NewValue);
	}

	check(false && "Can't find property by name");
}

/***********************************
 * Cast helper
 ***********************************/

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

/***********************************
 * FDLink
 ***********************************/

template <typename Type, typename ReturnType, int32 Hash>
struct FDLinkHelper
{
	using NonPointerReturnType = typename TRemovePointer<ReturnType>::Type;
	using CompleteReturnType = typename TConstRef<NonPointerReturnType*, true>::Type;
	using ResultType = CompleteReturnType;

	static ResultType Get(const UPsData* Instance)
	{
		return Cast<NonPointerReturnType>(UPsDataFunctionLibrary::GetDataByLinkHash(Instance, Hash));
	}
};

template <typename Type, typename ReturnType, int32 Hash>
struct FDLinkHelper<TArray<Type>, ReturnType, Hash>
{
	using NonPointerReturnType = typename TRemovePointer<ReturnType>::Type;
	using CompleteReturnType = typename TConstRef<NonPointerReturnType*, true>::Type;
	using ResultType = TArray<CompleteReturnType>;

	static ResultType Get(const UPsData* Instance)
	{
		ResultType Result;
		for (UPsData* Data : UPsDataFunctionLibrary::GetDataArrayByLinkHash(Instance, Hash))
		{
			Result.Add(Cast<NonPointerReturnType>(Data));
		}
		return Result;
	}
};

template <typename Type, typename ReturnType, int32 Hash>
struct FDLink : public FNoncopyable
{
private:
	UPsData* Instance;

public:
	FDLink(const char* Name, const char* Path, const char* CharReturnType, UPsData* InInstance)
		: Instance(InInstance)
	{
		static_assert(TIsContainer<ReturnType>::Value, "ReturnType must be non-container type");

		FDataReflection::InitLink(Name, Path, CharReturnType, Hash, false, !TIsContainer<Type>::Value, InInstance);
	}

	FDLink(const char* Name, const char* CharReturnType, UPsData* InInstance)
		: Instance(InInstance)
	{
		static_assert(TIsContainer<ReturnType>::Value, "ReturnType must be non-container type");

		FDataReflection::InitLink(Name, "<ABSTRACT>", CharReturnType, Hash, true, !TIsContainer<Type>::Value, InInstance);
	}

	typename FDLinkHelper<Type, ReturnType, Hash>::ResultType Get() const
	{
		return FDLinkHelper<Type, ReturnType, Hash>::Get(Instance);
	}

	bool IsEmpty() const
	{
		return UPsDataFunctionLibrary::IsLinkEmpty(Instance, Hash);
	}
};
} // namespace PsDataTools
