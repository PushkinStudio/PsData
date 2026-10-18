// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/UnrealType.h"
#include <Runtime/Launch/Resources/Version.h>

#define PS_FUNC (FString(__FUNCTION__))              // Current Class Name + Function Name where this is called
#define PS_LINE (FString::FromInt(__LINE__))         // Current Line Number in the code where this is called
#define PS_FUNC_LINE (PS_FUNC + "(" + PS_LINE + ")") // Current Class and Line Number where this is called!

#define OLD_CSV_IMPORT_FACTORY ENGINE_MINOR_VERSION < 25 && ENGINE_MAJOR_VERSION == 4
#define OLD_PROPERTY_STYLE ENGINE_MINOR_VERSION < 25 && ENGINE_MAJOR_VERSION == 4

/** True when the engine building this is Major.Minor or newer */
#define PSDATA_ENGINE_AT_LEAST(Major, Minor) (ENGINE_MAJOR_VERSION > (Major) || (ENGINE_MAJOR_VERSION == (Major) && ENGINE_MINOR_VERSION >= (Minor)))

/** "Don't shrink" for TArray::RemoveAt/Pop/SetNum: a bool until 5.4, EAllowShrinking since; 5.8 removed the bool overloads */
#if PSDATA_ENGINE_AT_LEAST(5, 4)
#define PSDATA_SHRINK(bAllowShrinking) ((bAllowShrinking) ? EAllowShrinking::Yes : EAllowShrinking::No)
#else
#define PSDATA_SHRINK(bAllowShrinking) (bAllowShrinking)
#endif
#define PSDATA_NO_SHRINK PSDATA_SHRINK(false)

#if OLD_PROPERTY_STYLE
using FProperty = UProperty;
using FNumericProperty = UNumericProperty;
using FByteProperty = UByteProperty;
using FInt8Property = UInt8Property;
using FInt16Property = UInt16Property;
using FIntProperty = UIntProperty;
using FInt64Property = UInt64Property;
using FUInt16Property = UUInt16Property;
using FUInt32Property = UUInt32Property;
using FUInt64Property = UUInt64Property;
using FFloatProperty = UFloatProperty;
using FDoubleProperty = UDoubleProperty;
using FBoolProperty = UBoolProperty;
using FObjectPropertyBase = UObjectPropertyBase;
using FObjectProperty = UObjectProperty;
using FWeakObjectProperty = UWeakObjectProperty;
using FLazyObjectProperty = ULazyObjectProperty;
using FSoftObjectProperty = USoftObjectProperty;
using FClassProperty = UClassProperty;
using FSoftClassProperty = USoftClassProperty;
using FInterfaceProperty = UInterfaceProperty;
using FNameProperty = UNameProperty;
using FStrProperty = UStrProperty;
using FArrayProperty = UArrayProperty;
using FMapProperty = UMapProperty;
using FSetProperty = USetProperty;
using FStructProperty = UStructProperty;
using FDelegateProperty = UDelegateProperty;
using FMulticastDelegateProperty = UMulticastDelegateProperty;
using FMulticastInlineDelegateProperty = UMulticastInlineDelegateProperty;
using FMulticastSparseDelegateProperty = UMulticastSparseDelegateProperty;
using FEnumProperty = UEnumProperty;
using FTextProperty = UTextProperty;

using FPropertyHelpers = UPropertyHelpers;

template <typename FieldType>
FieldType* CastField(UField* Src)
{
	return Cast<FieldType>(Src);
}

template <typename FieldType>
const FieldType* CastField(const UField* Src)
{
	return Cast<FieldType>(Src);
}

template <typename FieldType>
FieldType* CastFieldChecked(UField* Src)
{
	return CastChecked<FieldType>(Src);
}

template <typename FieldType>
const FieldType* CastFieldChecked(const UField* Src)
{
	return CastChecked<FieldType>(Src);
}

template <typename FieldType>
FieldType* NewProperty(UField* Owner, const FName& Name, EObjectFlags ObjectFlags, EPropertyFlags PropertyFlags = EPropertyFlags::CPF_None)
{
	FieldType* Property = NewObject<FieldType>(Owner, Name, ObjectFlags);
	Property->PropertyFlags = PropertyFlags;
	return Property;
}

template <typename FieldType>
FieldType* DuplicateProperty(FieldType* Target, UField* Owner, const FName& Name, EObjectFlags ObjectFlags, EPropertyFlags PropertyFlags = EPropertyFlags::CPF_None)
{
	FieldType* Property = CastChecked<FProperty>(StaticDuplicateObject(Target, Owner, Name, ObjectFlags));
	Property->SetFlags(ObjectFlags);
	Property->PropertyFlags = PropertyFlags;
	return Property;
}

#else

// 5.8 dropped object flags from FField: the constructors and FField::Duplicate stop taking them and
// FField::SetFlags became a no-op, so ObjectFlags is only honoured on older engines
template <typename FieldType>
FieldType* NewProperty(const FFieldVariant& Owner, const FName& Name, EObjectFlags ObjectFlags, EPropertyFlags PropertyFlags = EPropertyFlags::CPF_None)
{
#if PSDATA_ENGINE_AT_LEAST(5, 8)
	FieldType* Property = new FieldType(Owner, Name);
#else
	FieldType* Property = new FieldType(Owner, Name, ObjectFlags);
#endif
	Property->SetPropertyFlags(PropertyFlags);
	return Property;
}

template <typename FieldType>
FieldType* DuplicateProperty(FieldType* Target, const FFieldVariant& Owner, const FName& Name, EObjectFlags ObjectFlags, EPropertyFlags PropertyFlags = EPropertyFlags::CPF_None)
{
#if PSDATA_ENGINE_AT_LEAST(5, 8)
	FieldType* Property = CastFieldChecked<FieldType>(FField::Duplicate(Target, Owner, Name));
#else
	FieldType* Property = CastFieldChecked<FieldType>(FField::Duplicate(Target, Owner, Name, ObjectFlags));
	Property->SetFlags(ObjectFlags);
#endif
	Property->PropertyFlags = PropertyFlags;
	return Property;
}

#endif
