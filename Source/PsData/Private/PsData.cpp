// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#include "PsData.h"

#include "PsDataCore.h"
#include "PsDataDeferredTask.h"
#include "PsDataProperty.h"
#include "PsDataRoot.h"
#include "Serialize/PsDataBinarySerialization.h"
#include "Serialize/Stream/PsDataBufferInputStream.h"
#include "Serialize/Stream/PsDataBufferOutputStream.h"
#include "Serialize/Stream/PsDataMD5OutputStream.h"
#include "Types/PsData_UPsData.h"

FSimpleMulticastDelegate FDataDelegates::OnPostDataModuleInit;
TPsDataSimplePromise FDataDelegates::PostDataModuleInitPromise;

/***********************************
* PsData friend
***********************************/

namespace PsDataTools
{
void FPsDataFriend::ChangeDataName(UPsData* Data, const FString& Name, const FString& CollectionName)
{
	if (Data->DataKey != Name || Data->CollectionKey != CollectionName)
	{
		Data->DataKey = Name;
		Data->CollectionKey = CollectionName;
		if (Data->IsBound(UPsDataEvent::NameChanged, false))
		{
			Data->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::NameChanged, false));
		}
	}
}

void FPsDataFriend::AddChild(UPsData* Parent, UPsData* Data)
{
	if (Data->Parent.IsValid())
	{
		UE_LOG(LogData, Fatal, TEXT("Child already added"));
		return;
	}

	Data->Parent = Parent;
	Parent->Children.Add(Data);

	if (Data->IsBound(UPsDataEvent::AddedToParent, false))
	{
		Data->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::AddedToParent, false));
	}

	if (Data->IsBound(UPsDataEvent::Added, true))
	{
		Data->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::Added, true));
	}
}

void FPsDataFriend::RemoveChild(UPsData* Parent, UPsData* Data)
{
	if (Data->Parent != Parent)
	{
		UE_LOG(LogData, Fatal, TEXT("Child not added"));
		return;
	}

	const auto bIsBound = Data->IsBound(UPsDataEvent::Removed, true);

	Parent->Children.Remove(Data);
	Data->Parent.Reset();

	if (Data->IsBound(UPsDataEvent::RemovedFromParent, false))
	{
		Data->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::RemovedFromParent, false));
	}

	if (bIsBound)
	{
		auto Event = UPsDataEvent::ConstructEvent(UPsDataEvent::Removed, true);
		Data->Broadcast(Event);
		Parent->Broadcast(Event, Data);
	}
}

void FPsDataFriend::Changed(UPsData* Data, const TSharedPtr<const FDataField>& Field)
{
	Data->DropHash();

	if (Field->Meta.bEvent && Data->IsBound(Field->GetChangedEventName(), Field->Meta.bBubbles))
	{
		Data->Broadcast(UPsDataEvent::ConstructEvent(Field->GetChangedEventName(), Field->Meta.bBubbles));
	}

	if (!Data->bChanged)
	{
		Data->bChanged = true;
		DeferredTask(Data, [Data]() {
			Data->bChanged = false;
			if (Data->IsBound(UPsDataEvent::Changed, false))
			{
				Data->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::Changed, false));
			}
		});
	}
}

void FPsDataFriend::InitProperties(UPsData* Data)
{
	Data->InitProperties();
	Data->PostDeserialize();
}

TArray<FAbstractDataProperty*>& FPsDataFriend::GetProperties(UPsData* Data)
{
	return Data->Properties;
}

void FPsDataFriend::Serialize(const UPsData* Data, FPsDataSerializer* Serializer)
{
	Data->DataSerializeInternal(Serializer);
}

void FPsDataFriend::Deserialize(UPsData* Data, FPsDataDeserializer* Deserializer)
{
	Data->DataDeserializeInternal(Deserializer);
}
} // namespace PsDataTools

/***********************************
 * FPsDataBind
 ***********************************/

FPsDataBind::FPsDataBind(TSharedRef<FDelegateWrapper> InDelegateWrapper)
	: DelegateWrapper(InDelegateWrapper)
{
}

FPsDataBind::FPsDataBind()
	: DelegateWrapper(nullptr)
{
}

void FPsDataBind::Unbind()
{
	if (DelegateWrapper.IsValid())
	{
		DelegateWrapper->Delegate.Unbind();
		DelegateWrapper->DynamicDelegate.Unbind();
	}
	DelegateWrapper.Reset();
}

/***********************************
 * FPsDataBindCollection
 ***********************************/

FPsDataBindCollection::FPsDataBindCollection()
{
	Collection.Reserve(2);
}

void FPsDataBindCollection::Add(const FPsDataBind& Bind)
{
	if (Bind.DelegateWrapper.IsValid())
	{
		Collection.Add(Bind.DelegateWrapper.ToSharedRef());
	}
}

void FPsDataBindCollection::Unbind()
{
	for (auto& DelegateWrapper : Collection)
	{
		DelegateWrapper->Delegate.Unbind();
		DelegateWrapper->DynamicDelegate.Unbind();
	}
	Collection.Reset();
}

/***********************************
 * FPsDataReport
 ***********************************/

FPsDataReport::FPsDataReport(EPsDataReportType InType, const FString& InSourcePath, const FString& InReason)
	: Type(InType)
	, SourcePath(InSourcePath)
	, Reason(InReason)
{
}

FPsDataReport::FPsDataReport(EPsDataReportType InType, const FString& InSourcePath, const FString& InReason, const FString& InLinkedPath)
	: Type(InType)
	, SourcePath(InSourcePath)
	, Reason(InReason)
	, LinkedPath(InLinkedPath)
{
}

/***********************************
* PSDATA!
***********************************/

UPsData::UPsData(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Parent(nullptr)
	, BroadcastInProgress(0)
	, bChanged(false)
{
	PsDataTools::FDataReflection::PreConstruct(this);
}

void UPsData::PostInitProperties()
{
	Super::PostInitProperties();

	PsDataTools::FDataReflection::PostConstruct(this);
	PsDataTools::FDataReflection::Fill(this);
}

void UPsData::DropHash()
{
	Hash.Reset();
	if (Parent.IsValid())
	{
		Parent->DropHash();
	}
}

void UPsData::CalculateHash() const
{
	struct HashBinarySerializer : public FPsDataBinarySerializer
	{
		HashBinarySerializer(TSharedRef<FPsDataOutputStream> InOutputStream)
			: FPsDataBinarySerializer(InOutputStream)
		{
		}

		void WriteValue(const UPsData* Value) override
		{
			if (Value == nullptr)
			{
				OutputStream->WriteUint8(static_cast<uint8>(EBinaryTokens::Value_null));
			}
			else
			{
				if (!Value->Hash.IsSet())
				{
					Value->CalculateHash();
				}

				const auto& Digest = Value->Hash->GetDigest();
				for (auto Byte : Digest)
				{
					OutputStream->WriteUint8(Byte);
				}
			}
		}
	};

	auto OutputStream = MakeShared<FPsDataMD5OutputStream>();
	HashBinarySerializer Serializer(OutputStream);
	DataSerializeInternal(&Serializer);
	Hash = OutputStream->GetHash();
}

void UPsData::InitProperties()
{
}

void UPsData::PostDeserialize()
{
}

/***********************************
 * Event system
 ***********************************/

bool UPsData::IsBound(const FString& Type, bool bBubbles) const
{
	const auto Find = Delegates.Find(Type);
	if (Find)
	{
		for (auto& Wrapper : *Find)
		{
			if (Wrapper->IsBound())
			{
				return true;
			}
		}
	}

	if (bBubbles && Parent.IsValid())
	{
		return Parent->IsBound(Type, bBubbles);
	}

	return false;
}

void UPsData::Broadcast(UPsDataEvent* Event) const
{
	Broadcast(Event, nullptr);
}

FPsDataBind UPsData::Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	return BindInternal(Type, Delegate);
}

FPsDataBind UPsData::Bind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	return BindInternal(Type, Delegate);
}

void UPsData::Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	UnbindInternal(Type, Delegate);
}

void UPsData::Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	UnbindInternal(Type, Delegate);
}

FPsDataBind UPsData::Bind(int32 FieldHash, const FPsDataDynamicDelegate& Delegate) const
{
	const TSharedPtr<const FDataField> Field = PsDataTools::FDataReflection::GetFieldByHash(GetClass(), FieldHash);
	check(Field.IsValid());
	return BindInternal(Field->GetChangedEventName(), Delegate);
}

FPsDataBind UPsData::Bind(int32 FieldHash, const FPsDataDelegate& Delegate) const
{
	const TSharedPtr<const FDataField> Field = PsDataTools::FDataReflection::GetFieldByHash(GetClass(), FieldHash);
	check(Field.IsValid());
	return BindInternal(Field->GetChangedEventName(), Delegate);
}

void UPsData::Unbind(int32 FieldHash, const FPsDataDynamicDelegate& Delegate) const
{
	const TSharedPtr<const FDataField> Field = PsDataTools::FDataReflection::GetFieldByHash(GetClass(), FieldHash);
	check(Field.IsValid());
	UnbindInternal(Field->GetChangedEventName(), Delegate);
}

void UPsData::Unbind(int32 FieldHash, const FPsDataDelegate& Delegate) const
{
	const TSharedPtr<const FDataField> Field = PsDataTools::FDataReflection::GetFieldByHash(GetClass(), FieldHash);
	check(Field.IsValid());
	UnbindInternal(Field->GetChangedEventName(), Delegate);
}

void UPsData::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	BindInternal(Type, Delegate);
}

void UPsData::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	UnbindInternal(Type, Delegate);
}

void UPsData::UpdateDelegates() const
{
	if (BroadcastInProgress > 0)
	{
		return;
	}

	for (auto MapIt = Delegates.CreateIterator(); MapIt; ++MapIt)
	{
		for (auto ArrayIt = MapIt->Value.CreateIterator(); ArrayIt; ++ArrayIt)
		{
			TSharedRef<FDelegateWrapper>& Wrapper = *ArrayIt;
			if (!Wrapper->DynamicDelegate.IsBound() && !Wrapper->Delegate.IsBound())
			{
				ArrayIt.RemoveCurrent();
			}
		}

		if (MapIt->Value.Num() == 0)
		{
			MapIt.RemoveCurrent();
		}
	}
}

void UPsData::Broadcast(UPsDataEvent* Event, const UPsData* Previous) const
{
	if (Event->Target == nullptr)
	{
		Event->Target = const_cast<UPsData*>(this);
	}

	if (FPsDataEventScopeGuard::IsGuarded())
	{
		Event->AddToRoot();
		FPsDataEventScopeGuard::AddCallback([WeakThis = MakeWeakObjectPtr(this), WeakPrevious = MakeWeakObjectPtr(Previous), Event]() {
			Event->RemoveFromRoot();
			if (WeakThis.IsValid())
			{
				WeakThis->BroadcastInternal(Event, WeakPrevious.Get());
			}
		});
	}
	else
	{
		BroadcastInternal(Event, Previous);
	}
}

void UPsData::BroadcastInternal(UPsDataEvent* Event, const UPsData* Previous) const
{
	++BroadcastInProgress;

	if (!Event->bStopImmediate)
	{
		const auto Find = Delegates.Find(Event->Type);
		if (Find)
		{
			TArray<TSharedRef<FDelegateWrapper>> Copy = *Find;
			for (auto& Wrapper : Copy)
			{
				bool bExecute = true;
				if (Wrapper->Field.IsValid())
				{
					bExecute = false;
					if (Previous == nullptr)
					{
						if (Event->Type == Wrapper->Field->GetChangedEventName())
						{
							bExecute = true;
						}
					}
					else if (Wrapper->Field->Context->IsContainer())
					{
						if (Wrapper->Field->Name == Previous->GetCollectionKey())
						{
							bExecute = true;
						}
					}
				}
				if (bExecute)
				{
					Wrapper->DynamicDelegate.ExecuteIfBound(Event);
					Wrapper->Delegate.ExecuteIfBound(Event);
					if (Event->bStopImmediate)
					{
						break;
					}
				}
			}
		}
	}

	if (!Event->bStop && Event->bBubbles)
	{
		if (Parent.IsValid())
		{
			Parent->BroadcastInternal(Event, this);
		}
	}

	--BroadcastInProgress;

	UpdateDelegates();
}

FPsDataBind UPsData::BindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (!Delegate.IsBound())
	{
		return {};
	}

	const TSharedRef<FDelegateWrapper> Ref(new FDelegateWrapper(Delegate, Field));
	Delegates.FindOrAdd(Type).Add(Ref);
	UpdateDelegates();

	return FPsDataBind(Ref);
}

FPsDataBind UPsData::BindInternal(const FString& Type, const FPsDataDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (!Delegate.IsBound())
	{
		return FPsDataBind();
	}

	const TSharedRef<FDelegateWrapper> Ref(new FDelegateWrapper(Delegate, Field));
	Delegates.FindOrAdd(Type).Add(Ref);
	UpdateDelegates();

	return FPsDataBind(Ref);
}

void UPsData::UnbindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (Delegate.IsBound())
	{
		const auto Find = Delegates.Find(Type);
		if (Find)
		{
			for (auto& Wrapper : *Find)
			{
				if (Wrapper->DynamicDelegate == Delegate && Wrapper->Field == Field)
				{
					Wrapper->DynamicDelegate.Unbind();
				}
			}
		}
	}

	UpdateDelegates();
}

void UPsData::UnbindInternal(const FString& Type, const FPsDataDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (Delegate.IsBound())
	{
		const auto Find = Delegates.Find(Type);
		if (Find)
		{
			for (auto& Wrapper : *Find)
			{
				if (Wrapper->Delegate.GetHandle() == Delegate.GetHandle() && Wrapper->Field == Field)
				{
					Wrapper->Delegate.Unbind();
				}
			}
		}
	}

	UpdateDelegates();
}

/***********************************
 * Serialize/Deserialize
 ***********************************/

void UPsData::DataSerialize(FPsDataSerializer* Serializer) const
{
	Serializer->WriteValue(this);
}

void UPsData::DataDeserialize(FPsDataDeserializer* Deserializer, bool bPatch)
{
	if (!bPatch)
	{
		Reset();
	}

	auto This = this;
	Deserializer->ReadValue(This, {});
	check(This);
}

void UPsData::DataSerializeInternal(FPsDataSerializer* Serializer) const
{
	for (auto& Pair : PsDataTools::FDataReflection::GetAliasFields(this->GetClass()))
	{
		Serializer->WriteKey(Pair.Key);
		Properties[Pair.Value->Index]->Serialize(this, Serializer);
		Serializer->PopKey(Pair.Key);
	}
}

void UPsData::DataDeserializeInternal(FPsDataDeserializer* Deserializer)
{
	const auto& Fields = PsDataTools::FDataReflection::GetAliasFields(this->GetClass());
	FString Key;
	while (Deserializer->ReadKey(Key))
	{
		if (const auto Find = Fields.Find(Key))
		{
			auto& Field = *Find;
			Properties[Field->Index]->Deserialize(this, Deserializer);
		}
		else
		{
			UE_LOG(LogData, Warning, TEXT("Property \"%s\" not found in \"%s\""), *Key, *GetClass()->GetName())
		}
		Deserializer->PopKey(Key);
	}

	if (bChanged)
	{
		PostDeserialize();
	}
}

/***********************************
 * Data property
 ***********************************/

const FString& UPsData::GetDataKey() const
{
	return DataKey;
}

const FString& UPsData::GetCollectionKey() const
{
	return CollectionKey;
}

UPsData* UPsData::GetParent() const
{
	return Parent.Get();
}

UPsDataRoot* UPsData::GetRoot() const
{
	UPsData* Root = const_cast<UPsData*>(this);
	while (Root)
	{
		if (Root->IsA(UPsDataRoot::StaticClass()))
		{
			return static_cast<UPsDataRoot*>(Root);
		}

		Root = Root->Parent.Get();
	}

	return nullptr;
}

bool UPsData::HasRoot() const
{
	return GetRoot() != nullptr;
}

FString UPsData::GetHash() const
{
	if (!Hash.IsSet())
	{
		CalculateHash();
	}

	return Hash.GetValue().ToString();
}

FString UPsData::GetPathFromRoot() const
{
	const UPsData* Current = this;
	FString Path;
	do
	{
		const FString& CollectionName = Current->GetCollectionKey();
		if (CollectionName.IsEmpty())
		{
			if (Path.IsEmpty())
			{
				Path = Current->GetDataKey();
			}
			else
			{
				Path = Current->GetDataKey() + TEXT(".") + Path;
			}
		}
		else
		{
			if (Path.IsEmpty())
			{
				Path = CollectionName + TEXT(".") + Current->GetDataKey();
			}
			else
			{
				Path = CollectionName + TEXT(".") + Current->GetDataKey() + TEXT(".") + Path;
			}
		}
	} while ((Current = Current->GetParent()) != nullptr);
	return Path;
}

/***********************************
 * Utility
 ***********************************/

void UPsData::Reset()
{
	for (const auto& Pair : PsDataTools::FDataReflection::GetFields(GetClass()))
	{
		const auto& Field = Pair.Value;
		Properties[Field->Index]->Reset(this);
	}

	InitProperties();
	PostDeserialize();
}

UPsData* UPsData::Copy() const
{
	auto OutputStream = MakeShared<FPsDataBufferOutputStream>();
	FPsDataBinarySerializer Serializer(OutputStream);
	DataSerialize(&Serializer);
	UPsData* Copy = NewObject<UPsData>(GetTransientPackage(), GetClass());
	FPsDataBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(OutputStream->GetBuffer()));
	Copy->DataDeserialize(&Deserializer);
	return Copy;
}

TArray<FPsDataReport> UPsData::Validation() const
{
	TArray<FPsDataReport> Result;

	//TODO: PS-136
	UPsData* Data = const_cast<UPsData*>(this);
	UPsData* RootData = Data->GetRoot();
	const FString Path = GetPathFromRoot();

	for (auto& Pair : PsDataTools::FDataReflection::GetLinks(GetClass()))
	{
		const FString FieldPath = Path + TEXT(".") + Pair.Value->Name;

		if (Pair.Value->bAbstract)
		{
			Result.Add(FPsDataReport(EPsDataReportType::Logic, FieldPath, TEXT("Used abstract property")));
		}
		else
		{
			TArray<FString> Keys;
			UPsDataFunctionLibrary::GetLinkKeys(Data, Pair.Value, Keys);
			TMap<FString, UPsData*>* MapPtr = nullptr;
			const FString& LinkPath = UPsDataFunctionLibrary::GetLinkPath(Data, Pair.Value);
			if (PsDataTools::GetByPath(RootData, LinkPath, MapPtr))
			{
				TMap<FString, UPsData*> Map = *MapPtr;
				for (const FString& Key : Keys)
				{
					if (Key.Len() > 0)
					{
						UPsData** Find = Map.Find(Key);
						if (Find == nullptr)
						{
							Result.Add(FPsDataReport(EPsDataReportType::Link, FieldPath, TEXT("Property not found"), LinkPath + TEXT(".") + Key));
						}
					}
					else if (!Pair.Value->Meta.bNullable)
					{
						Result.Add(FPsDataReport(EPsDataReportType::Link, FieldPath, TEXT("Property is empty"), LinkPath + TEXT(".?")));
					}
				}
			}
			else
			{
				Result.Add(FPsDataReport(EPsDataReportType::Logic, LinkPath, TEXT("Used undeclared property")));
			}
		}
	}

	for (auto& Pair : PsDataTools::FDataReflection::GetFields(this->GetClass()))
	{
		if (Pair.Value->Context->IsData())
		{
			if (Pair.Value->Context->IsArray())
			{
				TArray<UPsData*>* Value = nullptr;
				if (PsDataTools::GetByField(Data, Pair.Value, Value))
				{
					for (UPsData* Element : *Value)
					{
						if (Element)
						{
							Result.Append(Element->Validation());
						}
					}
				}
			}
			else if (Pair.Value->Context->IsMap())
			{
				TMap<FString, UPsData*>* Value = nullptr;
				if (PsDataTools::GetByField(Data, Pair.Value, Value))
				{
					for (const auto& ElementPair : *Value)
					{
						if (ElementPair.Value)
						{
							Result.Append(ElementPair.Value->Validation());
						}
					}
				}
			}
			else
			{
				UPsData** Value = nullptr;
				if (PsDataTools::GetByField(Data, Pair.Value, Value))
				{
					if ((*Value))
					{
						Result.Append((*Value)->Validation());
					}
				}
			}
		}
	}

	return Result;
}
