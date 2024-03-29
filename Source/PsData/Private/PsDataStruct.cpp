// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "PsDataStruct.h"

#include "PsDataCore.h"
#include "Serialize/PsDataStructSerialization.h"

#include "UObject/Class.h"

#if WITH_EDITORONLY_DATA
#include "EdGraphSchema_K2.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#endif

namespace PsDataTools
{

struct FDataPropertyFlags
{
	EObjectFlags ObjectFlags;
	EPropertyFlags PropertyFlags;

	FDataPropertyFlags(EObjectFlags InObjectFlags, EPropertyFlags InPropertyFlags)
		: ObjectFlags(InObjectFlags)
		, PropertyFlags(InPropertyFlags)
	{
	}

	FDataPropertyFlags(EObjectFlags InObjectFlags)
		: ObjectFlags(InObjectFlags)
		, PropertyFlags(EPropertyFlags::CPF_None)
	{
	}

	FDataPropertyFlags(EPropertyFlags InPropertyFlags)
		: ObjectFlags(EObjectFlags::RF_NoFlags)
		, PropertyFlags(InPropertyFlags)
	{
	}
};

#if OLD_PROPERTY_STYLE
FProperty* CreateSingleProperty(bool bTypeFromField, UField* Owner, FProperty* BaseProperty, const FDataField* Field, FDataPropertyFlags Flags, const FString& Postfix = TEXT(""))
#else
FProperty* CreateSingleProperty(bool bTypeFromField, const FFieldVariant& Owner, FProperty* BaseProperty, const FDataField* Field, FDataPropertyFlags Flags, const FString& Postfix = TEXT(""))
#endif
{
	const FString CompleteName = Postfix.IsEmpty() ? Field->GetNameForSerialize() : FString::Printf(TEXT("%s_%s"), *Field->GetNameForSerialize(), *Postfix);

	FProperty* Property = nullptr;
	if (Field->Context->IsData() && bTypeFromField)
	{
		const auto FieldClass = CastChecked<UClass>(Field->Context->GetUEType());
		const auto FieldStruct = UPsDataStruct::Find(FieldClass);
		if (!FieldStruct)
		{
			UE_LOG(LogDataReflection, Fatal, TEXT("Attempting to use uncreated structure. Use macro \"MAKE_TABLE_STRUCT\" for %s"), *FieldClass->GetName());
		}

		FStructProperty* StructProperty = NewProperty<FStructProperty>(Owner, *CompleteName, Flags.ObjectFlags, Flags.PropertyFlags);
		StructProperty->Struct = FieldStruct;
		Property = StructProperty;
	}
	else
	{
		if (Field->Context->IsEnum())
		{
			FEnumProperty* EnumProperty = NewProperty<FEnumProperty>(Owner, *CompleteName, Flags.ObjectFlags, Flags.PropertyFlags);
			EnumProperty->SetEnum(CastChecked<UEnum>(Field->Context->GetUEType()));

			FNumericProperty* UnderlyingProp = NewProperty<FByteProperty>(EnumProperty, TEXT("UnderlyingType"), Flags.ObjectFlags);
			EnumProperty->AddCppProperty(UnderlyingProp);

			Property = EnumProperty;
		}
		else
		{
			Property = DuplicateProperty(BaseProperty, Owner, *CompleteName, Flags.ObjectFlags, Flags.PropertyFlags);

			if (const auto SoftClassProperty = CastField<FSoftClassProperty>(Property))
			{
				SoftClassProperty->SetMetaClass(CastChecked<UClass>(Field->Context->GetUEType()));
				SoftClassProperty->PropertyClass = UClass::StaticClass();
			}
			else if (const auto SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
			{
				SoftObjectProperty->PropertyClass = CastChecked<UClass>(Field->Context->GetUEType());
			}
		}
	}

#if OLD_PROPERTY_STYLE
	Owner->AddCppProperty(Property);
#else
	if (Owner.IsUObject())
	{
		CastChecked<UField>(Owner.ToUObject())->AddCppProperty(Property);
	}
	else
	{
		Owner.ToField()->AddCppProperty(Property);
	}
#endif

	return Property;
}

#if OLD_PROPERTY_STYLE
FProperty* CreateProperty(UField* Owner, FProperty* BaseProperty, const FDataField* Field)
#else
FProperty* CreateProperty(const FFieldVariant& Owner, FProperty* BaseProperty, const FDataField* Field)
#endif
{
	FDataPropertyFlags Flags = FDataPropertyFlags(RF_Public, CPF_Edit | CPF_BlueprintVisible);

	FProperty* Property = nullptr;
	if (Field->Context->IsArray())
	{
		FArrayProperty* BaseArrayProperty = CastFieldChecked<FArrayProperty>(BaseProperty);
		FArrayProperty* ArrayProperty = NewProperty<FArrayProperty>(Owner, *Field->GetNameForSerialize(), Flags.ObjectFlags, Flags.PropertyFlags);
		CreateSingleProperty(true, ArrayProperty, BaseArrayProperty->Inner, Field, RF_Public, TEXT("Value"));
		Property = ArrayProperty;
	}
	else if (Field->Context->IsMap())
	{
		FMapProperty* BaseMapProperty = CastFieldChecked<FMapProperty>(BaseProperty);
		FMapProperty* MapProperty = NewProperty<FMapProperty>(Owner, *Field->GetNameForSerialize(), Flags.ObjectFlags, Flags.PropertyFlags);
		CreateSingleProperty(false, MapProperty, BaseMapProperty->KeyProp, Field, RF_Public, TEXT("Key"));
		CreateSingleProperty(true, MapProperty, BaseMapProperty->ValueProp, Field, RF_Public, TEXT("Value"));
		Property = MapProperty;
	}
	else
	{
		Property = CreateSingleProperty(true, Owner, BaseProperty, Field, Flags);
	}

	{
		FArchive Ar;
		Property->LinkWithoutChangingOffset(Ar);
	}

	return Property;
}

void ApplyDataField(UPsDataStruct* Struct, const FDataField* Field)
{
	UScriptStruct* SuperStruct = Cast<UScriptStruct>(Struct->GetSuperStruct());
	if (SuperStruct && SuperStruct->FindPropertyByName(*Field->GetNameForSerialize()))
	{
		return;
	}

	const auto Function = Field->Context->GetUFunctions().ResolveGetFunction();
	FProperty* BaseProperty = Function->FindPropertyByName(TEXT("Out"));
	check(BaseProperty);

	const auto Property = CreateProperty(Struct, BaseProperty, Field);

#if WITH_EDITORONLY_DATA
	FEdGraphPinType PinType;
	GetDefault<UEdGraphSchema_K2>()->ConvertPropertyToPinType(Property, PinType);

	const auto EditorData = CastChecked<UUserDefinedStructEditorData>(Struct->EditorData);
	FStructVariableDescription VariableDescription;
	VariableDescription.VarName = *Field->GetNameForSerialize();
	VariableDescription.FriendlyName = Field->GetNameForSerialize();
	VariableDescription.SetPinType(PinType);
	VariableDescription.VarGuid = FGuid(GetTypeHash(Struct->GetName()), GetTypeHash(Field->GetNameForSerialize()), GetTypeHash(Struct->GetName() + TEXT("_") + Field->GetNameForSerialize()), 0);
	EditorData->VariablesDescriptions.Add(VariableDescription);
#endif
}

} // namespace PsDataTools

FString UPsDataStruct::GetStructName(UClass* PsDataClass)
{
	return FString::Printf(TEXT("%s_DataTableRow"), *PsDataClass->GetName());
}

UPsDataStruct* UPsDataStruct::Find(UClass* PsDataClass)
{
	check(PsDataClass && PsDataClass->IsChildOf(UPsData::StaticClass()));

	const auto StructName = GetStructName(PsDataClass);
	if (UPsDataStruct* ExistingStruct = FindObject<UPsDataStruct>(PsDataClass, *StructName))
	{
		return ExistingStruct;
	}

	return nullptr;
}

UPsDataStruct* UPsDataStruct::Create(UClass* PsDataClass, UPsData* DefaultData)
{
	check(PsDataClass && PsDataClass->IsChildOf(UPsData::StaticClass()) && DefaultData);

	UScriptStruct* SuperStruct = nullptr;
	if (PsDataClass->GetSuperClass() != UPsData::StaticClass())
	{
		SuperStruct = Create(PsDataClass->GetSuperClass(), DefaultData);
	}

	const auto StructName = GetStructName(PsDataClass);
	if (UPsDataStruct* ExistingStruct = FindObject<UPsDataStruct>(PsDataClass, *StructName))
	{
		UE_LOG(LogDataReflection, Fatal, TEXT("Attempting to recreate struct for class %s"), *PsDataClass->GetName());
	}

	UPsDataStruct* NewStruct = NewObject<UPsDataStruct>(PsDataClass, *StructName, RF_Public | RF_Standalone);
#if WITH_EDITORONLY_DATA
	NewStruct->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
#endif
	NewStruct->SetSuperStruct(SuperStruct);
	NewStruct->StructFlags = STRUCT_NoFlags;

	auto Fields = PsDataTools::FDataReflection::GetFieldsByClass(PsDataClass)->GetFieldsList();
	Fields.Sort([](const FDataField& A, const FDataField& B) {
		return A.Index > B.Index;
	});

#if WITH_EDITORONLY_DATA
	NewStruct->EditorData = NewObject<UUserDefinedStructEditorData>(NewStruct, NAME_None, RF_Transient);
#endif

	for (const auto Field : Fields)
	{
		if (!Field->Meta.bHidden)
		{
			PsDataTools::ApplyDataField(NewStruct, Field);
		}
	}

	NewStruct->Bind();
	NewStruct->StaticLink(true);
	NewStruct->AddToRoot();

	NewStruct->StructFlags = static_cast<EStructFlags>(NewStruct->StructFlags & ~STRUCT_ZeroConstructor);

	FPsDataStructSerializer Serializer;
	DefaultData->DataSerialize(&Serializer);

	NewStruct->Finalize(Serializer.GetRaw(NewStruct, false));
	return NewStruct;
}

UPsDataStruct::UPsDataStruct()
	: RawStruct(nullptr)
{
}

FProperty* UPsDataStruct::CustomFindProperty(const FName Name) const
{
	for (FProperty* Property : TFieldRange<FProperty>(this))
	{
		if (Property->GetFName() == Name)
		{
			return Property;
		}
	}

	return nullptr;
}

void UPsDataStruct::InitializeStruct(void* Dest, int32 ArrayDim) const
{
	Super::InitializeStruct(Dest, ArrayDim);
}

void UPsDataStruct::Finalize(uint8* DefaultStruct)
{
	RawStruct = DefaultStruct;
	DefaultStructInstance = FUserStructOnScopeIgnoreDefaults(this, RawStruct);
}

void UPsDataStruct::BeginDestroy()
{
	Super::BeginDestroy();
	if (RawStruct)
	{
		FMemory::Free(RawStruct);
		RawStruct = nullptr;
	}
}
