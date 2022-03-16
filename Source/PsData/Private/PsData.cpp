// Copyright 2015-2022 MY.GAMES. All Rights Reserved.

#include "PsData.h"

#include "PsDataCore.h"
#include "PsDataDeferredTask.h"
#include "PsDataRoot.h"
#include "Serialize/PsDataBinarySerialization.h"
#include "Serialize/Stream/PsDataBufferInputStream.h"
#include "Serialize/Stream/PsDataBufferOutputStream.h"
#include "Serialize/Stream/PsDataMD5OutputStream.h"
#include "Types/PsData_UPsData.h"

#include "Async/Async.h"

FSimpleMulticastDelegate FDataDelegates::OnPostDataModuleInit;
TPsDataSimplePromise FDataDelegates::PostDataModuleInitPromise;

/***********************************
 * PsData friend
 ***********************************/

namespace PsDataTools
{
void FPsDataFriend::ChangeDataName(UPsData* Data, const FString& Name, const FString& CollectionName)
{
	Data->ChangeName(Name, CollectionName);
}

void FPsDataFriend::AddChild(UPsData* Parent, UPsData* Data)
{
	Parent->AddChild(Data);
}

void FPsDataFriend::RemoveChild(UPsData* Parent, UPsData* Data)
{
	Parent->RemoveChild(Data);
}

void FPsDataFriend::Changed(UPsData* Data, const FDataField* Field)
{
	Data->Changed(Field);
}

void FPsDataFriend::InitProperties(UPsData* Data)
{
	Data->InitProperties();
	Data->PostDeserialize();
}

void FPsDataFriend::InitStructProperties(UPsData* Data)
{
	Data->InitStructProperties();
}

TArray<FAbstractDataProperty*>& FPsDataFriend::GetProperties(UPsData* Data)
{
	return Data->Properties;
}

FAbstractDataProperty* FPsDataFriend::GetProperty(UPsData* Data, int32 Index)
{
	return Data->Properties[Index];
}

const FAbstractDataProperty* FPsDataFriend::GetProperty(const UPsData* Data, int32 Index)
{
	return Data->Properties[Index];
}

TArray<FAbstractDataLinkProperty*>& FPsDataFriend::GetLinks(UPsData* Data)
{
	return Data->Links;
}

FAbstractDataLinkProperty* FPsDataFriend::GetLink(UPsData* Data, int32 Index)
{
	return Data->Links[Index];
}

const FAbstractDataLinkProperty* FPsDataFriend::GetLink(const UPsData* Data, int32 Index)
{
	return Data->Links[Index];
}

void FPsDataFriend::Serialize(const UPsData* Data, FPsDataSerializer* Serializer)
{
	Data->DataSerializeInternal(Serializer);
}

void FPsDataFriend::Deserialize(UPsData* Data, FPsDataDeserializer* Deserializer)
{
	Data->DataDeserializeInternal(Deserializer);
}

const FPsDataImprint& FPsDataFriend::GetImprint(const UPsData* Data)
{
	Data->CalculateImprint();
	return Data->Imprint;
}

const TSet<UPsData*>& FPsDataFriend::GetChildren(const UPsData* Data)
{
	return Data->Children;
}

FPsDataBind FPsDataFriend::BindInternal(const UPsData* Data, const FString& Type, const FPsDataDynamicDelegate& Delegate, EDataBindFlags Flags, const FDataField* Field)
{
	return Data->BindInternal(Type, Delegate, Flags, Field);
}

FPsDataBind FPsDataFriend::BindInternal(const UPsData* Data, const FString& Type, const FPsDataDelegate& Delegate, EDataBindFlags Flags, const FDataField* Field)
{
	return Data->BindInternal(Type, Delegate, Flags, Field);
}

void FPsDataFriend::UnbindInternal(const UPsData* Data, const FString& Type, const FPsDataDynamicDelegate& Delegate, const FDataField* Field)
{
	Data->UnbindInternal(Type, Delegate, Field);
}

void FPsDataFriend::UnbindInternal(const UPsData* Data, const FString& Type, const FPsDataDelegate& Delegate, const FDataField* Field)
{
	Data->UnbindInternal(Type, Delegate, Field);
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
	for (const auto& DelegateWrapper : Collection)
	{
		DelegateWrapper->Delegate.Unbind();
		DelegateWrapper->DynamicDelegate.Unbind();
	}
	Collection.Reset();
}

/***********************************
 * PSDATA!
 ***********************************/

UPsData::UPsData(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, Parent(nullptr)
	, Root(nullptr)
	, BroadcastInProgress(0)
	, bChanged(false)
	, SerializeBufferSize(1024)
	, ClassFields(nullptr)
{
	if (HasAnyFlags(RF_ClassDefaultObject | RF_DefaultSubObject))
	{
		PsDataTools::FDataReflection::PreConstruct(GetClass());
	}
	else
	{
		ClassFields = PsDataTools::FDataReflection::GetFieldsByClass(GetClass());
		Properties.Reset(ClassFields->GetNumFields());
		Links.Reset(ClassFields->GetNumLinks());
	}
}

void UPsData::PostInitProperties()
{
	Super::PostInitProperties();

	if (HasAnyFlags(RF_ClassDefaultObject | RF_DefaultSubObject))
	{
		PsDataTools::FDataReflection::PostConstruct(GetClass());
	}
	else
	{
		for (const auto Property : Properties)
		{
			if (Property->GetField()->Meta.bStrict)
			{
				Property->Allocate();
			}
		}

		InitProperties();
		PostDeserialize();
	}
}

void UPsData::AddChild(UPsData* Child)
{
	if (Child->Parent)
	{
		UE_LOG(LogData, Fatal, TEXT("Child already added"));
		return;
	}

	Child->Parent = this;
	Children.Add(Child);
	Child->AddToRootData();

	Child->DropImprint();

	if (Child->IsBound(UPsDataEvent::AddedToParent, false))
	{
		Child->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::AddedToParent, false));
	}

	if (Child->IsBound(UPsDataEvent::Added, true))
	{
		const auto Event = UPsDataEvent::ConstructEvent(UPsDataEvent::Added, false);
		Child->Broadcast(Event);

		const auto BubbleEvent = UPsDataEvent::ConstructEvent(UPsDataEvent::Added, true);
		BubbleEvent->Target = Child;
		BubbleEvent->ParentEvent = Event;
		Broadcast(BubbleEvent, Child);
	}
}

void UPsData::RemoveChild(UPsData* Child)
{
	if (Child->Parent != this)
	{
		UE_LOG(LogData, Fatal, TEXT("Child not added"));
		return;
	}

	const auto bIsBound = Child->IsBound(UPsDataEvent::Removed, true);

	Children.Remove(Child);
	Child->Parent = nullptr;
	Child->RemoveFromRootData();

	Child->DropImprint();

	if (Child->IsBound(UPsDataEvent::RemovedFromParent, false))
	{
		Child->Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::RemovedFromParent, false));
	}

	if (bIsBound)
	{
		const auto Event = UPsDataEvent::ConstructEvent(UPsDataEvent::Removed, false);
		Child->Broadcast(Event);

		const auto BubbleEvent = UPsDataEvent::ConstructEvent(UPsDataEvent::Removed, true);
		BubbleEvent->Target = Child;
		BubbleEvent->ParentEvent = Event;
		Broadcast(BubbleEvent, Child);
	}
}

void UPsData::ChangeName(const FString& Name, const FString& CollectionName)
{
	if (DataKey != Name || CollectionKey != CollectionName)
	{
		FullKey = CollectionName.IsEmpty() ? Name : (CollectionName + TEXT(".") + Name);
		DataKey = Name;
		CollectionKey = CollectionName;

		if (IsBound(UPsDataEvent::NameChanged, false))
		{
			Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::NameChanged, false));
		}
	}
}

void UPsData::Changed(const FDataField* Field)
{
	DropImprint();

	const auto& EventName = Field->GetChangedEventName();
	if (Field->Meta.bEvent)
	{
		if (IsBound(EventName, Field->Meta.bBubbles))
		{
			Broadcast(UPsDataEvent::ConstructEvent(EventName, Field->Meta.bBubbles));
		}
	}
	else if (IsBoundWithFlag(EventName, EDataBindFlags::IgnoreFieldMeta, false))
	{
		Broadcast(UPsDataEvent::ConstructEvent(EventName, false));
	}

	if (!bChanged)
	{
		bChanged = true;
		PsDataTools::DeferredTask(this, [this]() {
			bChanged = false;
			if (IsBound(UPsDataEvent::Changed, false))
			{
				Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::Changed, false));
			}
		});
	}
}

void UPsData::AddToRootData()
{
	if (Parent->Root)
	{
		Root = Parent->Root;

		if (IsBound(UPsDataEvent::AddedToRoot, false))
		{
			Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::AddedToRoot, false));
		}

		for (UPsData* Child : Children)
		{
			Child->AddToRootData();
		}
	}
}

void UPsData::RemoveFromRootData()
{
	if (Root)
	{
		Root = nullptr;

		if (IsBound(UPsDataEvent::RemovedFromRoot, false))
		{
			Broadcast(UPsDataEvent::ConstructEvent(UPsDataEvent::RemovedFromRoot, false));
		}

		for (UPsData* Child : Children)
		{
			Child->RemoveFromRootData();
		}
	}
}

void UPsData::DropImprint() const
{
	Imprint.Reset();
	DropHash();
}

void UPsData::DropHash() const
{
	Hash.Reset();
	if (Parent && Parent->Hash.IsSet())
	{
		Parent->DropHash();
	}
}

void UPsData::CalculateImprint() const
{
	if (!Imprint.IsSet())
	{
		FPsDataImprintBinarySerializer Serializer(&Imprint);
		Serializer.WriteObject();
		DataSerializeInternal(&Serializer);
		Serializer.PopObject();

		Imprint.Prepare();
	}
}

void UPsData::CalculateHash() const
{
	if (!Hash.IsSet())
	{
		CalculateImprint();
		if (Imprint.HasChildren())
		{
			for (const auto& Child : Imprint.GetChildren())
			{
				Child.GetData()->CalculateHash();
			}

			FPsDataMD5OutputStream OutputStream;

			uint64 A, B;
			Imprint.GetHash().GetDigest(A, B);
			OutputStream.WriteUint64(A);
			OutputStream.WriteUint64(B);

			for (const auto& Child : Imprint.GetChildren())
			{
				Child.GetData()->Hash->GetDigest(A, B);
				OutputStream.WriteUint64(A);
				OutputStream.WriteUint64(B);
			}

			Hash = OutputStream.GetHash();
		}
		else
		{
			Hash = Imprint.GetHash();
		}
	}
}

void UPsData::InitProperties()
{
}

void UPsData::PostDeserialize()
{
}

void UPsData::InitStructProperties()
{
}

/***********************************
 * Event system
 ***********************************/

bool UPsData::IsBound(const FString& EventType, bool bBubbles) const
{
	if (const auto Find = Delegates.Find(EventType))
	{
		for (const auto& Wrapper : *Find)
		{
			if (Wrapper->IsBound())
			{
				return true;
			}
		}
	}

	if (bBubbles && Parent)
	{
		return Parent->IsBound(EventType, bBubbles);
	}

	return false;
}

bool UPsData::IsBoundWithFlag(const FString& EventType, EDataBindFlags Flags, bool bBubbles) const
{
	if (const auto Find = Delegates.Find(EventType))
	{
		for (const auto& Wrapper : *Find)
		{
			if (static_cast<uint8>(Wrapper->Flags & Flags) != 0 && Wrapper->IsBound())
			{
				return true;
			}
		}
	}

	if (bBubbles && Parent)
	{
		return Parent->IsBoundWithFlag(EventType, Flags, bBubbles);
	}

	return false;
}

void UPsData::Broadcast(UPsDataEvent* Event) const
{
	Broadcast(Event, nullptr);
}

FPsDataBind UPsData::Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate, EDataBindFlags Flags) const
{
	return BindInternal(Type, Delegate, Flags);
}

FPsDataBind UPsData::Bind(const FString& Type, const FPsDataDelegate& Delegate, EDataBindFlags Flags) const
{
	return BindInternal(Type, Delegate, Flags);
}

void UPsData::Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	UnbindInternal(Type, Delegate);
}

void UPsData::Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	UnbindInternal(Type, Delegate);
}

void UPsData::UnbindAll(UObject* Object) const
{
	UnbindAllInternal(Object);
}

void UPsData::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate, bool bNonDeferred)
{
	EDataBindFlags Flags = EDataBindFlags::Default;
	if (bNonDeferred)
	{
		Flags = Flags | EDataBindFlags::NonDeferred;
	}
	BindInternal(Type, Delegate, Flags);
}

void UPsData::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	UnbindInternal(Type, Delegate);
}

void UPsData::BlueprintUnbindAll(UObject* Object)
{
	UnbindAllInternal(Object);
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
			const auto& Wrapper = *ArrayIt;
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
		BroadcastInternal(Event, Previous, EDataBroadcastPass::NonDeferred);

		Event->AddToRoot();
		FPsDataEventScopeGuard::AddCallback([WeakThis = MakeWeakObjectPtr(this), WeakPrevious = MakeWeakObjectPtr(Previous), Event]() {
			Event->RemoveFromRoot();
			if (WeakThis.IsValid())
			{
				WeakThis->BroadcastInternal(Event, WeakPrevious.Get(), EDataBroadcastPass::Deferred);
			}
		});
	}
	else
	{
		BroadcastInternal(Event, Previous, EDataBroadcastPass::Default);
	}
}

void UPsData::BroadcastInternal(UPsDataEvent* Event, const UPsData* Previous, EDataBroadcastPass Pass) const
{
	++BroadcastInProgress;

	if (!Event->IsStoppedImmediately())
	{
		TArray<TSharedRef<FDelegateWrapper>> Copy;
		if (const auto Find = Delegates.Find(Event->Type))
		{
			Copy.Append(*Find);
			for (const auto& Wrapper : Copy)
			{
				if (Pass != EDataBroadcastPass::Default)
				{
					const bool bNonDeferredDelegate = static_cast<uint8>(Wrapper->Flags & EDataBindFlags::NonDeferred) != 0;
					if (bNonDeferredDelegate)
					{
						if (Pass == EDataBroadcastPass::Deferred)
						{
							continue;
						}
					}
					else
					{
						if (Pass == EDataBroadcastPass::NonDeferred)
						{
							continue;
						}
					}
				}

				bool bExecute = true;
				if (Wrapper->Field)
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
					if (Event->IsStoppedImmediately())
					{
						break;
					}
				}
			}
			Copy.Reset();
		}
	}

	if (!Event->IsStopped() && Event->bBubbles)
	{
		if (Parent)
		{
			Parent->BroadcastInternal(Event, this, Pass);
		}
	}

	--BroadcastInProgress;

	UpdateDelegates();
}

FPsDataBind UPsData::BindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, EDataBindFlags Flags, const FDataField* Field) const
{
	if (!Delegate.IsBound())
	{
		return {};
	}

	const TSharedRef<FDelegateWrapper> Ref(new FDelegateWrapper(Delegate, Flags, Field));
	Delegates.FindOrAdd(Type).Add(Ref);
	UpdateDelegates();

	return FPsDataBind(Ref);
}

FPsDataBind UPsData::BindInternal(const FString& Type, const FPsDataDelegate& Delegate, EDataBindFlags Flags, const FDataField* Field) const
{
	if (!Delegate.IsBound())
	{
		return {};
	}

	const TSharedRef<FDelegateWrapper> Ref(new FDelegateWrapper(Delegate, Flags, Field));
	Delegates.FindOrAdd(Type).Add(Ref);
	UpdateDelegates();

	return FPsDataBind(Ref);
}

void UPsData::UnbindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, const FDataField* Field) const
{
	if (Delegate.IsBound())
	{
		if (const auto Find = Delegates.Find(Type))
		{
			for (const auto& Wrapper : *Find)
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

void UPsData::UnbindInternal(const FString& Type, const FPsDataDelegate& Delegate, const FDataField* Field) const
{
	if (Delegate.IsBound())
	{
		if (const auto Find = Delegates.Find(Type))
		{
			for (const auto& Wrapper : *Find)
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

void UPsData::UnbindAllInternal(UObject* Object) const
{
	for (auto& Pair : Delegates)
	{
		for (auto& Wrapper : Pair.Value)
		{
			if (Wrapper->DynamicDelegate.GetUObject() == Object)
			{
				Wrapper->DynamicDelegate.Unbind();
			}
			if (Wrapper->Delegate.GetUObject() == Object)
			{
				Wrapper->DynamicDelegate.Unbind();
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

void UPsData::DataSerializeAsync(FPsDataSerializer* Serializer, FPsDataAsyncSerializeDelegate CallbackDelegate) const
{
	auto OutputStream = MakeShared<FPsDataBufferOutputStream>();
	OutputStream->Reserve(SerializeBufferSize * 2);

	const auto StartOffset = FPsDataImprintBinarySerializer::Concatenate(OutputStream, this);
	SerializeBufferSize = FMath::Max(SerializeBufferSize, OutputStream->Size());

	auto WeakThis = MakeWeakObjectPtr(this);
	AsyncTask(ENamedThreads::AnyHiPriThreadHiPriTask, [WeakThis, OutputStream, StartOffset, Serializer, CallbackDelegate]() {
		const auto InputStream = MakeShared<FPsDataBufferInputStream>(OutputStream->GetBuffer());
		FPsDataImprintBinaryDeserializer ImprintDeserializer(InputStream, StartOffset);
		FPsDataImprintBinaryConvertor Convertor(&ImprintDeserializer);
		Convertor.Convert(Serializer);
		AsyncTask(ENamedThreads::GameThread, [WeakThis, CallbackDelegate]() {
			if (WeakThis.IsValid())
			{
				CallbackDelegate.ExecuteIfBound();
			}
		});
	});
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
	for (const auto Property : Properties)
	{
		const auto Field = Property->GetField();
		if ((Field->Meta.bDefault && !Serializer->bWriteDefaults && Property->IsDefault()) || Field->Meta.bHidden)
		{
			continue;
		}

		const auto& Key = Field->GetNameForSerialize();
		Serializer->WriteKey(Key);
		Property->Serialize(Serializer);
		Serializer->PopKey(Key);
	}
}

void UPsData::DataDeserializeInternal(FPsDataDeserializer* Deserializer)
{
	FString Key;
	while (Deserializer->ReadKey(Key))
	{
		if (const auto Field = ClassFields->GetFieldByAlias(Key))
		{
			Properties[Field->Index]->Deserialize(Deserializer);
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

const FString& UPsData::GetFullDataKey() const
{
	return FullKey;
}

const FString& UPsData::GetDataKey() const
{
	return DataKey;
}

const FString& UPsData::GetCollectionKey() const
{
	return CollectionKey;
}

void UPsData::GetPathFromRoot(FString& OutPath) const
{
	if (Parent)
	{
		Parent->GetPathFromRoot(OutPath);
		if (OutPath.Len() > 0)
		{
			OutPath.AppendChar('.');
		}
	}

	OutPath.Append(GetFullDataKey());
}

FString UPsData::GetPathFromRoot() const
{
	FString Result;
	GetPathFromRoot(Result);
	return Result;
}

UPsData* UPsData::GetParent() const
{
	return Parent;
}

UPsDataRoot* UPsData::GetRoot() const
{
	return Root;
}

bool UPsData::HasRoot() const
{
	return GetRoot() != nullptr;
}

FString UPsData::GetHash() const
{
	CalculateHash();
	return Hash.GetValue().ToString();
}

bool UPsData::InCollection() const
{
	return !CollectionKey.IsEmpty();
}

/***********************************
 * Utility
 ***********************************/

void UPsData::Reset()
{
	for (const auto Property : Properties)
	{
		Property->Reset();
	}

	InitProperties();
	PostDeserialize();
}

UPsData* UPsData::Copy() const
{
	auto OutputStream = MakeShared<FPsDataBufferOutputStream>();
	OutputStream->Reserve(SerializeBufferSize * 2);

	const auto StartOffset = FPsDataImprintBinarySerializer::Concatenate(OutputStream, this);
	SerializeBufferSize = FMath::Max(SerializeBufferSize, OutputStream->Size());

	UPsData* Copy = NewObject<UPsData>(GetTransientPackage(), GetClass());
	FPsDataImprintBinaryDeserializer Deserializer(MakeShared<FPsDataBufferInputStream>(OutputStream->GetBuffer()), StartOffset);
	Copy->DataDeserialize(&Deserializer);
	return Copy;
}

void UPsData::Validation(TArray<FString>& OutResult) const
{
	for (const auto LinkProperty : Links)
	{
		LinkProperty->Validate(OutResult);
	}

	for (const auto Child : Children)
	{
		Child->Validation(OutResult);
	}
}

TArray<FString> UPsData::Validation() const
{
	TArray<FString> Result;
	Validation(Result);
	return Result;
}