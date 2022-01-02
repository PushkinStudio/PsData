// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

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
#include "PsDataStruct.h"
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
struct TDMeta
{
	TDMeta(const char* MetaString)
	{
		static const auto StaticInit = FDataReflection::InitMeta(MetaString);
	}

	TDMeta(const TDMeta&) = delete;
	TDMeta& operator=(const TDMeta&) = delete;
	TDMeta(TDMeta&&) = delete;
	TDMeta& operator=(TDMeta&&) = delete;
};

/***********************************
 * TDBaseProp
 ***********************************/

template <class OwnerClass, int32 Hash, typename FieldType, typename... Other>
struct TDBaseProp
{
	TDBaseProp(OwnerClass* InPropertyOwner)
		: PropertyOwner(InPropertyOwner)
	{
	}

	TDBaseProp(const TDBaseProp&) = delete;
	TDBaseProp& operator=(const TDBaseProp&) = delete;
	TDBaseProp(TDBaseProp&&) = delete;
	TDBaseProp& operator=(TDBaseProp&&) = delete;

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
	static_assert(TIsContainer<T>::Value, "Unsupported type");

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

public:
	using Type = T;
	template <bool bConst>
	using TValueType = TConstRefType<T, bConst>;
	template <bool bConst>
	using TReturnType = TValueType<bConst>;

	TDProp(const char* Name, OwnerClass* InOwner)
		: TDBaseProp<OwnerClass, Hash, FDataField>(InOwner)
	{
		static const auto StaticInit = FDataReflection::InitProperty(StaticClass<OwnerClass>(), Name, &GetContext<Type>(), this->ProtectedStaticField());
		const auto Index = FPsDataFriend::GetProperties(InOwner).Add(this);

#if !UE_BUILD_SHIPPING
		check(Index == this->ProtectedStaticField()->Index);
#endif
	}

	TDProp(const TDProp&) = delete;
	TDProp& operator=(const TDProp&) = delete;
	TDProp(TDProp&&) = delete;
	TDProp& operator=(TDProp&&) = delete;
	virtual ~TDProp() override {}

	FPsDataBind Bind(const FPsDataDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return GetOwner()->Bind(GetField()->GetChangedEventName(), Delegate, Flags);
	}

	FPsDataBind Bind(const FPsDataDynamicDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return GetOwner()->Bind(GetField()->GetChangedEventName(), Delegate, Flags);
	}

	void Unbind(const FPsDataDelegate& Delegate) const
	{
		return GetOwner()->Unbind(GetField()->GetChangedEventName(), Delegate);
	}

	void Unbind(const FPsDataDynamicDelegate& Delegate) const
	{
		return GetOwner()->Unbind(GetField()->GetChangedEventName(), Delegate);
	}

	void Set(TValueType<false> InValue)
	{
		this->SetValue(InValue);
	}

	TReturnType<false> Get()
	{
		return this->GetValue();
	}

	TReturnType<true> Get() const
	{
		return this->GetValue();
	}

	void operator=(TValueType<false> InValue)
	{
		Set(InValue);
	}

	TReturnType<false> operator*()
	{
		return Get();
	}

	TReturnType<true> operator*() const
	{
		return Get();
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsPointer<K>::Value>::Type>
	TReturnType<false> operator->()
	{
		return Get();
	}

	template <typename K = T,
		typename = typename TEnableIf<TIsPointer<K>::Value>::Type>
	TReturnType<true> operator->() const
	{
		return Get();
	}

	template <typename K = T,
		typename = typename TEnableIf<!TIsPointer<K>::Value>::Type>
	const T* operator->() const
	{
		return &this->GetValue();
	}

	bool operator==(TValueType<true> InValue) const
	{
		return TTypeComparator<T>::Compare(this->GetValue(), InValue);
	}

	bool operator!=(TValueType<true> InValue) const
	{
		return !TTypeComparator<T>::Compare(this->GetValue(), InValue);
	}

	operator TReturnType<false>()
	{
		return Get();
	}

	operator TReturnType<true>() const
	{
		return Get();
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator+=(T InValue)
	{
		T Result = Get() + InValue;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator-=(T InValue)
	{
		T Result = Get() - InValue;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator*=(T InValue)
	{
		T Result = Get() * InValue;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator/=(T InValue)
	{
		T Result = Get() / InValue;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator%=(T InValue)
	{
		T Result = Get() % InValue;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator++()
	{
		T Result = Get() + 1;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator++(int)
	{
		T Result = Get();
		Set(Result + 1);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator--()
	{
		T Result = Get() - 1;
		Set(Result);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator--(int)
	{
		T Result = Get();
		Set(Result - 1);
		return Result;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator+(T InValue) const
	{
		return Get() + InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator-(T InValue) const
	{
		return Get() - InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator*(T InValue) const
	{
		return Get() * InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator/(T InValue) const
	{
		return Get() / InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator%(T InValue) const
	{
		return Get() % InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator+() const
	{
		return Get();
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	T operator-() const
	{
		return -Get();
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator>(T InValue) const
	{
		return Get() > InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator>=(T InValue) const
	{
		return Get() >= InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator<(T InValue) const
	{
		return Get() < InValue;
	}

	template <typename K = T,
		typename = typename TEnableIf<std::is_arithmetic<K>::value>::Type>
	bool operator<=(T InValue) const
	{
		return Get() <= InValue;
	}
};

/***********************************
 * TDPropConst
 ***********************************/

template <typename T, class OwnerClass, int32 Hash, typename Friend>
struct TDPropConst
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

	TDPropConst(const TDPropConst&) = delete;
	TDPropConst& operator=(const TDPropConst&) = delete;
	TDPropConst(TDPropConst&&) = delete;
	TDPropConst& operator=(TDPropConst&&) = delete;

	TReturnType<true> Get()
	{
		return Mutable.Get();
	}

	TReturnType<true> Get() const
	{
		return Mutable.Get();
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

private:
	friend Friend;
	DPropType Mutable;
};

/***********************************
 * TDPropProxy
 ***********************************/

template <typename T, class OwnerClass, int32 Hash>
struct TDPropProxy : public TDBaseProp<OwnerClass, Hash, FDataField>, protected TDataProperty<T>
{
	static_assert(TIsContainer<T>::Array || TIsContainer<T>::Map, "Unsupported type");

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

public:
	using Type = T;
	template <bool bConst>
	using TValueType = TConstRefType<Type, bConst>;
	template <bool bConst>
	using TReturnType = typename TSelector<
		TPsDataBaseArrayProxy<typename TIsContainer<T>::Type, bConst>,
		TPsDataBaseMapProxy<typename TIsContainer<T>::Type, bConst>,
		TIsContainer<T>::Array>::Value;
	using FKeyType = typename TReturnType<false>::FKeyType;

	TDPropProxy(const char* Name, OwnerClass* InOwner)
		: TDBaseProp<OwnerClass, Hash, FDataField>(InOwner)
		, Proxy(this)
	{
		static const auto StaticInit = FDataReflection::InitProperty(StaticClass<OwnerClass>(), Name, &GetContext<Type>(), this->ProtectedStaticField());
		const auto Index = FPsDataFriend::GetProperties(InOwner).Add(this);

#if !UE_BUILD_SHIPPING
		check(Index == this->ProtectedStaticField()->Index);
#endif
	}

	TDPropProxy(const TDPropProxy&) = delete;
	TDPropProxy& operator=(const TDPropProxy&) = delete;
	TDPropProxy(TDPropProxy&&) = delete;
	TDPropProxy& operator=(TDPropProxy&&) = delete;
	virtual ~TDPropProxy() override {}

	FPsDataBind Bind(const FPsDataDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return GetOwner()->Bind(GetField()->GetChangedEventName(), Delegate, Flags);
	}

	FPsDataBind Bind(const FPsDataDynamicDelegate& Delegate, EDataBindFlags Flags = EDataBindFlags::Default) const
	{
		return GetOwner()->Bind(GetField()->GetChangedEventName(), Delegate, Flags);
	}

	void Unbind(const FPsDataDelegate& Delegate) const
	{
		return GetOwner()->Unbind(GetField()->GetChangedEventName(), Delegate);
	}

	void Unbind(const FPsDataDynamicDelegate& Delegate) const
	{
		return GetOwner()->Unbind(GetField()->GetChangedEventName(), Delegate);
	}

	void Set(TValueType<false> InValue)
	{
		Proxy = InValue;
	}

	TReturnType<false> Get()
	{
		return Proxy;
	}

	TReturnType<true> Get() const
	{
		return Proxy;
	}

	void operator=(TValueType<false> InValue)
	{
		Proxy = InValue;
	}

	TReturnType<false> operator*()
	{
		return Proxy;
	}

	TReturnType<true> operator*() const
	{
		return Proxy;
	}

	TReturnType<false>* operator->()
	{
		return &Proxy;
	}

	const TReturnType<false>* operator->() const
	{
		return &Proxy;
	}

	auto operator[](const FKeyType& Key)
	{
		return Proxy[Key];
	}

	auto operator[](const FKeyType& Key) const
	{
		return Proxy[Key];
	}

	operator TReturnType<false>()
	{
		return Proxy;
	}

	operator TReturnType<true>() const
	{
		return Proxy;
	}

	auto begin() { return Proxy.begin(); }
	auto end() { return Proxy.end(); }
	auto begin() const { return Proxy.begin(); }
	auto end() const { return Proxy.end(); }

private:
	TReturnType<false> Proxy;
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

	TDLink(const TDLink&) = delete;
	TDLink& operator=(const TDLink&) = delete;
	TDLink(TDLink&&) = delete;
	TDLink& operator=(TDLink&&) = delete;

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

template <typename T, class OwnerClass, int32 Hash>
using TDPropSelector = typename TSelector<
	TDProp<T, OwnerClass, Hash>,
	TDPropProxy<T, OwnerClass, Hash>,
	TIsContainer<T>::Value>::Value;

} // namespace PsDataTools

/***********************************
 * Common macros
 ***********************************/

#define COMMA ,

/***********************************
 * Macro DMETA
 ***********************************/

#define DMETA(...) \
private:           \
	PsDataTools::TDMeta<ThisClass, __LINE__> PREPROCESSOR_JOIN(DMeta_, __LINE__){#__VA_ARGS__};

/***********************************
 * Macro DPROP
 ***********************************/

#define DPROP(__Type__, __Name__)                                                                                \
protected:                                                                                                       \
	using DPropType_##__Name__ = PsDataTools::TDPropSelector<__Type__, ThisClass, GetStaticTypeHash(#__Name__)>; \
                                                                                                                 \
public:                                                                                                          \
	DPropType_##__Name__ __Name__{#__Name__, this};

/***********************************
 * Macro DPROP_CONST
 ***********************************/

#define DPROP_CONST(__Type__, __Name__, __Friend__)                                                                       \
protected:                                                                                                                \
	DMETA(ReadOnly, Strict);                                                                                              \
	using DPropType_##__Name__ = PsDataTools::TDPropConst<__Type__, ThisClass, GetStaticTypeHash(#__Name__), __Friend__>; \
                                                                                                                          \
public:                                                                                                                   \
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
	const PsDataTools::TDLink<DPropType_##__Name__::Type, __ReturnType__, ThisClass, GetStaticTypeHash(#__Name__), false> LinkBy##__Name__{#__Name__, this, #__Path__};

/***********************************
 * Macro DLINK_ABSTRACT
 ***********************************/

#define DLINK_ABSTRACT(__ReturnType__, __Name__) \
public:                                          \
	const PsDataTools::TDLink<DPropType_##__Name__::Type, __ReturnType__, ThisClass, GetStaticTypeHash(#__Name__), true> LinkByAbstract##__Name__{#__Name__, this};

/***********************************
 * Macro MAKE_TABLE_ROW
 ***********************************/

#define MAKE_TABLE_STRUCT(...)                                     \
protected:                                                         \
	virtual void InitStructProperties() override                   \
	{                                                              \
		Super::InitStructProperties();                             \
                                                                   \
		{                                                          \
			__VA_ARGS__                                            \
		}                                                          \
                                                                   \
		if (!UPsDataStruct::Find(ThisClass::StaticClass()))        \
			UPsDataStruct::Create(ThisClass::StaticClass(), this); \
	}                                                              \
                                                                   \
public:

/***********************************
 * Macro DEFERRED_EVENT_PROCESSING
 ***********************************/

#define DEFERRED_EVENT_PROCESSING() \
	FPsDataEventScopeGuard EventScopeGuard;

#if PSDATA_OLD_MACRO

/***********************************
 * Macro DPROP_OLD
 ***********************************/

#define DPROP_OLD(__Type__, __Name__)                                                                            \
protected:                                                                                                       \
	using DPropType_##__Name__ = PsDataTools::TDPropSelector<__Type__, ThisClass, GetStaticTypeHash(#__Name__)>; \
	DPropType_##__Name__ DProp_##__Name__{#__Name__, this};                                                      \
                                                                                                                 \
public:                                                                                                          \
	void Set##__Name__(typename DPropType_##__Name__::TValueType<false> Value)                                   \
	{                                                                                                            \
		DProp_##__Name__.Set(Value);                                                                             \
	}                                                                                                            \
                                                                                                                 \
	typename DPropType_##__Name__::TReturnType<false> Get##__Name__()                                            \
	{                                                                                                            \
		return DProp_##__Name__;                                                                                 \
	}                                                                                                            \
                                                                                                                 \
	typename DPropType_##__Name__::TReturnType<true> Get##__Name__() const                                       \
	{                                                                                                            \
		return DProp_##__Name__;                                                                                 \
	}                                                                                                            \
                                                                                                                 \
	static const FString& Get##__Name__##ChangedEventName()                                                      \
	{                                                                                                            \
		return DPropType_##__Name__::StaticField()->GetChangedEventName();                                       \
	}                                                                                                            \
                                                                                                                 \
	FPsDataBind Bind_##__Name__##Changed(const FPsDataDelegate& Delegate) const                                  \
	{                                                                                                            \
		return DProp_##__Name__.Bind(Delegate);                                                                  \
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