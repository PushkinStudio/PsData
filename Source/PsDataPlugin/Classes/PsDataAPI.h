// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Collection/PsDataArrayProxy.h"
#include "Collection/PsDataMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataFunctionLibrary.h"
#include "PsDataHardObjectPtr.h"
#include "Serialize/PsDataJsonSerialization.h"
#include "Serialize/PsDataSerialization.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

/***********************************
 * Macro DMETA
 ***********************************/

#define DMETA(...) \
protected:         \
	const FDataReflectionTools::FDMeta _UNIQ(meta){#__VA_ARGS__};

/***********************************
 * Macro DPROP
 ***********************************/

#define DPROP(__Type__, __Name__)                                                                                      \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP is available only for non-container types");                                                      \
                                                                                                                       \
	_REFLECT(__Type__, __Name__);                                                                                      \
                                                                                                                       \
public:                                                                                                                \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	typename FDataReflectionTools::TConstRef<__Type__>::Type Get##__Name__()                                           \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(this, _UNIQ(hash), Output);                                                    \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	void Set##__Name__(FDataReflectionTools::TConstRef<__Type__>::Type Value)                                          \
	{                                                                                                                  \
		FDataReflectionTools::SetByHash<__Type__>(this, _UNIQ(hash), Value);                                           \
	}                                                                                                                  \
                                                                                                                       \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                        \
	{                                                                                                                  \
		return Bind(_UNIQ(hash), Delegate);                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	static const FString& Get##__Name__##ChangedEventName()                                                            \
	{                                                                                                                  \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                     \
	}

/***********************************
 * Macro DPROP_DEPRECATED
 ***********************************/

#define DPROP_DEPRECATED(__Type__, __Name__)                                                                           \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP_DEPRECATED is available only for non-container types");                                           \
                                                                                                                       \
	DMETA(Deprecated);                                                                                                 \
	_REFLECT(__Type__, __Name__);                                                                                      \
                                                                                                                       \
public:                                                                                                                \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	typename FDataReflectionTools::TConstRef<__Type__>::Type Get##__Name__()                                           \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(this, _UNIQ(hash), Output);                                                    \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	void Set##__Name__(FDataReflectionTools::TConstRef<__Type__>::Type Value)                                          \
	{                                                                                                                  \
		FDataReflectionTools::SetByHash<__Type__>(this, _UNIQ(hash), Value);                                           \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                        \
	{                                                                                                                  \
		return Bind(_UNIQ(hash), Delegate);                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	static const FString& Get##__Name__##ChangedEventName()                                                            \
	{                                                                                                                  \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                     \
	}

/***********************************
 * Macro DPROP_CONST
 ***********************************/

#define DPROP_CONST(__Type__, __Name__, __Friend__)                                                                    \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsPsData<__Type__>::Value,                                                              \
		"Macro DPROP_CONST is available only for UPsData");                                                            \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP_CONST is available only for non-container types");                                                \
                                                                                                                       \
	DMETA(ReadOnly, Strict);                                                                                           \
	_REFLECT(__Type__, __Name__);                                                                                      \
                                                                                                                       \
public:                                                                                                                \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	struct FMutable_##__Name__                                                                                         \
	{                                                                                                                  \
		FMutable_##__Name__(UPsData* Instance)                                                                         \
			: This(Instance)                                                                                           \
		{                                                                                                              \
		}                                                                                                              \
                                                                                                                       \
	private:                                                                                                           \
		friend class __Friend__;                                                                                       \
                                                                                                                       \
		UPsData* This;                                                                                                 \
                                                                                                                       \
		typename FDataReflectionTools::TConstRef<__Type__>::Type operator()() const                                    \
		{                                                                                                              \
			__Type__* Output = nullptr;                                                                                \
			FDataReflectionTools::GetByHash(This, _UNIQ(hash), Output);                                                \
			return *Output;                                                                                            \
		}                                                                                                              \
	} const GetMutable##__Name__{this};                                                                                \
                                                                                                                       \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                        \
	{                                                                                                                  \
		return Bind(_UNIQ(hash), Delegate);                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	static const FString& Get##__Name__##ChangedEventName()                                                            \
	{                                                                                                                  \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                     \
	}

/***********************************
 * Macro DPROP_CONST_DEPRECATED
 ***********************************/

#define DPROP_CONST_DEPRECATED(__Type__, __Name__, __Friend__)                                                         \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsPsData<__Type__>::Value,                                                              \
		"Macro DPROP_CONST_DEPRECATED is available only for UPsData");                                                 \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP_CONST_DEPRECATED is available only for non-container types");                                     \
                                                                                                                       \
	DMETA(ReadOnly, Strict, Deprecated);                                                                               \
	_REFLECT(__Type__, __Name__);                                                                                      \
                                                                                                                       \
public:                                                                                                                \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	struct FMutable_##__Name__                                                                                         \
	{                                                                                                                  \
		FMutable_##__Name__(UPsData* Instance)                                                                         \
			: This(Instance)                                                                                           \
		{                                                                                                              \
		}                                                                                                              \
                                                                                                                       \
	private:                                                                                                           \
		friend class __Friend__;                                                                                       \
                                                                                                                       \
		UPsData* This;                                                                                                 \
                                                                                                                       \
		DEPRECATED(0, "Property was marked as deprecated")                                                             \
		typename FDataReflectionTools::TConstRef<__Type__>::Type operator()() const                                    \
		{                                                                                                              \
			__Type__* Output = nullptr;                                                                                \
			FDataReflectionTools::GetByHash(This, _UNIQ(hash), Output);                                                \
			return *Output;                                                                                            \
		}                                                                                                              \
	} const GetMutable##__Name__{this};                                                                                \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                        \
	{                                                                                                                  \
		return Bind(_UNIQ(hash), Delegate);                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	static const FString& Get##__Name__##ChangedEventName()                                                            \
	{                                                                                                                  \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                     \
	}

/***********************************
 * Macro DARRAY
 ***********************************/

#define DARRAY(__Type__, __Name__)                                                                                     \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<TArray<__Type__>>::Array,                                                   \
		"Macro DARRAY is available only for TArray<ContentType> type");                                                \
                                                                                                                       \
	_REFLECT(TArray<__Type__>, __Name__);                                                                              \
                                                                                                                       \
public:                                                                                                                \
	FPsDataConstArrayProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                  \
		return FPsDataConstArrayProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash)); \
	}                                                                                                                  \
                                                                                                                       \
	FPsDataArrayProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                  \
		return FPsDataArrayProxy<__Type__>(this, _UNIQ(hash));                                                         \
	}                                                                                                                  \
                                                                                                                       \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                        \
	{                                                                                                                  \
		return Bind(_UNIQ(hash), Delegate);                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	static const FString& Get##__Name__##ChangedEventName()                                                            \
	{                                                                                                                  \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                     \
	}

/***********************************
 * Macro DARRAY_DEPRECATED
 ***********************************/

#define DARRAY_DEPRECATED(__Type__, __Name__)                                                                          \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<TArray<__Type__>>::Array,                                                   \
		"Macro DARRAY_DEPRECATED is available only for TArray<ContentType> type");                                     \
                                                                                                                       \
	DMETA(Deprecated)                                                                                                  \
	_REFLECT(TArray<__Type__>, __Name__)                                                                               \
                                                                                                                       \
public:                                                                                                                \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataConstArrayProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                  \
		return FPsDataConstArrayProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash)); \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataArrayProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                  \
		return FPsDataArrayProxy<__Type__>(this, _UNIQ(hash));                                                         \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                        \
	{                                                                                                                  \
		return Bind(_UNIQ(hash), Delegate);                                                                            \
	}                                                                                                                  \
                                                                                                                       \
	static const FString& Get##__Name__##ChangedEventName()                                                            \
	{                                                                                                                  \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                     \
	}

/***********************************
 * Macro DMAP
 ***********************************/

#define DMAP(__Type__, __Name__)                                                                                     \
	static_assert(                                                                                                   \
		FDataReflectionTools::TIsContainer<TMap<FString, __Type__>>::Map,                                            \
		"Macro DMAP is available only for TMap<FString, ContentType> type");                                         \
                                                                                                                     \
	_REFLECT(TMap<FString COMMA __Type__>, __Name__);                                                                \
                                                                                                                     \
public:                                                                                                              \
	FPsDataConstMapProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                \
		return FPsDataConstMapProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash)); \
	}                                                                                                                \
                                                                                                                     \
	FPsDataMapProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                \
		return FPsDataMapProxy<__Type__>(this, _UNIQ(hash));                                                         \
	}                                                                                                                \
                                                                                                                     \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                      \
	{                                                                                                                \
		return Bind(_UNIQ(hash), Delegate);                                                                          \
	}                                                                                                                \
                                                                                                                     \
	static const FString& Get##__Name__##ChangedEventName()                                                          \
	{                                                                                                                \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                   \
	}

/***********************************
 * Macro DMAP_DEPRECATED
 ***********************************/

#define DMAP_DEPRECATED(__Type__, __Name__)                                                                          \
	static_assert(                                                                                                   \
		FDataReflectionTools::TIsContainer<TMap<FString, __Type__>>::Map,                                            \
		"Macro DMAP_DEPRECATED is available only for TMap<FString, ContentType> type");                              \
                                                                                                                     \
	DMETA(Deprecated);                                                                                               \
	_REFLECT(TMap<FString COMMA __Type__>, __Name__);                                                                \
                                                                                                                     \
public:                                                                                                              \
	DEPRECATED(0, "Property was marked as deprecated")                                                               \
	FPsDataConstMapProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                \
		return FPsDataConstMapProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIQ(hash)); \
	}                                                                                                                \
                                                                                                                     \
	DEPRECATED(0, "Property was marked as deprecated")                                                               \
	FPsDataMapProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                \
		return FPsDataMapProxy<__Type__>(this, _UNIQ(hash));                                                         \
	}                                                                                                                \
                                                                                                                     \
	DEPRECATED(0, "Property was marked as deprecated")                                                               \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                      \
	{                                                                                                                \
		return Bind(_UNIQ(hash), Delegate);                                                                          \
	}                                                                                                                \
                                                                                                                     \
	static const FString& Get##__Name__##ChangedEventName()                                                          \
	{                                                                                                                \
		return FDataReflection::GetFieldByHash(StaticClass(), _UNIQ(hash))->GetChangedEventName();                   \
	}

/***********************************
 * Macro DLINK
 ***********************************/

#define DLINK(__ReturnType__, __Name__, __Path__)    \
private:                                             \
	typedef decltype(_zprop##__Name__) _UNIQ(dprop); \
                                                     \
public:                                              \
	const FDataReflectionTools::FDLink<_UNIQ(dprop)::PropType, __ReturnType__, _UNIQ(dprop)::PropHash> LinkBy##__Name__{#__Name__, #__Path__, #__ReturnType__, this};

/***********************************
 * Macro DLINK_DEPRECATED
 ***********************************/

#define DLINK_DEPRECATED(__ReturnType__, __Name__, __Path__) \
private:                                                     \
	typedef decltype(_zprop##__Name__) _UNIQ(dprop);         \
                                                             \
public:                                                      \
	DEPRECATED(0, "Property was marked as deprecated")       \
	const FDataReflectionTools::FDLink<_UNIQ(dprop)::PropType, __ReturnType__, _UNIQ(dprop)::PropHash> LinkBy##__Name__{#__Name__, #__Path__, #__ReturnType__, this};

/***********************************
 * Macro DLINK_ABSTRACT
 ***********************************/

#define DLINK_ABSTRACT(__ReturnType__, __Name__)     \
private:                                             \
	typedef decltype(_zprop##__Name__) _UNIQ(dprop); \
                                                     \
public:                                              \
	const FDataReflectionTools::FDLink<_UNIQ(dprop)::PropType, __ReturnType__, _UNIQ(dprop)::PropHash> LinkByAbstract##__Name__{#__Name__, #__ReturnType__, this};

/***********************************
 * Macro DLINK_ABSTRACT_DEPRECATED
 ***********************************/

#define DLINK_ABSTRACT_DEPRECATED(__ReturnType__, __Name__) \
private:                                                    \
	typedef decltype(_zprop##__Name__) _UNIQ(dprop);        \
                                                            \
public:                                                     \
	DEPRECATED(0, "Property was marked as deprecated")      \
	const FDataReflectionTools::FDLink<_UNIQ(dprop)::PropType, __ReturnType__, _UNIQ(dprop)::PropHash> LinkByAbstract##__Name__{#__Name__, #__ReturnType__, this};

/***********************************
 * Macro DESCRIBE_ENUM
 ***********************************/

#define DESCRIBE_ENUM(__Type__)                                                                                                   \
	template <>                                                                                                                   \
	struct FDataTypeContext<__Type__> : public FEnumDataTypeContext<__Type__>                                                     \
	{                                                                                                                             \
		virtual ~FDataTypeContext() {}                                                                                            \
	};                                                                                                                            \
                                                                                                                                  \
	template <>                                                                                                                   \
	struct FDataTypeContext<TArray<__Type__>> : public FEnumDataTypeContext<TArray<__Type__>>                                     \
	{                                                                                                                             \
		virtual ~FDataTypeContext() {}                                                                                            \
	};                                                                                                                            \
                                                                                                                                  \
	namespace FDataReflectionTools                                                                                                \
	{                                                                                                                             \
	template <>                                                                                                                   \
	struct FTypeDefault<__Type__>                                                                                                 \
	{                                                                                                                             \
		static const __Type__ GetDefaultValue() { return static_cast<__Type__>(0); }                                              \
	};                                                                                                                            \
                                                                                                                                  \
	template <>                                                                                                                   \
	struct FTypeSerializer<__Type__>                                                                                              \
	{                                                                                                                             \
		static void Serialize(const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer, const __Type__ Value)     \
		{                                                                                                                         \
			UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());                                                              \
			if (Enum)                                                                                                             \
			{                                                                                                                     \
				Serializer->WriteValue(Enum->GetNameStringByValue(static_cast<int64>(Value)));                                    \
			}                                                                                                                     \
			else                                                                                                                  \
			{                                                                                                                     \
				Serializer->WriteValue(static_cast<uint8>(Value));                                                                \
			}                                                                                                                     \
		}                                                                                                                         \
	};                                                                                                                            \
                                                                                                                                  \
	template <>                                                                                                                   \
	struct FTypeDeserializer<__Type__>                                                                                            \
	{                                                                                                                             \
		static __Type__ Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer)                 \
		{                                                                                                                         \
			UEnum* Enum = Cast<UEnum>(Field->Context->GetUE4Type());                                                              \
			uint8 Result = 0;                                                                                                     \
			if (Enum)                                                                                                             \
			{                                                                                                                     \
				FString String;                                                                                                   \
				if (Deserializer->ReadValue(String))                                                                              \
				{                                                                                                                 \
					Result = static_cast<uint8>(Enum->GetValueByNameString(String, EGetByNameFlags::None));                       \
				}                                                                                                                 \
			}                                                                                                                     \
			else                                                                                                                  \
			{                                                                                                                     \
				Deserializer->ReadValue(Result);                                                                                  \
			}                                                                                                                     \
			return static_cast<__Type__>(Result);                                                                                 \
		}                                                                                                                         \
                                                                                                                                  \
		static __Type__ Deserialize(const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer, __Type__ Value) \
		{                                                                                                                         \
			return Deserialize(Field, Deserializer);                                                                              \
		}                                                                                                                         \
	};                                                                                                                            \
                                                                                                                                  \
	template <class ReturnType, int32 Hash>                                                                                       \
	struct FDLink<__Type__, ReturnType, Hash> : public FDLinkBase<__Type__, ReturnType, Hash>                                     \
	{                                                                                                                             \
	public:                                                                                                                       \
		FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false) \
			: FDLinkBase<__Type__, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)                           \
		{                                                                                                                         \
		}                                                                                                                         \
                                                                                                                                  \
		FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)                                           \
			: FDLinkBase<__Type__, ReturnType, Hash>(Name, CharReturnType, InInstance)                                            \
		{                                                                                                                         \
		}                                                                                                                         \
                                                                                                                                  \
		typename FDLinkHelper<ReturnType, Hash>::CompleteReturnType Get() const                                                   \
		{                                                                                                                         \
			return FDLinkHelper<ReturnType, Hash>::Get(this->Instance);                                                           \
		}                                                                                                                         \
                                                                                                                                  \
		FDLink(const FDLink&) = delete;                                                                                           \
		FDLink(FDLink&&) = delete;                                                                                                \
		FDLink& operator=(const FDLink&) = delete;                                                                                \
		FDLink& operator=(FDLink&&) = delete;                                                                                     \
	};                                                                                                                            \
                                                                                                                                  \
	template <class ReturnType, int32 Hash>                                                                                       \
	struct FDLink<TArray<__Type__>, ReturnType, Hash> : public FDLinkBase<TArray<__Type__>, ReturnType, Hash>                     \
	{                                                                                                                             \
	public:                                                                                                                       \
		FDLink(const char* Name, const char* Path, const char* CharReturnType, const UPsData* InInstance, bool bAbstract = false) \
			: FDLinkBase<TArray<__Type__>, ReturnType, Hash>(Name, Path, CharReturnType, InInstance, bAbstract)                   \
		{                                                                                                                         \
		}                                                                                                                         \
                                                                                                                                  \
		FDLink(const char* Name, const char* CharReturnType, const UPsData* InInstance)                                           \
			: FDLinkBase<TArray<__Type__>, ReturnType, Hash>(Name, CharReturnType, InInstance)                                    \
		{                                                                                                                         \
		}                                                                                                                         \
                                                                                                                                  \
		typename FDLinkHelper<ReturnType, Hash>::CompleteReturnType Get() const                                                   \
		{                                                                                                                         \
			return FDLinkHelper<ReturnType, Hash>::GetAsArray(this->Instance);                                                    \
		}                                                                                                                         \
                                                                                                                                  \
		FDLink(const FDLink&) = delete;                                                                                           \
		FDLink(FDLink&&) = delete;                                                                                                \
		FDLink& operator=(const FDLink&) = delete;                                                                                \
		FDLink& operator=(FDLink&&) = delete;                                                                                     \
	};                                                                                                                            \
	}
