// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Collection/PsDataArrayProxy.h"
#include "Collection/PsDataMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataFunctionLibrary.h"
#include "Serialize/PsDataSerialization.h"

#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"

/***********************************
 * Macro DMETA
 ***********************************/

#define DMETA(...) \
private:           \
	FDataReflectionTools::FMeta _UNIC(meta) = FDataReflectionTools::FMeta(#__VA_ARGS__);

/***********************************
 * Macro DPROP
 ***********************************/

#define DPROP(__Class__, __Type__, __Name__)                                                                           \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP is available only for non-container types");                                                      \
                                                                                                                       \
	_REFLECT(__Class__, __Type__, __Name__);                                                                           \
                                                                                                                       \
public:                                                                                                                \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	typename FDataReflectionTools::TConstRef<__Type__>::Type Get##__Name__()                                           \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(this, _UNIC(hash), Output);                                                    \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	void Set##__Name__(FDataReflectionTools::TConstRef<__Type__>::Type Value)                                          \
	{                                                                                                                  \
		FDataReflectionTools::SetByHash<__Type__>(this, _UNIC(hash), Value);                                           \
	}                                                                                                                  \
                                                                                                                       \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                     \
	{                                                                                                                  \
		Bind(_UNIC(hash), Delegate);                                                                                   \
	}

/***********************************
 * Macro DPROP_DEPRECATED
 ***********************************/

#define DPROP_DEPRECATED(__Class__, __Type__, __Name__)                                                                \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP_DEPRECATED is available only for non-container types");                                           \
                                                                                                                       \
	DMETA(Deprecated);                                                                                                 \
	_REFLECT(__Class__, __Type__, __Name__);                                                                           \
                                                                                                                       \
public:                                                                                                                \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	typename FDataReflectionTools::TConstRef<__Type__>::Type Get##__Name__()                                           \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(this, _UNIC(hash), Output);                                                    \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	void Set##__Name__(FDataReflectionTools::TConstRef<__Type__>::Type Value)                                          \
	{                                                                                                                  \
		FDataReflectionTools::SetByHash<__Type__>(this, _UNIC(hash), Value);                                           \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                     \
	{                                                                                                                  \
		Bind(_UNIC(hash), Delegate);                                                                                   \
	}

/***********************************
 * Macro DPROP_CONST
 ***********************************/

#define DPROP_CONST(__Class__, __Type__, __Name__, __Friend__)                                                         \
	static_assert(                                                                                                     \
		std::is_base_of<UPsData, FDataReflectionTools::TRemovePointer<__Type__>::Type>::value,                         \
		"Macro DPROP_CONST is available only for UPsData");                                                            \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP_CONST is available only for non-container types");                                                \
                                                                                                                       \
	DMETA(ReadOnly, Strict);                                                                                           \
	_REFLECT(__Class__, __Type__, __Name__);                                                                           \
                                                                                                                       \
public:                                                                                                                \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	struct FMutable_##__Name__                                                                                         \
	{                                                                                                                  \
	private:                                                                                                           \
		friend class __Friend__;                                                                                       \
		friend class __Class__;                                                                                        \
                                                                                                                       \
		UPsData* This;                                                                                                 \
                                                                                                                       \
		FMutable_##__Name__(UPsData* Instance)                                                                         \
			: This(Instance)                                                                                           \
		{                                                                                                              \
		}                                                                                                              \
                                                                                                                       \
		typename FDataReflectionTools::TConstRef<__Type__>::Type operator()() const                                    \
		{                                                                                                              \
			__Type__* Output = nullptr;                                                                                \
			FDataReflectionTools::GetByHash(This, _UNIC(hash), Output);                                                \
			return *Output;                                                                                            \
		}                                                                                                              \
	} const GetMutable##__Name__ = FMutable_##__Name__(this);                                                          \
                                                                                                                       \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                     \
	{                                                                                                                  \
		Bind(_UNIC(hash), Delegate);                                                                                   \
	}

/***********************************
 * Macro DPROP_CONST_DEPRECATED
 ***********************************/

#define DPROP_CONST_DEPRECATED(__Class__, __Type__, __Name__, __Friend__)                                              \
	static_assert(                                                                                                     \
		std::is_base_of<UPsData, FDataReflectionTools::TRemovePointer<__Type__>::Type>::value,                         \
		"Macro DPROP_CONST_DEPRECATED is available only for UPsData");                                                 \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<__Type__>::Value,                                                           \
		"Macro DPROP_CONST_DEPRECATED is available only for non-container types");                                     \
                                                                                                                       \
	DMETA(ReadOnly, Strict, Deprecated);                                                                               \
	_REFLECT(__Class__, __Type__, __Name__);                                                                           \
                                                                                                                       \
public:                                                                                                                \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	typename FDataReflectionTools::TConstRef<__Type__, true>::Type Get##__Name__() const                               \
	{                                                                                                                  \
		__Type__* Output = nullptr;                                                                                    \
		FDataReflectionTools::GetByHash(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash), Output); \
		return *Output;                                                                                                \
	}                                                                                                                  \
                                                                                                                       \
	struct FMutable_##__Name__                                                                                         \
	{                                                                                                                  \
	private:                                                                                                           \
		friend class __Friend__;                                                                                       \
		friend class __Class__;                                                                                        \
                                                                                                                       \
		UPsData* This;                                                                                                 \
                                                                                                                       \
		FMutable_##__Name__(UPsData* Instance)                                                                         \
			: This(Instance)                                                                                           \
		{                                                                                                              \
		}                                                                                                              \
                                                                                                                       \
		DEPRECATED(0, "Property was marked as deprecated")                                                             \
		typename FDataReflectionTools::TConstRef<__Type__>::Type operator()() const                                    \
		{                                                                                                              \
			__Type__* Output = nullptr;                                                                                \
			FDataReflectionTools::GetByHash(This, _UNIC(hash), Output);                                                \
			return *Output;                                                                                            \
		}                                                                                                              \
	} const GetMutable##__Name__ = FMutable_##__Name__(this);                                                          \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                     \
	{                                                                                                                  \
		Bind(_UNIC(hash), Delegate);                                                                                   \
	}

/***********************************
 * Macro DARRAY
 ***********************************/

#define DARRAY(__Class__, __Type__, __Name__)                                                                          \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<TArray<__Type__>>::Array,                                                   \
		"Macro DARRAY is available only for TArray<ContentType> type");                                                \
                                                                                                                       \
	_REFLECT(__Class__, TArray<__Type__>, __Name__);                                                                   \
                                                                                                                       \
public:                                                                                                                \
	FPsDataConstArrayProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                  \
		return FPsDataConstArrayProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash)); \
	}                                                                                                                  \
                                                                                                                       \
	FPsDataArrayProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                  \
		return FPsDataArrayProxy<__Type__>(this, _UNIC(hash));                                                         \
	}                                                                                                                  \
                                                                                                                       \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                     \
	{                                                                                                                  \
		Bind(_UNIC(hash), Delegate);                                                                                   \
	}

/***********************************
 * Macro DARRAY_DEPRECATED
 ***********************************/

#define DARRAY_DEPRECATED(__Class__, __Type__, __Name__)                                                               \
	static_assert(                                                                                                     \
		FDataReflectionTools::TIsContainer<TArray<__Type__>>::Array,                                                   \
		"Macro DARRAY_DEPRECATED is available only for TArray<ContentType> type");                                     \
                                                                                                                       \
	DMETA(Deprecated)                                                                                                  \
	_REFLECT(__Class__, TArray<__Type__>, __Name__)                                                                    \
                                                                                                                       \
public:                                                                                                                \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataConstArrayProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                  \
		return FPsDataConstArrayProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash)); \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	FPsDataArrayProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                  \
		return FPsDataArrayProxy<__Type__>(this, _UNIC(hash));                                                         \
	}                                                                                                                  \
                                                                                                                       \
	DEPRECATED(0, "Property was marked as deprecated")                                                                 \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                     \
	{                                                                                                                  \
		Bind(_UNIC(hash), Delegate);                                                                                   \
	}

/***********************************
 * Macro DMAP
 ***********************************/

#define DMAP(__Class__, __Type__, __Name__)                                                                          \
	static_assert(                                                                                                   \
		FDataReflectionTools::TIsContainer<TMap<FString, __Type__>>::Map,                                            \
		"Macro DMAP is available only for TMap<FString, ContentType> type");                                         \
                                                                                                                     \
	_REFLECT(__Class__, TMap<FString COMMA __Type__>, __Name__);                                                     \
                                                                                                                     \
public:                                                                                                              \
	FPsDataConstMapProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                \
		return FPsDataConstMapProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash)); \
	}                                                                                                                \
                                                                                                                     \
	FPsDataMapProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                \
		return FPsDataMapProxy<__Type__>(this, _UNIC(hash));                                                         \
	}                                                                                                                \
                                                                                                                     \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                   \
	{                                                                                                                \
		Bind(_UNIC(hash), Delegate);                                                                                 \
	}

/***********************************
 * Macro DMAP_DEPRECATED
 ***********************************/

#define DMAP_DEPRECATED(__Class__, __Type__, __Name__)                                                               \
	static_assert(                                                                                                   \
		FDataReflectionTools::TIsContainer<TMap<FString, __Type__>>::Map,                                            \
		"Macro DMAP_DEPRECATED is available only for TMap<FString, ContentType> type");                              \
                                                                                                                     \
	DMETA(Deprecated);                                                                                               \
	_REFLECT(__Class__, TMap<FString COMMA __Type__>, __Name__);                                                     \
                                                                                                                     \
public:                                                                                                              \
	DEPRECATED(0, "Property was marked as deprecated")                                                               \
	FPsDataConstMapProxy<__Type__> Get##__Name__() const                                                             \
	{                                                                                                                \
		return FPsDataConstMapProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), _UNIC(hash)); \
	}                                                                                                                \
                                                                                                                     \
	DEPRECATED(0, "Property was marked as deprecated")                                                               \
	FPsDataMapProxy<__Type__> Get##__Name__()                                                                        \
	{                                                                                                                \
		return FPsDataMapProxy<__Type__>(this, _UNIC(hash));                                                         \
	}                                                                                                                \
                                                                                                                     \
	DEPRECATED(0, "Property was marked as deprecated")                                                               \
	void Bind_##__Name__##Changed(const FPsDataDelegate& Delegate)                                                   \
	{                                                                                                                \
		Bind(_UNIC(hash), Delegate);                                                                                 \
	}

/***********************************
 * Macro DPROP_LINK
 ***********************************/

#define DPROP_LINK(__Class__, __Type__, __Name__, __Path__, __ReturnType__)                                                                                          \
	static_assert(                                                                                                                                                   \
		std::is_base_of<FString, __Type__>::value,                                                                                                                   \
		"Only FString property can be linked");                                                                                                                      \
                                                                                                                                                                     \
	DMETA(Link = __Type__::__Path__::__ReturnType__);                                                                                                                \
	DPROP(__Class__, __Type__, __Name__);                                                                                                                            \
                                                                                                                                                                     \
public:                                                                                                                                                              \
	typename FDataReflectionTools::TConstRef<__ReturnType__*, true>::Type GetLinkBy##__Name__() const                                                                \
	{                                                                                                                                                                \
		static const FString Name(#__Name__);                                                                                                                        \
		static const FString Path(#__Path__);                                                                                                                        \
		return FDataReflectionTools::FPsDataCastHelper<const __ReturnType__>::Cast(UPsDataFunctionLibrary::GetDataByPath(const_cast<__Class__*>(this), Name, Path)); \
	}

/***********************************
 * Macro DPROP_LINK_DEPRECATED
 ***********************************/

#define DPROP_LINK_DEPRECATED(__Class__, __Type__, __Name__, __Path__, __ReturnType__)                                                                               \
	static_assert(                                                                                                                                                   \
		std::is_base_of<FString, __Type__>::value,                                                                                                                   \
		"Only FString property can be linked");                                                                                                                      \
                                                                                                                                                                     \
	DMETA(Link = __Type__::__Path__::__ReturnType__);                                                                                                                \
	DPROP_DEPRECATED(__Class__, __Type__, __Name__);                                                                                                                 \
                                                                                                                                                                     \
public:                                                                                                                                                              \
	DEPRECATED(0, "Property was marked as deprecated")                                                                                                               \
	typename FDataReflectionTools::TConstRef<__ReturnType__*, true>::Type GetLinkBy##__Name__() const                                                                \
	{                                                                                                                                                                \
		static const FString Name(#__Name__);                                                                                                                        \
		static const FString Path(#__Path__);                                                                                                                        \
		return FDataReflectionTools::FPsDataCastHelper<const __ReturnType__>::Cast(UPsDataFunctionLibrary::GetDataByPath(const_cast<__Class__*>(this), Name, Path)); \
	}

/***********************************
 * Macro DARRAY_LINK
 ***********************************/

#define DARRAY_LINK(__Class__, __Type__, __Name__, __Path__, __ReturnType__)                                                                                              \
	static_assert(                                                                                                                                                        \
		std::is_base_of<FString, __Type__>::value,                                                                                                                        \
		"Only FString property can be linked");                                                                                                                           \
                                                                                                                                                                          \
	DMETA(Link = __Type__::__Path__::__ReturnType__);                                                                                                                     \
	DARRAY(__Class__, __Type__, __Name__);                                                                                                                                \
                                                                                                                                                                          \
public:                                                                                                                                                                   \
	TArray<FDataReflectionTools::TConstRef<__ReturnType__*, true>::Type> GetLinkBy##__Name__() const                                                                      \
	{                                                                                                                                                                     \
		static const FString Name(#__Name__);                                                                                                                             \
		static const FString Path(#__Path__);                                                                                                                             \
		return FDataReflectionTools::FPsDataCastHelper<const __ReturnType__>::Cast(UPsDataFunctionLibrary::GetDataArrayByPath(const_cast<__Class__*>(this), Name, Path)); \
	}

/***********************************
 * Macro DARRAY_LINK_DEPRECATED
 ***********************************/

#define DARRAY_LINK_DEPRECATED(__Class__, __Type__, __Name__, __Path__, __ReturnType__)                                                                                   \
	static_assert(                                                                                                                                                        \
		std::is_base_of<FString, __Type__>::value,                                                                                                                        \
		"Only FString property can be linked");                                                                                                                           \
                                                                                                                                                                          \
	DMETA(Link = __Type__::__Path__::__ReturnType__);                                                                                                                     \
	DARRAY_DEPRECATED(__Class__, __Type__, __Name__);                                                                                                                     \
                                                                                                                                                                          \
public:                                                                                                                                                                   \
	DEPRECATED(0, "Property was marked as deprecated")                                                                                                                    \
	TArray<FDataReflectionTools::TConstRef<__ReturnType__*, true>::Type> GetLinkBy##__Name__() const                                                                      \
	{                                                                                                                                                                     \
		static const FString Name(#__Name__);                                                                                                                             \
		static const FString Path(#__Path__);                                                                                                                             \
		return FDataReflectionTools::FPsDataCastHelper<const __ReturnType__>::Cast(UPsDataFunctionLibrary::GetDataArrayByPath(const_cast<__Class__*>(this), Name, Path)); \
	}

/***********************************
 * Macro DESCRIBE_ENUM
 ***********************************/

#define DESCRIBE_ENUM(__Type__)                                               \
	template <>                                                               \
	struct FDataMemory<__Type__> : public FEnumDataMemory<__Type__>           \
	{                                                                         \
		virtual ~FDataMemory() {}                                             \
	};                                                                        \
                                                                              \
	template <>                                                               \
	struct FDataTypeContext<__Type__> : public FEnumDataTypeContext<__Type__> \
	{                                                                         \
		virtual ~FDataTypeContext() {}                                        \
	};
