// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsData.h"
#include "PsEvent.h"
#include "PsDataAccess.h"
#include "Async/Async.h"
#include <type_traits>

enum class EDataFieldType : uint8
{
	DFT_Data,
	DFT_float,
	DFT_int32,
	DFT_String,
	DFT_bool
};

enum class EDataContainerType : uint8
{
	DCT_None,
	DCT_Array,
	DCT_Map,
};

enum class EDataToken : uint8
{
	DT_Object,
	DT_Array,
	DT_Value
};

struct EDataMetaType
{
	static const FString Strict;
	static const FString Event;
	static const FString Bubbles;
	static const FString Alias;
	static const FString Link;
	static const FString Deprecated;
};

/** see UPsData::Set{Type}Property UPsData::Get{Type}Property */
/** see FDataReflectionTools::TypeHandler<{Type}> */
/** see FDataReflection::GetTypeAsString */

struct FDataMeta
{
	bool bStrict;
	bool bEvent;
	bool bBubbles;
	bool bDeprecated;
	bool bLink;
	FString LinkPath;
	FString Alias;
	FString EventType;
	
	FDataMeta()
	: bStrict(false)
	, bEvent(false)
	, bBubbles(false)
	, bDeprecated(false)
	, bLink(false)
	, LinkPath()
	, Alias()
	, EventType()
	{}
};

struct FDataFieldDescription
{
	EDataFieldType Type;
	EDataContainerType ContainerType;
	FString Name;
	int32 Offset;
	int32 Size;
	UClass* Class;
	FDataMeta Meta;
	
	FDataFieldDescription(const EDataFieldType& InType, const EDataContainerType& InContainerType, const FString& InName, const int32& InOffset, const int32& InSize, TArray<FString>& MetaCollection)
	: Type(InType)
	, ContainerType(InContainerType)
	, Name(InName)
	, Offset(InOffset)
	, Size(InSize)
	, Class(nullptr)
	{
		ParseMeta(MetaCollection);
	}
	
	FDataFieldDescription(UClass* InClass, const EDataContainerType& InContainerType, const FString& InName, const int32& InOffset, const int32& InSize, TArray<FString>& MetaCollection)
	: Type(EDataFieldType::DFT_Data)
	, ContainerType(InContainerType)
	, Name(InName)
	, Offset(InOffset)
	, Size(InSize)
	, Class(InClass)
	{
		ParseMeta(MetaCollection);
	}
	
	void ParseMeta(TArray<FString>& Collection);
};

struct PSDATAPLUGIN_API FDataReflection
{
private:
	static IPsDataAccess* DataAccess;
	static TMap<UClass*, TMap<FString, FDataFieldDescription>> Fields;
	static TArray<UClass*> ClassQueue;
	static TArray<FString> MetaCollection;
	
public:
	static FString GetTypeAsString(EDataFieldType Type);
	static FString GenerateGetFunctionName(const FDataFieldDescription& Field);
	static FString GenerateSetFunctionName(const FDataFieldDescription& Field);
	static FString GenerateChangePropertyEventTypeName(const FDataFieldDescription& Field);
	
	static void AddField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataFieldType Type, EDataContainerType ContainerType);
	static void AddField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, UClass* Type, EDataContainerType ContainerType);
	
	static void AddToQueue(UPsData* Instance);
	static void RemoveFromQueue(UPsData* Instance);
	static void Fill(UPsData* Instance);
	static bool InQueue(UClass* StaticClass);
	static bool InQueue();
	static UClass* GetLastClassInQueue();
	
	static bool HasClass(const UClass* StaticClass);
	static const TMap<FString, FDataFieldDescription>& GetFields(const UClass* StaticClass);
	static void DeclareMeta(FString Meta);
	static void ClearMeta();
	
	static void SetDataAccess(IPsDataAccess* DataAccessInterface);
	static IPsDataAccess* GetDataAccess();
};

namespace FDataReflectionTools
{
	template <typename T> struct TAlwaysFalse : std::false_type {};
	
	template <typename T> struct TRemovePointer
	{
		typedef T NonPoinerType;
		
		static T& Get(T& Value)
		{
			return Value;
		}
	};
	
	template <typename T> struct TRemovePointer<T*>
	{
		typedef T NonPoinerType;
		
		static T& Get(T*& Value)
		{
			return *Value;
		}
	};
	
	template<typename T>
	struct TypeHandler
	{
		static_assert(TAlwaysFalse<T>::value, "Unsupported type");
		
		static void DeclareField(UClass* StaticClass, const FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType);
		static bool SetValue(UPsData* Instance, T& Value, T& NewValue);
		static void Rename(T& Value, const FString& Name);
		static bool CheckType(EDataFieldType Type);
		static bool CheckContainerType(EDataContainerType Type);
	};
	
	template<>
	struct TypeHandler<float>
	{
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			FDataReflection::AddField(StaticClass, Name, Offset, Size, EDataFieldType::DFT_float, ContainerType);
		}
		
		static bool SetValue(UPsData* Instance, float& Value, float& NewValue)
		{
			if (Value == NewValue) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(float& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return EDataFieldType::DFT_float == Type;
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_None == Type;
		}
	};

	template<>
	struct TypeHandler<int32>
	{
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			FDataReflection::AddField(StaticClass, Name, Offset, Size, EDataFieldType::DFT_int32, ContainerType);
		}
		
		static bool SetValue(UPsData* Instance, int32& Value, int32& NewValue)
		{
			if (Value == NewValue) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(int32& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return EDataFieldType::DFT_int32 == Type;
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_None == Type;
		}
	};

	template<>
	struct TypeHandler<FString>
	{
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			FDataReflection::AddField(StaticClass, Name, Offset, Size, EDataFieldType::DFT_String, ContainerType);
		}
		
		static bool SetValue(UPsData* Instance, FString& Value, FString& NewValue)
		{
			if (Value == NewValue) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(FString& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return EDataFieldType::DFT_String == Type;
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_None == Type;
		}
	};
	
	template<>
	struct TypeHandler<bool>
	{
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			FDataReflection::AddField(StaticClass, Name, Offset, Size, EDataFieldType::DFT_bool, ContainerType);
		}
		
		static bool SetValue(UPsData* Instance, bool& Value, bool& NewValue)
		{
			if (Value == NewValue) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(bool& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return EDataFieldType::DFT_bool == Type;
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_None == Type;
		}
	};
	
	template<typename T>
	struct TypeHandler<T*>
	{
		static_assert(std::is_base_of<UPsData, T>::value, "Pointer mast be only UPsData");
		
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			FDataReflection::AddField(StaticClass, Name, Offset, Size, T::StaticClass(), ContainerType);
		}
		
		static bool SetValue(UPsData* Instance, T*& Value, T*& NewValue)
		{
			if (Value == NewValue) return false;
			
			if (Value)
			{
				FPsDataFriend::RemoveChild(Instance, Value);
			}
			
			Value = NewValue;
			
			if (NewValue)
			{
				FPsDataFriend::AddChild(Instance, NewValue);
			}
			
			return true;
		}
		
		static void Rename(T*& Value, const FString& Name)
		{
			FPsDataFriend::ChangeDataName(Value, Name);
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return EDataFieldType::DFT_Data == Type;
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_None == Type;
		}
	};
	
	template<typename T>
	struct TypeHandler<TArray<T>>
	{
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			TypeHandler<T>::DeclareField(StaticClass, Name, Offset, Size, EDataContainerType::DCT_Array);
		}
		
		static bool SetValue(UPsData* Instance, TArray<T>& Value, TArray<T>& NewValue)
		{
			// TODO: need optimization
			if (Value == NewValue) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(TArray<T>& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return TypeHandler<T>::CheckType(Type);
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_Array == Type;
		}
	};
	
	template<typename T>
	struct TypeHandler<TArray<T*>>
	{
		static_assert(std::is_base_of<UPsData, T>::value, "Pointer mast be only UPsData");
		
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			TypeHandler<T*>::DeclareField(StaticClass, Name, Offset, Size, EDataContainerType::DCT_Array);
		}
		
		static bool SetValue(UPsData* Instance, TArray<T*>& Value, TArray<T*>& NewValue)
		{
			// TODO: need optimization
			bool bChange = false;
			TSet<T*> ValueSet(Value);
			TSet<T*> NewValueSet(NewValue);
			for(int32 i = 0; i < Value.Num(); ++i)
			{
				if (!NewValueSet.Contains(Value[i]))
				{
					FPsDataFriend::RemoveChild(Instance, Value[i]);
					bChange = true;
				}
			}
			
			for(int32 i = 0; i < NewValue.Num(); ++i)
			{
				if (!ValueSet.Contains(NewValue[i]))
				{
					FPsDataFriend::AddChild(Instance, NewValue[i]);
					bChange = true;
				}
			}
			
			if (!bChange) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(TArray<T*>& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return TypeHandler<T*>::CheckType(Type);
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_Array == Type;
		}
	};
	
	template<typename T>
	struct TypeHandler<TMap<FString, T>>
	{
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			TypeHandler<T>::DeclareField(StaticClass, Name, Offset, Size, EDataContainerType::DCT_Map);
		}
		
		static bool SetValue(UPsData* Instance, TMap<FString, T>& Value, TMap<FString, T>& NewValue)
		{
			// TODO: need optimization
			bool bChange = false;
			if (Value.Num() == NewValue.Num())
			{
				for(auto& Pair : Value)
				{
					if (T* ValueFromMap = NewValue.Find(Pair.Key))
					{
						if (*ValueFromMap != Pair.Value)
						{
							bChange = true;
							break;
						}
					}
					else
					{
						bChange = true;
						break;
					}
				}
			}
			else
			{
				bChange = true;
			}
			
			if (!bChange) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(TMap<FString, T>& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return TypeHandler<T>::CheckType(Type);
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_Map == Type;
		}
	};
	
	template<typename T>
	struct TypeHandler<TMap<FString, T*>>
	{
		static_assert(std::is_base_of<UPsData, T>::value, "Pointer mast be only UPsData");
		
		static void DeclareField(UClass* StaticClass, FString& Name, int32 Offset, int32 Size, EDataContainerType ContainerType)
		{
			TypeHandler<T*>::DeclareField(StaticClass, Name, Offset, Size, EDataContainerType::DCT_Map);
		}
		
		static bool SetValue(UPsData* Instance, TMap<FString, T*>& Value, TMap<FString, T*>& NewValue)
		{
			// TODO: need optimization
			bool bChange = false;
			
			TSet<T*> ValueSet;
			ValueSet.Reserve(Value.Num());
			for(auto& Pair : Value)
			{
				ValueSet.Add(Pair.Value);
			}
			
			TSet<T*> NewValueSet;
			NewValueSet.Reserve(NewValue.Num());
			for(auto& Pair : NewValue)
			{
				NewValueSet.Add(Pair.Value);
			}
			
			for(auto& Pair : Value)
			{
				if (!NewValueSet.Contains(Pair.Value))
				{
					FPsDataFriend::RemoveChild(Instance, Pair.Value);
					bChange = true;
				}
			}
			
			for(auto& Pair : NewValue)
			{
				if (!ValueSet.Contains(Pair.Value))
				{
					FPsDataFriend::AddChild(Instance, Pair.Value);
					bChange = true;
				}
				if (Pair.Value->GetName() != Pair.Key)
				{
					FPsDataFriend::ChangeDataName(Pair.Value, Pair.Key);
					bChange = true;
				}
			}
			
			if (!bChange) return false;
			Value = NewValue;
			return true;
		}
		
		static void Rename(TMap<FString, T*>& Value, const FString& Name)
		{
			
		}
		
		static bool CheckType(EDataFieldType Type)
		{
			return TypeHandler<T*>::CheckType(Type);
		}
		
		static bool CheckContainerType(EDataContainerType Type)
		{
			return EDataContainerType::DCT_Map == Type;
		}
	};

	template<typename T>
	void DeclareField(UClass* StaticClass, FString Name, int32 Offset, int32 Size)
	{
		if (!FDataReflection::InQueue(StaticClass))
		{
			if (FDataReflection::InQueue())
			{
				UE_LOG(LogData, Error, TEXT("Can't describe: %s::%s another class is active: %s"), *StaticClass->GetName(), *Name, *FDataReflection::GetLastClassInQueue()->GetName());
			}
			else
			{
				UE_LOG(LogData, Error, TEXT("Can't describe: %s::%s because queue is empty"), *StaticClass->GetName(), *Name);
			}
			return;
		}
		
		UE_LOG(LogData, Verbose, TEXT(" + %s::%s"), *StaticClass->GetName(), *Name)
		TypeHandler<T>::DeclareField(StaticClass, Name, Offset, Size, EDataContainerType::DCT_None);
	}
	
	/***********************************
	 * UNSAFE SET/GET
	 ***********************************/
	
	template<typename T>
	bool UnsafeSet(UPsData* Instance, T& Value, T& NewValue)
	{
		return TypeHandler<T>::SetValue(Instance, Value, NewValue);
	}
	
	template<typename T>
	T* UnsafeGet(UPsData* Instance, int32 Offset)
	{
		return (T*)((char*)Instance + Offset);
	}
	
	/***********************************
	 * SET
	 ***********************************/
	
	template<typename T>
	void Set(UPsData* Instance, const TArray<FString>& Path, int32 PathOffset, T& NewValue, bool bCreate = false)
	{
		UE_LOG(LogData, Fatal, TEXT("Not supported"));
	}
	
	template<typename T>
	void Set(UPsData* Instance, const FString& Name, T& NewValue, bool bCreate = false)
	{
		IPsDataAccess* DataAccess = FDataReflection::GetDataAccess();
		if (DataAccess != nullptr && !DataAccess->CanModify())
		{
			UE_LOG(LogData, Fatal, TEXT("You can't edit property (see IPsDataAccess::CanModify())"))
		}
		
		const FDataFieldDescription* Find = FDataReflection::GetFields(Instance->GetClass()).Find(Name);
		if (Find)
		{
			const FDataFieldDescription& Field = *Find;
			if (Field.Meta.bStrict && !Instance->HasAnyFlags(EObjectFlags::RF_NeedInitialization))
			{
				UE_LOG(LogData, Error, TEXT("Can't set strict %s::%s property"), *Instance->GetClass()->GetName(), *Field.Name);
				return;
			}
			
			if (TypeHandler<T>::CheckType(Field.Type) && TypeHandler<T>::CheckContainerType(Field.ContainerType))
			{
				T& Value = *UnsafeGet<T>(Instance, Field.Offset);
				if (UnsafeSet<T>(Instance, Value, NewValue))
				{
					TypeHandler<T>::Rename(Value, Name);
					if (Field.Meta.bEvent)
					{
						Instance->Broadcast(UPsEvent::ConstructEvent(FDataReflection::GenerateChangePropertyEventTypeName(Field), Field.Meta.bBubbles));
					}
					
					if (!FPsDataFriend::IsChanged(Instance))
					{
						FPsDataFriend::SetIsChanged(Instance, true);
						TWeakObjectPtr<UPsData> InstanceWeakPtr(Instance);
						AsyncTask(ENamedThreads::GameThread, [InstanceWeakPtr]()
						{
							if (InstanceWeakPtr.IsValid())
							{
								FPsDataFriend::SetIsChanged(InstanceWeakPtr.Get(), false);
								InstanceWeakPtr->Broadcast(UPsEvent::ConstructEvent(TEXT("Changed"), false));
							}
						});
					}
				}
			}
			else
			{
				UE_LOG(LogData, Error, TEXT("Property %s::%s has another type"), *Instance->GetClass()->GetName(), *Field.Name);
			}
			return;
		}
		else
		{
			TArray<FString> Path;
			Name.ParseIntoArray(Path, TEXT("."));
			if (Path.Num() > 1)
			{
				Set<T>(Instance, Path, 0, NewValue, bCreate);
				return;
			}
		}
		
		UE_LOG(LogData, Error, TEXT("Property %s::%s is not found"), *Instance->GetClass()->GetName(), *Name);
	}
	
	/***********************************
	 * GET
	 ***********************************/
	
	template<typename T>
	bool Get(UPsData* Instance, const FDataFieldDescription& Field, T*& OutValue)
	{
		check(Instance);
		
		if (TypeHandler<T>::CheckType(Field.Type) && TypeHandler<T>::CheckContainerType(Field.ContainerType))
		{
			OutValue = UnsafeGet<T>(Instance, Field.Offset);
			return true;
		}
		else
		{
			OutValue = nullptr;
			
			UE_LOG(LogData, Error, TEXT("Property %s::%s has another type"), *Instance->GetClass()->GetName(), *Field.Name);
			return false;
		}
		
		OutValue = nullptr;
		
		UE_LOG(LogData, Error, TEXT("Property %s::%s is not found"), *Instance->GetClass()->GetName(), *Field.Name);
		return false;
	}
	
	template<typename T>
	bool Get(UPsData* Instance, const TArray<FString>& Path, int32 PathOffset, int32 PathLength, T*& OutValue)
	{
		const int32 Delta = PathLength - PathOffset;
		OutValue = nullptr;
		
		check(PathLength <= Path.Num());
		check(Delta > 0);
		check(Instance);
		
		const FDataFieldDescription* Find = FDataReflection::GetFields(Instance->GetClass()).Find(Path[PathOffset]);
		if (Find)
		{
			const FDataFieldDescription& Field = *Find;
			if (Delta == 1)
			{
				return Get<T>(Instance, Field, OutValue);
			}
			else if (Delta > 1)
			{
				if (Field.ContainerType == EDataContainerType::DCT_None)
				{
					if (Field.Type == EDataFieldType::DFT_Data)
					{
						UPsData** DataPtr = nullptr;
						if (Get<UPsData*>(Instance, Path, PathOffset, PathOffset + 1, DataPtr))
						{
							UPsData* Data = *DataPtr;
							if (Data)
							{
								return Get<T>(Data, Path, PathOffset + 1, Path.Num(), OutValue);
							}
							else
							{
								UE_LOG(LogData, Error, TEXT("Property %s::%s is null"), *Instance->GetClass()->GetName(), *Field.Name);
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
						UE_LOG(LogData, Error, TEXT("Property %s::%s doesn't contain children"), *Instance->GetClass()->GetName(), *Field.Name);
						return false;
					}
				}
				else if (Field.ContainerType == EDataContainerType::DCT_Array)
				{
					if (Field.Type == EDataFieldType::DFT_Data)
					{
						TArray<UPsData*>* ArrayPtr = nullptr;
						if (Get<TArray<UPsData*>>(Instance, Field, ArrayPtr))
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
										return Get<T>(Array[ArrayIndex], Path, PathOffset + 2, Path.Num(), OutValue);
									}
									else
									{
										UE_LOG(LogData, Error, TEXT("Property %s::%s[%d] is null"), *Instance->GetClass()->GetName(), *Field.Name, ArrayIndex);
										return false;
									}
								}
								else
								{
									UE_LOG(LogData, Error, TEXT("Property %s::%s[%d] is not found"), *Instance->GetClass()->GetName(), *Field.Name, ArrayIndex);
									return false;
								}
							}
							else
							{
								UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] index is not valid"), *Instance->GetClass()->GetName(), *Field.Name, *StringArrayIndex);
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
						if (Get<TArray<T>>(Instance, Field, ArrayPtr))
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
									UE_LOG(LogData, Error, TEXT("Property %s::%s[%d] is not found"), *Instance->GetClass()->GetName(), *Field.Name, ArrayIndex);
									return false;
								}
							}
							else
							{
								UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] index is not valid"), *Instance->GetClass()->GetName(), *Field.Name, *StringArrayIndex);
								return false;
							}
						}
						else
						{
							return false;
						}
					}
				}
				else if (Field.ContainerType == EDataContainerType::DCT_Map)
				{
					if (Field.Type == EDataFieldType::DFT_Data)
					{
						TMap<FString, UPsData*>* MapPtr = nullptr;
						if (Get<TMap<FString, UPsData*>>(Instance, Field, MapPtr))
						{
							TMap<FString, UPsData*>& Map = *MapPtr;
							const FString& Key = Path[PathOffset + 1];
							UPsData** DataPtr = Map.Find(Key);
							if (DataPtr)
							{
								return Get<T>(*DataPtr, Path, PathOffset + 2, Path.Num(), OutValue);
							}
							else
							{
								UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] is not found"), *Instance->GetClass()->GetName(), *Field.Name, *Key);
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
						if (Get<TMap<FString, T>>(Instance, Field, MapPtr))
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
								UE_LOG(LogData, Error, TEXT("Property %s::%s[%s] is not found"), *Instance->GetClass()->GetName(), *Field.Name, *Key);
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
		
		UE_LOG(LogData, Error, TEXT("Unknown error"));
		return false;
	}
	
	template<typename T>
	bool Get(UPsData* Instance, const FString& Name, T*& OutValue)
	{
		check(Instance);
		
		const FDataFieldDescription* Find = FDataReflection::GetFields(Instance->GetClass()).Find(Name);
		if (Find)
		{
			return Get<T>(Instance, *Find, OutValue);
		}
		else
		{
			TArray<FString> Path;
			Name.ParseIntoArray(Path, TEXT("."));
			if (Path.Num() > 1)
			{
				return Get<T>(Instance, Path, 0, Path.Num(), OutValue);
			}
		}
		
		OutValue = nullptr;
		
		UE_LOG(LogData, Error, TEXT("Property %s::%s is not found"), *Instance->GetClass()->GetName(), *Name);
		return false;
	}
	
	/***********************************
	 * Link Type trait
	 ***********************************/
	
	template <typename T, typename K> struct TLinkType
	{
		typedef K ResultType;
		
		static void Get(UPsData* Instance, const FString& Property, const FString& Path)
		{
			static_assert(TAlwaysFalse<T>::value, "Unsupported type");
		}
	};
	
	template <typename K> struct TLinkType<FString, K>
	{
		typedef K* ResultType;

		static ResultType Get(UPsData* Instance, const FString& Path, const FString& Property)
		{
			return Cast<K>(Instance->GetDataProperty_Link(Path, Property));
		}
	};

	template <typename K> struct TLinkType<TArray<FString>, K>
	{
		typedef TArray<K*> ResultType;

		static ResultType Get(UPsData* Instance, const FString& Path, const FString& Property)
		{
			TArray<UPsData*> Array = Instance->GetDataArrayProperty_Link(Path, Property);
			ResultType ResultArray;
			for(UPsData* Data : Array)
			{
				ResultArray.Add(Cast<K>(Data));
			}
			return ResultArray;
		}
	};
	
}

/** Private macros */

#define _TOKENPASTE(x, y, z) x ## y ## z
#define _TOKENPASTE2(x, y, z) _TOKENPASTE(x, y, z)
#define _UNIQUE(prefix, postfix) _TOKENPASTE2(prefix, __LINE__, postfix)

/** Public macros */

#define COMMA ,

#define DMETA(...) \
private: \
	struct _UNIQUE(__zmeta_, _struct) { \
		_UNIQUE(__zmeta_, _struct)() { \
			if (FDataReflection::InQueue()) \
				FDataReflection::DeclareMeta(TEXT(#__VA_ARGS__)); \
		} \
} _UNIQUE(__zmeta_, _inst);

/** DPROP */

#define DPROP(Class, Type, Name) \
protected: \
	Type Name; \
	\
public: \
	const typename FDataReflectionTools::TRemovePointer< Type >::NonPoinerType& Get##Name##Ref() \
	{ \
		return FDataReflectionTools::TRemovePointer< Type >::Get(Name); \
	} \
	\
	Type Get##Name() const \
	{ \
		return Name; \
	} \
	\
	void Set##Name(Type& Value) \
	{ \
		FDataReflectionTools::Set<Type>(this, TEXT(#Name), Value); \
	} \
	\
	void Set##Name(Type&& Value) \
	{ \
		FDataReflectionTools::Set<Type>(this, TEXT(#Name), Value); \
	} \
	\
private: \
	struct _UNIQUE(__zprop_, _struct) { \
		_UNIQUE(__zprop_, _struct)() { \
			if (FDataReflection::InQueue(Class::StaticClass())) \
				FDataReflectionTools::DeclareField<Type>(Class::StaticClass(), TEXT(#Name), offsetof(Class, Name), sizeof(Type)); \
			FDataReflection::ClearMeta(); \
		} \
	} _UNIQUE(__zprop_, _inst);

/** DPROP with link */

#define DPROP_LINK(Class, Type, Name, Path, ReturnType) \
	static_assert(std::is_base_of<UPsData, FDataReflectionTools::TRemovePointer<ReturnType>::NonPoinerType>::value, "Only UPsData can be return type"); \
	static_assert(std::is_base_of<FString, Type>::value || std::is_base_of<TArray<FString>, Type>::value, "Only FString or TArray<FString> property can be linked"); \
	\
	DMETA(Link=Type::Path::ReturnType) \
	DPROP(Class, Type, Name) \
	\
public: \
	FDataReflectionTools::TLinkType<Type, FDataReflectionTools::TRemovePointer<ReturnType>::NonPoinerType>::ResultType Get##Name##_Link() \
	{ \
		return FDataReflectionTools::TLinkType<Type, FDataReflectionTools::TRemovePointer<ReturnType>::NonPoinerType>::Get(this, TEXT(#Name), TEXT(#Path)); \
	}

/** DPROP deprecated */

#define DPROP_DEPRECATED(Class, Type, Name) \
	DMETA(Deprecated) \
protected: \
	DEPRECATED(0, "Property was marked as deprecated") \
	Type Name; \
	\
public: \
	DEPRECATED(0, "Property was marked as deprecated") \
	Type Get##Name() const \
	{ \
		return Name; \
	} \
	\
	DEPRECATED(0, "Property was marked as deprecated") \
	void Set##Name(Type& Value) \
	{ \
		FDataReflectionTools::Set<Type>(this, TEXT(#Name), Value); \
	} \
	\
	DEPRECATED(0, "Property was marked as deprecated") \
	void Set##Name(Type&& Value) \
	{ \
		FDataReflectionTools::Set<Type>(this, TEXT(#Name), Value); \
	} \
	\
private: \
	struct _UNIQUE(__zprop_, _struct) { \
		_UNIQUE(__zprop_, _struct)() { \
			if (FDataReflection::InQueue(Class::StaticClass())) \
				FDataReflectionTools::DeclareField<Type>(Class::StaticClass(), TEXT(#Name), offsetof(Class, Name), sizeof(Type)); \
			FDataReflection::ClearMeta(); \
		} \
	} _UNIQUE(__zprop_, _inst);

#define DPROP_LINK_DEPRECATED(Class, Type, Name, Path, ReturnType) \
	static_assert(std::is_base_of<UPsData, FDataReflectionTools::TRemovePointer<ReturnType>::NonPoinerType>::value, "Only UPsData can be return type"); \
	static_assert(std::is_base_of<FString, Type>::value || std::is_base_of<TArray<FString>, Type>::value, "Only FString or TArray<FString> property can be linked"); \
	\
	DMETA(Link=Type::Path::ReturnType) \
	DPROP_DEPRECATED(Class, Type, Name) \
	\
public: \
	DEPRECATED(0, "Property was marked as deprecated") \
	FDataReflectionTools::TLinkType<Type, FDataReflectionTools::TRemovePointer<ReturnType>::NonPoinerType>::ResultType Get##Name##_Link()  \
	{ \
		return FDataReflectionTools::TLinkType<Type, FDataReflectionTools::TRemovePointer<ReturnType>::NonPoinerType>::Get(this, TEXT(#Name), TEXT(#Path)); \
	}

#define DMAP(Class, Type, Name) DPROP(Class, TMap<FString COMMA Type>, Name)

#define DMAP_DEPRECATED(Class, Type, Name) DPROP_DEPRECATED(Class, TMap<FString COMMA Type>, Name)
