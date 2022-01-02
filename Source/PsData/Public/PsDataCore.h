// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsData.h"
#include "PsDataField.h"
#include "PsDataProperty.h"
#include "PsDataStringView.h"
#include "PsDataTraits.h"

#include "CoreMinimal.h"
#include "UObject/Package.h"

#define PSDATA_DEFAULT_THROW_RULE (!UE_BUILD_SHIPPING)
#define PSDATA_ABSTRACT_LINK_SALT 0xFF

namespace PsDataTools
{

struct PSDATA_API FClassFields
{
	FClassFields();
	~FClassFields();

	void AddField(FDataField* Field);
	void AddLink(FDataLink* Link);
	void AddSuper(const FClassFields& SuperFields);
	void Sort();

	FDataField* GetMutableField(const FDataField* Field);

	const TArray<FDataField*>& GetFieldsList();
	const TArray<const FDataField*>& GetFieldsList() const;

	FDataField* GetFieldByHash(int32 Hash);
	FDataField* GetFieldByName(const FString& Name);
	FDataField* GetFieldByAlias(const FString& Alias);
	FDataField* GetFieldByIndex(int32 Index);
	FDataField* GetFieldByHashChecked(int32 Hash);
	FDataField* GetFieldByNameChecked(const FString& Name);
	FDataField* GetFieldByAliasChecked(const FString& Alias);
	FDataField* GetFieldByIndexChecked(int32 Index);
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

	const TArray<FDataLink*>& GetLinksList();
	const TArray<const FDataLink*>& GetLinksList() const;

	FDataLink* GetLinkByHash(int32 Hash);
	FDataLink* GetLinkByHashChecked(int32 Hash);

	const FDataLink* GetLinkByHash(int32 Hash) const;
	const FDataLink* GetLinkByHashChecked(int32 Hash) const;

	bool HasLinkWithHash(int32 Hash) const;

	int32 GetNumLinks() const;

	void CalculateDependencies(UClass* HeadClass, UClass* MainClass, TSet<UClass*>& OutList) const;

private:
	TArray<FDataField*> FieldsList;
	TArray<const FDataField*> ConstFieldsList;
	TMap<FString, FDataField*> FieldsByName;
	TMap<FString, FDataField*> FieldsByAlias;
	TMap<int32, FDataField*> FieldsByHash;

	TArray<FDataLink*> LinkList;
	TArray<const FDataLink*> ConstLinkList;
	TMap<int32, FDataLink*> LinksByHash;

	mutable int32 Recursion;
};

struct PSDATA_API FDataReflection
{
private:
	static TMap<UClass*, FClassFields> FieldsByClass;
	static TMap<const FDataField*, FLinkPathFunction> LinkPathFunctionByField;
	static FDataRawMeta RawMeta;
	static UClass* DescribedClass;
	static bool bCompiled;

public:
	static bool InitMeta(const char* MetaString);
	static bool InitProperty(UClass* Class, const char* Name, FAbstractDataTypeContext* Context, FDataField*& OutField);
	static bool InitLinkProperty(UClass* Class, const char* Name, bool bAbstract, FAbstractDataTypeContext* ReturnContext, FLinkPathFunction PathFunction, FDataLink*& OutLink);

	static void PreConstruct(UClass* Class);
	static void PostConstruct(UClass* Class);

	static const FClassFields* GetFieldsByClass(const UClass* Class);
	static bool HasClass(const UClass* OwnerClass);

	static bool IsBaseClass(const UClass* Class);

	static void Compile();
	static void CompileClass(UClass* Class);
};

/***********************************
 * Context
 ***********************************/

template <typename T>
struct TDataTypeContext : public FAbstractDataTypeContext
{
	static_assert(TAlwaysFalse<T>::value, "Unsupported type");
};

template <typename T, class L>
struct TDataTypeContextExtended : public FAbstractDataTypeContext
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

	virtual FDataLinkFunctions GetLinkUFunctions() const override
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

	virtual ~TDataTypeContextExtended() override
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
TDataTypeContext<T>& GetContext()
{
	static const TSharedPtr<TDataTypeContext<T>> Context(new TDataTypeContext<T>());
	return *Context.Get();
}

/***********************************
 * Check type by context
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
 * Get value by field
 ***********************************/

template <bool bThrowError, typename T>
bool GetByField(UPsData* Instance, const FDataField* Field, T*& OutValue)
{
	if (Instance && Field)
	{
		auto OutputContext = &GetContext<T>();
		if (CheckType<T>(OutputContext, Field->Context))
		{
			UnsafeGet(Instance, Field, OutValue);
			return true;
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't cast property %s::%s to %s"), *Instance->GetClass()->GetName(), *Field->Name, *OutputContext->GetCppType());
		}
	}
	else
	{
		if (bThrowError && !Instance)
		{
			UE_LOG(LogData, Fatal, TEXT("Instance is null"));
		}

		if (bThrowError && !Field)
		{
			UE_LOG(LogData, Fatal, TEXT("Field is null"));
		}
	}

	OutValue = nullptr;
	return false;
}

template <bool bThrowError, bool bThrowContainerError, typename T>
bool GetByFieldAndKey(UPsData* Instance, const FDataField* Field, const FString& Key, T*& OutValue)
{
	if (Instance && Field)
	{
		if (Field->Context->IsMap())
		{
			TMap<FString, T>* MapPtr = nullptr;
			if (GetByField<bThrowError>(Instance, Field, MapPtr))
			{
				if (auto ValuePtr = MapPtr->Find(Key))
				{
					OutValue = ValuePtr;
					return true;
				}
				else if (bThrowContainerError)
				{
					UE_LOG(LogData, Fatal, TEXT("Can't find \"%s\" in %s::%s"), *Key, *Instance->GetClass()->GetName(), *Field->Name);
				}
			}
		}
		else if (Field->Context->IsArray())
		{
			const auto KeyView = ToStringView(Key);
			if (IsUnsignedInteger(KeyView))
			{
				TArray<T>* ArrayPtr = nullptr;
				if (GetByField<bThrowError>(Instance, Field, ArrayPtr))
				{
					const auto Index = ToUnsignedInteger(KeyView);
					if (ArrayPtr->IsValidIndex(Index))
					{
						OutValue = &(*ArrayPtr)[Index];
						return true;
					}
					else if (bThrowContainerError)
					{
						UE_LOG(LogData, Fatal, TEXT("Can't find \"%d\" in %s::%s out of bounds (array size: %d)"), Index, *Instance->GetClass()->GetName(), *Field->Name, ArrayPtr->Num());
					}
				}
			}
			else if (bThrowError)
			{
				UE_LOG(LogData, Fatal, TEXT("Can't find \"%s\" in %s::%s invalid index"), *Key, *Instance->GetClass()->GetName(), *Field->Name);
			}
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Field is not container"));
		}
	}
	else
	{
		if (bThrowError && !Instance)
		{
			UE_LOG(LogData, Fatal, TEXT("Instance is null"));
		}

		if (bThrowError && !Field)
		{
			UE_LOG(LogData, Fatal, TEXT("Field is null"));
		}
	}

	OutValue = nullptr;
	return false;
}

/***********************************
 * Get value by hash
 ***********************************/

template <bool bThrowError, typename T>
bool GetByHash(UPsData* Instance, int32 Hash, T*& OutValue)
{
	if (Instance)
	{
		const auto InstanceClass = Instance->GetClass();
		const auto Field = FDataReflection::GetFieldsByClass(InstanceClass)->GetFieldByHash(Hash);
		if (Field)
		{
			return GetByField<bThrowError>(Instance, Field, OutValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property with hash \"%d\" for %s"), Hash, *InstanceClass->GetName());
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	OutValue = nullptr;
	return false;
}

template <bool bThrowError, typename T>
bool GetByHashAndKey(UPsData* Instance, int32 Hash, const FString& Key, T*& OutValue)
{
	if (Instance)
	{
		const auto InstanceClass = Instance->GetClass();
		const auto Field = FDataReflection::GetFieldsByClass(InstanceClass)->GetFieldByHash(Hash);
		if (Field)
		{
			return GetByFieldAndKey<bThrowError>(Instance, Field, Key, OutValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property with hash \"%d\" for %s"), Hash, *InstanceClass->GetName());
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	OutValue = nullptr;
	return false;
}

/***********************************
 * Get value by name
 ***********************************/

template <bool bThrowError, typename T>
bool GetByName(UPsData* Instance, const FString& Name, T*& OutValue)
{
	if (Instance)
	{
		const auto InstanceClass = Instance->GetClass();
		const auto Field = FDataReflection::GetFieldsByClass(InstanceClass)->GetFieldByName(Name);
		if (Field)
		{
			return GetByField<bThrowError>(Instance, Field, OutValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property %s::%s"), *InstanceClass->GetName(), *Name);
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	OutValue = nullptr;
	return false;
}

template <bool bThrowError, typename T>
bool GetByName(UPsData* Instance, const FString& Name, const FString& Key, T*& OutValue)
{
	if (Instance)
	{
		const auto InstanceClass = Instance->GetClass();
		const auto Field = FDataReflection::GetFieldsByClass(InstanceClass)->GetFieldByName(Name);
		if (Field)
		{
			return GetByField<bThrowError>(Instance, Field, Key, OutValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property %s::%s"), *InstanceClass->GetName(), *Name);
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	OutValue = nullptr;
	return false;
}

/***********************************
 * Data path executor
 ***********************************/

template <bool bThrowError, bool bThrowContainerError>
struct TDataPathExecutor
{
	TDataPathExecutor(UPsData* InData, const FString& InPath)
		: Data(InData)
		, Field(nullptr)
		, bError(false)
	{
		check(Data);

		auto PathView = ToStringView(InPath);
		while (PathView.Len() > 0)
		{
			auto KeyView = PathView.LeftByChar('.');
			Keys.Add(ToFString(KeyView));

			PathView.RightChopInline(KeyView.Len() + 1);
		}
	}

	explicit TDataPathExecutor(UPsData* InData)
		: Data(InData)
		, Field(nullptr)
		, bError(false)
	{
		check(Data);
	}

	template <bool bThrowErrorOther, bool bThrowContainerErrorOther>
	TDataPathExecutor(const TDataPathExecutor<bThrowErrorOther, bThrowContainerErrorOther>& Other)
	{
		if ((bThrowError || bThrowContainerErrorOther) && Other.HasError())
		{
			UE_LOG(LogData, Fatal, TEXT("Can't copy broken executor: %s::%s with path: \"%s\""), *Other.Data->GetClass()->GetName(), *Other.Field->Name, *GetPath());
		}

		Data = Other.Data;
		Field = Other.Field;
		Keys = Other.Keys;
		bError = Other.bError;
	}

	template <bool bThrowErrorOther, bool bThrowContainerErrorOther>
	TDataPathExecutor(TDataPathExecutor<bThrowErrorOther, bThrowContainerErrorOther>&& Other)
	{
		if ((bThrowError || bThrowContainerErrorOther) && Other.HasError())
		{
			UE_LOG(LogData, Fatal, TEXT("Can't move broken executor: %s::%s with path: \"%s\""), *Other.Data->GetClass()->GetName(), *Other.Field->Name, *GetPath());
		}

		Data = std::move(Other.Data);
		Field = std::move(Other.Field);
		Keys = std::move(Other.Keys);
		bError = std::move(Other.bError);
	}

	UPsData* GetData() const
	{
		return Data;
	}

	const FDataField* GetField() const
	{
		return Field;
	}

	bool HasKeys() const
	{
		return Keys.Num() != 0;
	}

	const TArray<FString>& GetKeys() const
	{
		return Keys;
	}

	FString GetPath() const
	{
		FString Result;
		for (const auto& Key : Keys)
		{
			if (!Result.IsEmpty())
			{
				Result.AppendChar('.');
			}
			Result.Append(Key);
		}

		return Result;
	}

	bool HasError() const
	{
		return bError;
	}

	bool Previous()
	{
		const auto Parent = Data->GetParent();
		if (Parent)
		{
			PrependKey(Data->GetFullDataKey());
			Field = FDataReflection::GetFieldsByClass(Parent->GetClass())->GetFieldByNameChecked(Data->GetDataKey());
			Data = Parent;
			bError = false;

			return true;
		}
		else if (Field)
		{
			PrependKey(Field->Name);
			Field = nullptr;
			bError = false;

			return true;
		}

		return false;
	}

	bool Next()
	{
		if (bError)
		{
			return false;
		}

		if (!Field && Normalize())
		{
			return true;
		}

		if (Field->Context->IsData())
		{
			if (Field->Context->IsContainer())
			{
				if (Keys.Num() > 1)
				{
					UPsData** DataPtr = nullptr;
					if (GetByFieldAndKey<bThrowError, bThrowError>(Data, Field, Keys[0], DataPtr) && *DataPtr)
					{
						const auto NewData = *DataPtr;
						const auto NewField = FDataReflection::GetFieldsByClass(NewData->GetClass())->GetFieldByName(Keys[1]);
						if (NewField)
						{
							Keys.RemoveAt(0, 2);
							Data = NewData;
							Field = NewField;
							return true;
						}
						else if (bThrowError)
						{
							UE_LOG(LogData, Fatal, TEXT("Can't find property %s::%s"), *NewData->GetClass()->GetName(), *Keys[1]);
						}
						else
						{
							bError = true;
						}
					}
					else if (bThrowError)
					{
						UE_LOG(LogData, Fatal, TEXT("Can't find \"%s\" in %s::%s or it is null"), *Keys[0], *Data->GetClass()->GetName(), *Field->Name);
					}
					else
					{
						bError = true;
					}
				}
			}
			else
			{
				if (Keys.Num() > 0)
				{
					UPsData** DataPtr = nullptr;
					if (GetByField<bThrowError>(Data, Field, DataPtr) && *DataPtr)
					{
						const auto NewData = *DataPtr;
						const auto NewField = FDataReflection::GetFieldsByClass(NewData->GetClass())->GetFieldByName(Keys[0]);
						if (NewField)
						{
							Keys.RemoveAt(0, 1);
							Data = NewData;
							Field = NewField;
							return true;
						}
						else if (bThrowError)
						{
							UE_LOG(LogData, Fatal, TEXT("Can't find property %s::%s"), *NewData->GetClass()->GetName(), *Keys[0]);
						}
						else
						{
							bError = true;
						}
					}
					else if (bThrowError)
					{
						UE_LOG(LogData, Fatal, TEXT("Can't find %s::%s or it is null"), *Data->GetClass()->GetName(), *Field->Name);
					}
					else
					{
						bError = true;
					}
				}
			}
		}

		return false;
	}

	bool Execute()
	{
		while (Next())
		{
			// do nothing
		}

		return !bError && Keys.Num() <= 1;
	}

	template <typename T>
	bool Execute(T*& OutValue)
	{
		if (Execute())
		{
			const auto NumKeys = Keys.Num();
			if (NumKeys == 0)
			{
				return GetByField<bThrowError>(Data, Field, OutValue);
			}
			else if (NumKeys == 1)
			{
				return GetByFieldAndKey<bThrowError, bThrowContainerError>(Data, Field, Keys[0], OutValue);
			}
		}

		if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't execute %s::%s with path: \"%s\""), *Data->GetClass()->GetName(), *Field->Name, *GetPath());
		}

		OutValue = nullptr;
		return false;
	}

	void AppendKey(const FString& Key)
	{
		Keys.Add(Key);
	}

private:
	void PrependKey(const FString& Key)
	{
		Keys.Insert(Key, 0);
	}

	bool Normalize()
	{
		if (!Field)
		{
			const auto Parent = Data->GetParent();
			if (Parent)
			{
				if (Data->InCollection())
				{
					PrependKey(Data->GetCollectionKey());
				}

				Field = FDataReflection::GetFieldsByClass(Parent->GetClass())->GetFieldByNameChecked(Data->GetDataKey());
				Data = Parent;
			}
			else if (Keys.Num() > 0)
			{
				if (auto NewField = FDataReflection::GetFieldsByClass(Data->GetClass())->GetFieldByName(Keys[0]))
				{
					Field = NewField;
					Keys.RemoveAt(0, 1);
				}
			}
		}

		return Field != nullptr;
	}

	UPsData* Data;
	const FDataField* Field;
	TArray<FString> Keys;
	bool bError;

	template <bool bThrowErrorOther, bool bThrowContainerErrorOther>
	friend struct TDataPathExecutor;
};

using FDataPathExecutor = TDataPathExecutor<PSDATA_DEFAULT_THROW_RULE, PSDATA_DEFAULT_THROW_RULE>;

/***********************************
 * Get value by path
 ***********************************/

template <bool bThrowError, typename T>
bool GetByPath(UPsData* Instance, const FString& Path, T*& OutValue)
{
	if (Instance)
	{
		if (Path.Len() > 0)
		{
			TDataPathExecutor<bThrowError, bThrowError> PathExecutor(Instance, Path);
			return PathExecutor.Execute(OutValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Path is empty"));
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	OutValue = nullptr;
	return false;
}

/***********************************
 * Set value by field
 ***********************************/

template <bool bThrowError, typename T>
bool SetByField(UPsData* Instance, const FDataField* Field, TConstRefType<T, false> NewValue)
{
	if (Instance && Field)
	{
		auto OutputContext = &GetContext<T>();
		if (CheckType<T>(OutputContext, Field->Context))
		{
			UnsafeSet<T>(Instance, Field, NewValue);
			return true;
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't cast property %s::%s to %s"), *Instance->GetClass()->GetName(), *Field->Name, *OutputContext->GetCppType());
		}
	}
	else
	{
		if (bThrowError && !Instance)
		{
			UE_LOG(LogData, Fatal, TEXT("Instance is null"));
		}

		if (bThrowError && !Field)
		{
			UE_LOG(LogData, Fatal, TEXT("Field is null"));
		}
	}

	return false;
}

/***********************************
 * Set value by hash
 ***********************************/

template <bool bThrowError, typename T>
bool SetByHash(UPsData* Instance, int32 Hash, TConstRefType<T, false> NewValue)
{
	if (Instance)
	{
		const auto InstanceClass = Instance->GetClass();
		const auto Field = FDataReflection::GetFieldsByClass(InstanceClass)->GetFieldByHash(Hash);
		if (Field)
		{
			return SetByField<bThrowError, T>(Instance, Field, NewValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property with hash \"%d\" for %s"), Hash, *InstanceClass->GetName());
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	return false;
}

/***********************************
 * Set value by name
 ***********************************/

template <bool bThrowError, typename T>
bool SetByName(UPsData* Instance, const FString& Name, TConstRefType<T, false> NewValue)
{
	if (Instance)
	{
		const auto InstanceClass = Instance->GetClass();
		const auto Field = FDataReflection::GetFieldsByClass(InstanceClass)->GetFieldByName(Name);
		if (Field)
		{
			return SetByField<bThrowError, T>(Instance, Field, NewValue);
		}
		else if (bThrowError)
		{
			UE_LOG(LogData, Fatal, TEXT("Can't find property %s::%s"), *InstanceClass->GetName(), *Name);
		}
	}
	else if (bThrowError)
	{
		UE_LOG(LogData, Fatal, TEXT("Instance is null"));
	}

	return false;
}

} // namespace PsDataTools
