// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PsData.h"
#include "PsEvent.h"
#include "UObject/UObjectThreadContext.h"
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
};

/** see UPsData::Set{Type}Property UPsData::Get{Type}Property */
/** see FDataReflectionTools::TypeHandler<{Type}> */
/** see FDataReflection::GetTypeAsString */

struct FDataMeta
{
	bool bStrict;
	bool bEvent;
	bool bBubbles;
	FString Alias;
	FString EventType;
	
	FDataMeta()
	: bStrict(false)
	, bEvent(false)
	, bBubbles(false)
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
	
	FDataFieldDescription(const EDataFieldType& InType, const EDataContainerType& InContainerType, const FString& InName, const int32& InOffset, const int32& InSize, UClass* InClass, TArray<FString>& MetaCollection)
	: Type(InType)
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
public:
	static TMap<UClass*, TMap<FString, FDataFieldDescription>> Fields;
	static TArray<UClass*> ClassQueue;
	static TArray<FString> MetaCollection;
	
	static FString GetTypeAsString(EDataFieldType Type);
	static FString GenerateGetFunctionName(const FDataFieldDescription& Field);
	static FString GenerateSetFunctionName(const FDataFieldDescription& Field);
	static FString GenerateChangePropertyEventTypeName(const FDataFieldDescription& Field);
	
	static void AddToQueue(UPsData* Instance);
	static void RemoveFromQueue(UPsData* Instance);
	static void Fill(UPsData* Instance);
	static bool InQueue(UClass* StaticClass);
	static bool InQueue();
	
	static bool HasClass(const UClass* StaticClass);
	static const TMap<FString, FDataFieldDescription>& GetFields(const UClass* StaticClass);
	static void DeclareMeta(FString Meta);
	static void ClearMeta();
};

namespace FDataReflectionTools
{
	template <typename T> struct always_false : std::false_type {};
	
	template<typename T>
	struct TypeHandler
	{
		static_assert(always_false<T>::value, "Unsupported type");
		
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
			FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(EDataFieldType::DFT_float, ContainerType, Name, Offset, Size, FDataReflection::MetaCollection));
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
			FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(EDataFieldType::DFT_int32, ContainerType, Name, Offset, Size, FDataReflection::MetaCollection));
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
			FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(EDataFieldType::DFT_String, ContainerType, Name, Offset, Size, FDataReflection::MetaCollection));
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
			FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(EDataFieldType::DFT_bool, ContainerType, Name, Offset, Size, FDataReflection::MetaCollection));
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
			FDataReflection::Fields.FindOrAdd(StaticClass).Add(Name, FDataFieldDescription(EDataFieldType::DFT_Data, ContainerType, Name, Offset, Size, T::StaticClass(), FDataReflection::MetaCollection));
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
			if (FDataReflection::ClassQueue.Num() > 0)
			{
				UE_LOG(LogData, Error, TEXT("Can't describe: %s::%s because active class is: %s"), *StaticClass->GetName(), *Name, *FDataReflection::ClassQueue.Last()->GetName());
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
	
	template<typename T>
	bool UnsafeSet(UPsData* Instance, T& Value, const T& NewValue)
	{
		return TypeHandler<T>::SetValue(Instance, Value, const_cast<T&>(NewValue));
	}
	
	template<typename T>
	T* UnsafeGet(UPsData* Instance, int32 Offset)
	{
		return (T*)((char*)Instance + Offset);
	}
	
	template<typename T>
	void Set(UPsData* Instance, const FString& Name, const T& NewValue)
	{
		const FDataFieldDescription* Find = FDataReflection::GetFields(Instance->GetClass()).Find(Name);
		if (Find)
		{
			auto& ThreadContext = FUObjectThreadContext::Get();
			
			const FDataFieldDescription& Field = *Find;
			if (Field.Meta.bStrict && ThreadContext.ConstructedObject != Instance)
			{
				UE_LOG(LogData, Error, TEXT("Can't set strict %s::%s property"), *Instance->GetClass()->GetName(), *Field.Name)
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
				}
			}
			else
			{
				UE_LOG(LogData, Error, TEXT("Property %s::%s has another type"), *Instance->GetClass()->GetName(), *Field.Name)
			}
			return;
		}
		
		UE_LOG(LogData, Error, TEXT("Property %s::%s not found"), *Instance->GetClass()->GetName(), *Name)
	}
	
	template<typename T>
	T& Get(UPsData* Instance, const FString& Name, T& Default)
	{
		const FDataFieldDescription* Find = FDataReflection::GetFields(Instance->GetClass()).Find(Name);
		if (Find)
		{
			const FDataFieldDescription& Field = *Find;
			if (TypeHandler<T>::CheckType(Field.Type) && TypeHandler<T>::CheckContainerType(Field.ContainerType))
			{
				return *UnsafeGet<T>(Instance, Field.Offset);
			}
			else
			{
				UE_LOG(LogData, Error, TEXT("Property %s::%s has another type"), *Instance->GetClass()->GetName(), *Field.Name)
				return Default;
			}
		}
		
		UE_LOG(LogData, Error, TEXT("Property %s::%s not found"), *Instance->GetClass()->GetName(), *Name)
		return Default;
	}
}

#define TOKENPASTE(x, y, z) x ## y ## z
#define TOKENPASTE2(x, y, z) TOKENPASTE(x, y, z)
#define UNIQUE(prefix, postfix) TOKENPASTE2(prefix, __LINE__, postfix)
#define COMMA ,

#define DMETA(...) \
private: \
	struct UNIQUE(__zmeta_, _struct) { \
		UNIQUE(__zmeta_, _struct)() { \
			if (FDataReflection::InQueue()) \
				FDataReflection::DeclareMeta(TEXT(#__VA_ARGS__)); \
		} \
} UNIQUE(__zmeta_, _inst);

#define DPROP(Class, Type, Name) \
private: \
	Type Name; \
	\
public: \
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
private: \
	struct UNIQUE(__zprop_, _struct) { \
		UNIQUE(__zprop_, _struct)() { \
			if (FDataReflection::InQueue(Class::StaticClass())) { \
				FDataReflectionTools::DeclareField<Type>(Class::StaticClass(), TEXT(#Name), offsetof(Class, Name), sizeof(Type)); \
			} \
			FDataReflection::ClearMeta(); \
		} \
	} UNIQUE(__zprop_, _inst);









