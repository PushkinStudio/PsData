// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "Collection/PsDataArrayProxy.h"
#include "Collection/PsDataMapProxy.h"
#include "PsData.h"
#include "PsDataCore.h"
#include "PsDataEvent.h"
#include "PsDataFunctionLibrary.h"
#include "PsDataHardObjectPtr.h"
#include "PsDataLink.h"
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

#define PSDATA_OLD_MACRO 1

namespace PsDataTools
{

/***********************************
 * TDMeta
 ***********************************/

template <class OwnerClass, int32 Hash>
struct TDMeta : public FDataNoncopyable, public FDataNonmovable
{
	TDMeta(const char* MetaString)
	{
		static const auto StaticInit = FDataReflection::InitMeta(MetaString);
	}
};

/***********************************
 * TDBaseProp
 ***********************************/

template <class OwnerClass, int32 Hash, typename FieldType, typename... Other>
struct TDBaseProp : public FDataNoncopyable, public FDataNonmovable
{
	TDBaseProp(OwnerClass* InPropertyOwner)
		: PropertyOwner(InPropertyOwner)
	{
	}

protected:
	static FieldType*& ProtectedStaticField()
	{
		static FieldType* Field = nullptr;
		return Field;
	}

	OwnerClass* GetPropertyOwner() const
	{
		return PropertyOwner;
	}

	OwnerClass* GetPropertyOwner()
	{
		return PropertyOwner;
	}

public:
	static const FieldType* StaticField()
	{
		return ProtectedStaticField();
	}

private:
	mutable OwnerClass* PropertyOwner;
};

/***********************************
 * TDProp
 ***********************************/

template <typename T, class OwnerClass, int32 Hash>
struct TDProp : public TDBaseProp<OwnerClass, Hash, FDataField>, protected TDataProperty<T>
{
private:
	virtual const FDataField* GetField() const override
	{
		return this->StaticField();
	}

	virtual UPsData* GetOwner() override
	{
		return this->GetPropertyOwner();
	}

	virtual UPsData* GetOwner() const override
	{
		return this->GetPropertyOwner();
	}

protected:
	template <bool bConst>
	using TArrayType = FPsDataBaseArrayProxy<typename TIsContainer<T>::Type, bConst>;
	template <bool bConst>
	using TMapType = FPsDataBaseMapProxy<typename TIsContainer<T>::Type, bConst>;

public:
	using Type = T;
	template <bool bConst>
	using TValueType = TConstRefType<T, bConst>;
	template <bool bConst>
	using TReturnType = typename TSelector<
		TValueType<bConst>,
		typename TSelector<
			TArrayType<bConst>,
			typename TSelector<
				TMapType<bConst>,
				void,
				TIsContainer<T>::Map>::Value,
			TIsContainer<T>::Array>::Value,
		TIsContainer<T>::Value>::Value;

	TDProp(const char* Name, OwnerClass* InOwner)
		: TDBaseProp<OwnerClass, Hash, FDataField>(InOwner)
	{
		static const auto StaticInit = FDataReflection::InitProperty(StaticClass<OwnerClass>(), Name, &GetContext<T>(), this->ProtectedStaticField());
		const auto Index = FPsDataFriend::GetProperties(InOwner).Add(this);

#if !UE_BUILD_SHIPPING
		check(Index == this->ProtectedStaticField()->Index);
#endif
	}

	virtual ~TDProp() override
	{
	}

	void Set(TValueType<false> InValue)
	{
		this->SetValue(InValue);
	}

	TReturnType<false> Get()
	{
		return PrivateGet();
	}

	TReturnType<true> Get() const
	{
		return PrivateGet();
	}

	FPsDataBind Bind(const FPsDataDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return this->GetPropertyOwner()->Bind(this->ProtectedStaticField()->GetChangedEventName(), Delegate, Flags);
	}

	TDProp& operator=(TValueType<false> InValue)
	{
		Set(InValue);
		return *this;
	}

	TReturnType<false> operator*()
	{
		return PrivateGet();
	}

	TReturnType<true> operator*() const
	{
		return PrivateGet();
	}

	TReturnType<false> operator->()
	{
		return PrivateGet();
	}

	TReturnType<true> operator->() const
	{
		return PrivateGet();
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	TDProp& operator+=(T InValue)
	{
		Set(PrivateGet() + InValue);
		return *this;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	TDProp& operator-=(T InValue)
	{
		Set(PrivateGet() - InValue);
		return *this;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	TDProp& operator*=(T InValue)
	{
		Set(PrivateGet() * InValue);
		return *this;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	TDProp& operator/=(T InValue)
	{
		Set(PrivateGet() / InValue);
		return *this;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	TDProp& operator++()
	{
		Set(PrivateGet() + 1);
		return *this;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator++(int)
	{
		auto Result = PrivateGet();
		Set(Result + 1);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	TDProp& operator--()
	{
		Set(PrivateGet() - 1);
		return *this;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator--(int)
	{
		auto Result = PrivateGet();
		Set(Result - 1);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator+(T InValue) const
	{
		return PrivateGet() + InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator-(T InValue) const
	{
		return PrivateGet() - InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator*(T InValue) const
	{
		return PrivateGet() * InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator/(T InValue) const
	{
		return PrivateGet() / InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator+() const
	{
		return PrivateGet();
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator-() const
	{
		return -PrivateGet();
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator%(T InValue) const
	{
		return PrivateGet() % InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator>(T InValue) const
	{
		return PrivateGet() > InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator>=(T InValue) const
	{
		return PrivateGet() >= InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator<(T InValue) const
	{
		return PrivateGet() < InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator<=(T InValue) const
	{
		return PrivateGet() <= InValue;
	}

	template <typename Other, typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Value>::Type>
	bool operator==(const Other& InValue) const
	{
		return PrivateGet() == InValue;
	}

	template <typename Other, typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Value>::Type>
	bool operator!=(const Other& InValue) const
	{
		return PrivateGet() != InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Value>::Type>
	operator TReturnType<false>()
	{
		return PrivateGet();
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Value>::Type>
	operator TReturnType<true>() const
	{
		return PrivateGet();
	}

private:
	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Value>::Type>
	TValueType<false> PrivateGet()
	{
		return this->GetValue();
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Value>::Type>
	TValueType<true> PrivateGet() const
	{
		return this->GetValue();
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Array>::Type>
	TArrayType<false> PrivateGet()
	{
		return {this};
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Array>::Type>
	TArrayType<true> PrivateGet() const
	{
		return {this};
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Map>::Type>
	TMapType<false> PrivateGet()
	{
		return {this};
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsContainer<K>::Map>::Type>
	TMapType<true> PrivateGet() const
	{
		return {this};
	}
};

/***********************************
 * TDPropConst
 ***********************************/

template <typename T, class OwnerClass, int32 Hash, typename Friend>
struct TDPropConst : public FDataNoncopyable, public FDataNonmovable
{
	using DPropType = TDProp<T, OwnerClass, Hash>;
	using Type = typename DPropType::Type;
	template <bool bConst>
	using TValueType = typename DPropType::template TValueType<bConst>;
	template <bool bConst>
	using TReturnType = typename DPropType::template TReturnType<bConst>;

	TDPropConst(const char* Name, OwnerClass* InOwner)
		: Mutable(Name, InOwner)
	{
	}

	TReturnType<true> operator*()
	{
		return Get();
	}

	TReturnType<true> operator*() const
	{
		return Get();
	}

	TReturnType<true> operator->()
	{
		return Get();
	}

	TReturnType<true> operator->() const
	{
		return Get();
	}

	TReturnType<true> Get()
	{
		return Mutable.Get();
	}

	TReturnType<true> Get() const
	{
		return Mutable.Get();
	}

private:
	friend Friend;
	DPropType Mutable;
};

/***********************************
 * TDLink
 ***********************************/

template <typename KeyType, typename ValueType, class OwnerClass, int32 Hash, bool bAbstract>
struct TDLink : public TDBaseProp<OwnerClass, Hash, FDataLink, std::integral_constant<bool, bAbstract>>, protected TDataLinkProperty<KeyType, ValueType>
{
private:
	using Types = typename TDataLinkProperty<KeyType, ValueType>::Types;
	using FReturnType = typename Types::LinkValueType;

	virtual const FDataLink* GetLink() const override
	{
		return this->StaticField();
	}

	virtual UPsData* GetOwner() override
	{
		return this->GetPropertyOwner();
	}

	virtual UPsData* GetOwner() const override
	{
		return this->GetPropertyOwner();
	}

public:
	TDLink(const char* Name, OwnerClass* InOwner, const FLinkPathFunction& PathFunction)
		: TDBaseProp<OwnerClass, Hash, FDataLink, std::integral_constant<bool, bAbstract>>(InOwner)
	{
		static const auto StaticInit = FDataReflection::InitLinkProperty(StaticClass<OwnerClass>(), Name, bAbstract, &GetContext<typename Types::LinkValueContextType>(), PathFunction, this->ProtectedStaticField());
		const auto Index = FPsDataFriend::GetLinks(InOwner).Add(this);

#if !UE_BUILD_SHIPPING
		check(Index == this->ProtectedStaticField()->Index);
#endif

		this->Construct();
	}

	template <bool bOtherAbstract = bAbstract,
		typename = typename TEnableIf<!bOtherAbstract>::Type>
	TDLink(const char* Name, OwnerClass* InOwner, const char* Path)
		: TDLink(Name, InOwner, [Path](UPsData* Data, FString& OutPath) {
			OutPath = Path;
		})
	{
	}

	template <bool bOtherAbstract = bAbstract,
		typename = typename TEnableIf<bOtherAbstract>::Type>
	TDLink(const char* Name, OwnerClass* InOwner)
		: TDLink(Name, InOwner, nullptr)
	{
	}

	virtual ~TDLink() override
	{
		this->Destruct();
	}

	FReturnType Get() const
	{
		return this->GetValue();
	}

	bool IsEmpty() const
	{
		return this->IsEmptyKey();
	}

	FReturnType operator*() const
	{
		return Get();
	}

	operator FReturnType() const
	{
		return Get();
	}
};

} // namespace PsDataTools

/***********************************
 * Private macros
 ***********************************/

#define COMMA ,
#define __TOKENPASTE(x) #x
#define _TOKENPASTE(x) __TOKENPASTE(x)
#define __TOKENPASTE2(x, y) x##y
#define _TOKENPASTE2(x, y) __TOKENPASTE2(x, y)
#define __TOKENPASTE3(x, y, z) x##y##z
#define _TOKENPASTE3(x, y, z) __TOKENPASTE3(x, y, z)

/***********************************
 * Macro DMETA
 ***********************************/

#define DMETA(...) \
private:           \
	PsDataTools::TDMeta<ThisClass, __LINE__> _TOKENPASTE2(DMeta_, __LINE__){#__VA_ARGS__};

/***********************************
 * Macro DPROP
 ***********************************/

#define DPROP(__Type__, __Name__)                                                                                                 \
protected:                                                                                                                        \
	using DPropType_##__Name__ = PsDataTools::TDProp<__Type__, ThisClass, PsDataTools::FDataStringViewChar(#__Name__).GetHash()>; \
                                                                                                                                  \
public:                                                                                                                           \
	DPropType_##__Name__ __Name__{#__Name__, this};

/***********************************
 * Macro DPROP_CONST
 ***********************************/

#define DPROP_CONST(__Type__, __Name__, __Friend__)                                                                                                \
protected:                                                                                                                                         \
	DMETA(ReadOnly, Strict);                                                                                                                       \
	using DPropType_##__Name__ = PsDataTools::TDPropConst<__Type__, ThisClass, PsDataTools::FDataStringViewChar(#__Name__).GetHash(), __Friend__>; \
                                                                                                                                                   \
public:                                                                                                                                            \
	DPropType_##__Name__ __Name__{#__Name__, this};

/***********************************
 * Macro DARRAY
 ***********************************/

#define DARRAY(__Type__, __Name__) DPROP(TArray<__Type__>, __Name__);

/***********************************
 * Macro DMAP
 ***********************************/

#define DMAP(__Type__, __Name__) DPROP(TMap<FString COMMA __Type__>, __Name__);

/***********************************
 * Macro DLINK
 ***********************************/

#define DLINK(__ReturnType__, __Name__, __Path__) \
public:                                           \
	const PsDataTools::TDLink<DPropType_##__Name__::Type, __ReturnType__, ThisClass, PsDataTools::FDataStringViewChar(#__Name__).GetHash(), false> LinkBy##__Name__{#__Name__, this, #__Path__};

/***********************************
 * Macro DLINK_ABSTRACT
 ***********************************/

#define DLINK_ABSTRACT(__ReturnType__, __Name__) \
public:                                          \
	const PsDataTools::TDLink<DPropType_##__Name__::Type, __ReturnType__, ThisClass, PsDataTools::FDataStringViewChar(#__Name__).GetHash(), true> LinkByAbstract##__Name__{#__Name__, this};

/***********************************
 * Macro DEFERRED_EVENT_PROCESSING
 ***********************************/

#define DEFERRED_EVENT_PROCESSING() \
	FPsDataEventScopeGuard EventScopeGuard();

#if PSDATA_OLD_MACRO

/***********************************
 * Macro DPROP_OLD
 ***********************************/

#define DPROP_OLD(__Type__, __Name__)                                                                                             \
protected:                                                                                                                        \
	using DPropType_##__Name__ = PsDataTools::TDProp<__Type__, ThisClass, PsDataTools::FDataStringViewChar(#__Name__).GetHash()>; \
	DPropType_##__Name__ DProp_##__Name__{#__Name__, this};                                                                       \
                                                                                                                                  \
public:                                                                                                                           \
	typename DPropType_##__Name__::TReturnType<true> Get##__Name__() const                                                        \
	{                                                                                                                             \
		return DProp_##__Name__.Get();                                                                                            \
	}                                                                                                                             \
                                                                                                                                  \
	typename DPropType_##__Name__::TReturnType<false> Get##__Name__()                                                             \
	{                                                                                                                             \
		return DProp_##__Name__.Get();                                                                                            \
	}                                                                                                                             \
                                                                                                                                  \
	void Set##__Name__(typename DPropType_##__Name__::TValueType<false> Value)                                                    \
	{                                                                                                                             \
		DProp_##__Name__.Set(Value);                                                                                              \
	}                                                                                                                             \
                                                                                                                                  \
	static const FString& Get##__Name__##ChangedEventName()                                                                       \
	{                                                                                                                             \
		return DPropType_##__Name__::StaticField()->GetChangedEventName();                                                        \
	}                                                                                                                             \
                                                                                                                                  \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                                   \
	{                                                                                                                             \
		return DProp_##__Name__.Bind(Delegate);                                                                                   \
	}

/***********************************
 * Macro DPROP_CONST_OLD
 ***********************************/

#define DPROP_CONST_OLD(__Type__, __Name__, __Friend__)                    \
	DPROP_CONST(__Type__, __Name__, __Friend__)                            \
                                                                           \
	typename DPropType_##__Name__::TReturnType<true> Get##__Name__() const \
	{                                                                      \
		return __Name__.Get();                                             \
	}                                                                      \
                                                                           \
	typename DPropType_##__Name__::TReturnType<true> Get##__Name__()       \
	{                                                                      \
		return __Name__.Get();                                             \
	}

/***********************************
 * Macro DARRAY_OLD
 ***********************************/

#define DARRAY_OLD(__Type__, __Name__) DPROP_OLD(TArray<__Type__>, __Name__);

/***********************************
 * Macro DMAP_OLD
 ***********************************/

#define DMAP_OLD(__Type__, __Name__) DPROP_OLD(TMap<FString COMMA __Type__>, __Name__);

#endif