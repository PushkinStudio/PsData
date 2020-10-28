// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Collection/PsDataArrayProxy.h"
#include "Collection/PsDataMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataFunctionLibrary.h"
#include "PsDataHardObjectPtr.h"
#include "PsDataRoot.h"
#include "PsDataStringView.h"
#include "PsDataTraits.h"
#include "Types/PsData_Enum.h"
#include "Types/PsData_FLinearColor.h"
#include "Types/PsData_FName.h"
#include "Types/PsData_FPsDataBigInteger.h"
#include "Types/PsData_FString.h"
#include "Types/PsData_FText.h"
#include "Types/PsData_TSoftClassPtr.h"
#include "Types/PsData_TSoftObjectPtr.h"
#include "Types/PsData_UPsData.h"
#include "Types/PsData_bool.h"
#include "Types/PsData_float.h"
#include "Types/PsData_int32.h"
#include "Types/PsData_int64.h"
#include "Types/PsData_uint8.h"

/***********************************
 * Private macros
 ***********************************/

#define COMMA ,
#define __TOKENPASTE_OneParam(x) #x
#define _TOKENPASTE_OneParam(x) __TOKENPASTE_OneParam(x)
#define __TOKENPASTE_TwoParams(x, y) x##y
#define _TOKENPASTE_TwoParams(x, y) __TOKENPASTE_TwoParams(x, y)

#define _UNIQ(name) _TOKENPASTE_TwoParams(__d##name, __LINE__)

#define _DPROP_DECLARE(__Type__, __Name__)                                                                                                        \
	static_assert(!PsDataTools::TStringView::Equal(#__Name__, "Name"), "Bad property name");                                                      \
                                                                                                                                                  \
protected:                                                                                                                                        \
	struct DPropType##__Name__                                                                                                                    \
		: public PsDataTools::FDataProperty<__Type__>,                                                                                            \
		  public FNoncopyable                                                                                                                     \
	{                                                                                                                                             \
		using PropertyType = __Type__;                                                                                                            \
		static constexpr int32 PropertyHash = PsDataTools::TStringView(#__Name__).GetHash();                                                      \
                                                                                                                                                  \
		static TSharedPtr<FDataField>& StaticField()                                                                                              \
		{                                                                                                                                         \
			static TSharedPtr<FDataField> Field;                                                                                                  \
			return Field;                                                                                                                         \
		}                                                                                                                                         \
                                                                                                                                                  \
		DPropType##__Name__(const char* Name, UPsData* Instance)                                                                                  \
		{                                                                                                                                         \
			PsDataTools::FDataReflection::InitField(Name, PropertyHash, &PsDataTools::GetContext<PropertyType>(), StaticField(), Instance, this); \
		}                                                                                                                                         \
                                                                                                                                                  \
		virtual TSharedPtr<const FDataField> GetField() const override                                                                            \
		{                                                                                                                                         \
			return StaticField();                                                                                                                 \
		}                                                                                                                                         \
	};                                                                                                                                            \
                                                                                                                                                  \
	DPropType##__Name__ __dprop_##__Name__{#__Name__, this};

#define _DPROP_DEPRECATE(__Name__) DEPRECATED(0, "Property " __Name__ " was marked as deprecated")

/***********************************
 * Macro _DPROP_BIND
 ***********************************/

#define _DPROP_BIND(__Name__)                                                   \
	static const FString& Get##__Name__##ChangedEventName()                     \
	{                                                                           \
		return DPropType##__Name__::StaticField()->GetChangedEventName();       \
	}                                                                           \
                                                                                \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const \
	{                                                                           \
		return Bind(Get##__Name__##ChangedEventName(), Delegate);               \
	}

/***********************************
 * Macro _DPROP_BIND_DEPRECATED
 ***********************************/

#define _DPROP_BIND_DEPRECATED(__Name__)                                        \
	_DPROP_DEPRECATE(__Name__)                                                  \
	static const FString& Get##__Name__##ChangedEventName()                     \
	{                                                                           \
		return DPropType##__Name__::StaticField()->GetChangedEventName();       \
	}                                                                           \
                                                                                \
	_DPROP_DEPRECATE(__Name__)                                                  \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const \
	{                                                                           \
		return Bind(Get##__Name__##ChangedEventName(), Delegate);               \
	}

/***********************************
 * Macro DMETA
 ***********************************/

#define DMETA(...)                                                \
protected:                                                        \
	struct _UNIQ(DMetaType)                                       \
		: public FNoncopyable                                     \
	{                                                             \
		_UNIQ(DMetaType)                                          \
		()                                                        \
		{                                                         \
			PsDataTools::FDataReflection::InitMeta(#__VA_ARGS__); \
		}                                                         \
	} _UNIQ(meta)();

/***********************************
 * Macro DPROP
 ***********************************/

#define DPROP(__Type__, __Name__)                                               \
	static_assert(                                                              \
		PsDataTools::TIsContainer<__Type__>::Value,                             \
		"Macro DPROP is available only for non-container types");               \
                                                                                \
	_DPROP_DECLARE(__Type__, __Name__);                                         \
                                                                                \
public:                                                                         \
	typename PsDataTools::TConstRef<__Type__, true>::Type Get##__Name__() const \
	{                                                                           \
		return __dprop_##__Name__.Get();                                        \
	}                                                                           \
                                                                                \
	typename PsDataTools::TConstRef<__Type__>::Type Get##__Name__()             \
	{                                                                           \
		return __dprop_##__Name__.Get();                                        \
	}                                                                           \
                                                                                \
	void Set##__Name__(PsDataTools::TConstRef<__Type__>::Type Value)            \
	{                                                                           \
		__dprop_##__Name__.Set(Value, this);                                    \
	}                                                                           \
                                                                                \
	_DPROP_BIND(__Name__);

/***********************************
 * Macro DPROP_DEPRECATED
 ***********************************/

#define DPROP_DEPRECATED(__Type__, __Name__)                                    \
	static_assert(                                                              \
		PsDataTools::TIsContainer<__Type__>::Value,                             \
		"Macro DPROP_DEPRECATED is available only for non-container types");    \
                                                                                \
	DMETA(Deprecated);                                                          \
	_DPROP_DECLARE(__Type__, __Name__);                                         \
                                                                                \
public:                                                                         \
	_DPROP_DEPRECATE(__Name__)                                                  \
	typename PsDataTools::TConstRef<__Type__, true>::Type Get##__Name__() const \
	{                                                                           \
		return __dprop_##__Name__.Get();                                        \
	}                                                                           \
                                                                                \
	_DPROP_DEPRECATE(__Name__)                                                  \
	typename PsDataTools::TConstRef<__Type__>::Type Get##__Name__()             \
	{                                                                           \
		return __dprop_##__Name__.Get();                                        \
	}                                                                           \
                                                                                \
	_DPROP_DEPRECATE(__Name__)                                                  \
	void Set##__Name__(PsDataTools::TConstRef<__Type__>::Type Value)            \
	{                                                                           \
		__dprop_##__Name__.Set(Value, this);                                    \
	}                                                                           \
                                                                                \
	_DPROP_BIND_DEPRECATED(__Name__);

/***********************************
 * Macro DPROP_CONST
 ***********************************/

#define DPROP_CONST(__Type__, __Name__, __Friend__)                             \
	static_assert(                                                              \
		PsDataTools::TIsContainer<__Type__>::Value,                             \
		"Macro DPROP_CONST is available only for non-container types");         \
                                                                                \
	DMETA(ReadOnly, Strict);                                                    \
	_DPROP_DECLARE(__Type__, __Name__);                                         \
                                                                                \
public:                                                                         \
	typename PsDataTools::TConstRef<__Type__, true>::Type Get##__Name__() const \
	{                                                                           \
		return __dprop_##__Name__.Get();                                        \
	}                                                                           \
                                                                                \
	struct FMutable_##__Name__                                                  \
	{                                                                           \
		FMutable_##__Name__(DPropType##__Name__* InProperty)                    \
			: Property(InProperty)                                              \
		{                                                                       \
		}                                                                       \
                                                                                \
	private:                                                                    \
		friend class __Friend__;                                                \
                                                                                \
		DPropType##__Name__* Property;                                          \
                                                                                \
		typename PsDataTools::TConstRef<__Type__>::Type operator()() const      \
		{                                                                       \
			return Property->Get();                                             \
		}                                                                       \
	} const GetMutable##__Name__{&__dprop_##__Name__};

/***********************************
 * Macro DPROP_CONST_DEPRECATED
 ***********************************/

#define DPROP_CONST_DEPRECATED(__Type__, __Name__, __Friend__)                     \
	static_assert(                                                                 \
		PsDataTools::TIsContainer<__Type__>::Value,                                \
		"Macro DPROP_CONST_DEPRECATED is available only for non-container types"); \
                                                                                   \
	DMETA(ReadOnly, Strict, Deprecated);                                           \
	_DPROP_DECLARE(__Type__, __Name__);                                            \
                                                                                   \
public:                                                                            \
	_DPROP_DEPRECATE(__Name__)                                                     \
	typename PsDataTools::TConstRef<__Type__, true>::Type Get##__Name__() const    \
	{                                                                              \
		return __dprop_##__Name__.Get();                                           \
	}                                                                              \
                                                                                   \
	struct FMutable_##__Name__                                                     \
	{                                                                              \
		FMutable_##__Name__(DPropType##__Name__* InProperty)                       \
			: Property(InProperty)                                                 \
		{                                                                          \
		}                                                                          \
                                                                                   \
	private:                                                                       \
		friend class __Friend__;                                                   \
                                                                                   \
		DPropType##__Name__* Property;                                             \
                                                                                   \
		_DPROP_DEPRECATE(__Name__)                                                 \
		typename PsDataTools::TConstRef<__Type__>::Type operator()() const         \
		{                                                                          \
			return Property->Get();                                                \
		}                                                                          \
	} const GetMutable##__Name__{&__dprop_##__Name__};

/***********************************
 * Macro DARRAY
 ***********************************/

#define DARRAY(__Type__, __Name__)                                                                                             \
	static_assert(                                                                                                             \
		PsDataTools::TIsContainer<TArray<__Type__>>::Array,                                                                    \
		"Macro DARRAY is available only for TArray<ContentType> type");                                                        \
                                                                                                                               \
	_DPROP_DECLARE(TArray<__Type__>, __Name__);                                                                                \
                                                                                                                               \
public:                                                                                                                        \
	FPsDataConstArrayProxy<__Type__> Get##__Name__() const                                                                     \
	{                                                                                                                          \
		return FPsDataConstArrayProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), &__dprop_##__Name__); \
	}                                                                                                                          \
                                                                                                                               \
	FPsDataArrayProxy<__Type__> Get##__Name__()                                                                                \
	{                                                                                                                          \
		return FPsDataArrayProxy<__Type__>(this, &__dprop_##__Name__);                                                         \
	}                                                                                                                          \
                                                                                                                               \
	_DPROP_BIND(__Name__);

/***********************************
 * Macro DARRAY_DEPRECATED
 ***********************************/

#define DARRAY_DEPRECATED(__Type__, __Name__)                                                                                  \
	static_assert(                                                                                                             \
		PsDataTools::TIsContainer<TArray<__Type__>>::Array,                                                                    \
		"Macro DARRAY_DEPRECATED is available only for TArray<ContentType> type");                                             \
                                                                                                                               \
	DMETA(Deprecated)                                                                                                          \
	_DPROP_DECLARE(TArray<__Type__>, __Name__)                                                                                 \
                                                                                                                               \
public:                                                                                                                        \
	_DPROP_DEPRECATE(__Name__)                                                                                                 \
	FPsDataConstArrayProxy<__Type__> Get##__Name__() const                                                                     \
	{                                                                                                                          \
		return FPsDataConstArrayProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), &__dprop_##__Name__); \
	}                                                                                                                          \
                                                                                                                               \
	_DPROP_DEPRECATE(__Name__)                                                                                                 \
	FPsDataArrayProxy<__Type__> Get##__Name__()                                                                                \
	{                                                                                                                          \
		return FPsDataArrayProxy<__Type__>(this, &__dprop_##__Name__);                                                         \
	}                                                                                                                          \
                                                                                                                               \
	_DPROP_BIND_DEPRECATED(__Name__);

/***********************************
 * Macro DMAP
 ***********************************/

#define DMAP(__Type__, __Name__)                                                                                             \
	static_assert(                                                                                                           \
		PsDataTools::TIsContainer<TMap<FString, __Type__>>::Map,                                                             \
		"Macro DMAP is available only for TMap<FString, ContentType> type");                                                 \
                                                                                                                             \
	_DPROP_DECLARE(TMap<FString COMMA __Type__>, __Name__);                                                                  \
                                                                                                                             \
public:                                                                                                                      \
	FPsDataConstMapProxy<__Type__> Get##__Name__() const                                                                     \
	{                                                                                                                        \
		return FPsDataConstMapProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), &__dprop_##__Name__); \
	}                                                                                                                        \
                                                                                                                             \
	FPsDataMapProxy<__Type__> Get##__Name__()                                                                                \
	{                                                                                                                        \
		return FPsDataMapProxy<__Type__>(this, &__dprop_##__Name__);                                                         \
	}                                                                                                                        \
                                                                                                                             \
	_DPROP_BIND(__Name__);

/***********************************
 * Macro DMAP_DEPRECATED
 ***********************************/

#define DMAP_DEPRECATED(__Type__, __Name__)                                                                                  \
	static_assert(                                                                                                           \
		PsDataTools::TIsContainer<TMap<FString, __Type__>>::Map,                                                             \
		"Macro DMAP_DEPRECATED is available only for TMap<FString, ContentType> type");                                      \
                                                                                                                             \
	DMETA(Deprecated);                                                                                                       \
	_DPROP_DECLARE(TMap<FString COMMA __Type__>, __Name__);                                                                  \
                                                                                                                             \
public:                                                                                                                      \
	_DPROP_DEPRECATE(__Name__)                                                                                               \
	FPsDataConstMapProxy<__Type__> Get##__Name__() const                                                                     \
	{                                                                                                                        \
		return FPsDataConstMapProxy<__Type__>(const_cast<UPsData*>(static_cast<const UPsData*>(this)), &__dprop_##__Name__); \
	}                                                                                                                        \
                                                                                                                             \
	_DPROP_DEPRECATE(__Name__)                                                                                               \
	FPsDataMapProxy<__Type__> Get##__Name__()                                                                                \
	{                                                                                                                        \
		return FPsDataMapProxy<__Type__>(this, &__dprop_##__Name__);                                                         \
	}                                                                                                                        \
                                                                                                                             \
	_DPROP_BIND_DEPRECATED(__Name__);

/***********************************
 * Macro DLINK
 ***********************************/

#define DLINK(__ReturnType__, __Name__, __Path__) \
public:                                           \
	const PsDataTools::FDLink<DPropType##__Name__::PropertyType, __ReturnType__, DPropType##__Name__::PropertyHash> LinkBy##__Name__{#__Name__, #__Path__, #__ReturnType__, this};

/***********************************
 * Macro DLINK_DEPRECATED
 ***********************************/

#define DLINK_DEPRECATED(__ReturnType__, __Name__, __Path__) \
public:                                                      \
	_DPROP_DEPRECATE(__Name__)                               \
	const PsDataTools::FDLink<DPropType##__Name__::PropertyType, __ReturnType__, DPropType##__Name__::PropertyHash> LinkBy##__Name__{#__Name__, #__Path__, #__ReturnType__, this};

/***********************************
 * Macro DLINK_ABSTRACT
 ***********************************/

#define DLINK_ABSTRACT(__ReturnType__, __Name__) \
public:                                          \
	const PsDataTools::FDLink<DPropType##__Name__::PropertyType, __ReturnType__, DPropType##__Name__::PropertyHash> LinkByAbstract##__Name__{#__Name__, #__ReturnType__, this};

/***********************************
 * Macro DLINK_ABSTRACT_DEPRECATED
 ***********************************/

#define DLINK_ABSTRACT_DEPRECATED(__ReturnType__, __Name__) \
public:                                                     \
	_DPROP_DEPRECATE(__Name__)                              \
	const PsDataTools::FDLink<DPropType##__Name__::PropertyType, __ReturnType__, DPropType##__Name__::PropertyHash> LinkByAbstract##__Name__{#__Name__, #__ReturnType__, this};

/***********************************
 * Macro DEFERRED_EVENT_PROCESSING
 ***********************************/

#define DEFERRED_EVENT_PROCESSING() \
	FPsDataEventScopeGuard EventScopeGuard();