// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "PsData.h"

#include "PsDataCore.h"
#include "PsDataMemory.h"
#include "Serialize/PsDataBinarySerialization.h"

#include "Async/Async.h"
#include "Core/Public/Misc/SecureHash.h"

/***********************************
* PsData friend
***********************************/

namespace FDataReflectionTools
{
void FPsDataFriend::ChangeDataName(UPsData* Data, const FString& Name)
{
	if (Data->DataKey != Name)
	{
		Data->DataKey = Name;
		Data->Broadcast(UPsDataEvent::ConstructEvent(TEXT("NameChanged"), false));
	}
}

void FPsDataFriend::AddChild(UPsData* Parent, UPsData* Data)
{
	if (Data->Parent != nullptr)
	{
		UE_LOG(LogData, Fatal, TEXT("Child already added"));
		return;
	}

	Data->Parent = Parent;
	Parent->Children.Add(Data);
	Data->Broadcast(UPsDataEvent::ConstructEvent(TEXT("Added"), true));
}

void FPsDataFriend::RemoveChild(UPsData* Parent, UPsData* Data)
{
	if (Data->Parent != Parent)
	{
		UE_LOG(LogData, Fatal, TEXT("Child not added"));
		return;
	}

	Data->Broadcast(UPsDataEvent::ConstructEvent(TEXT("Removing"), true));
	Parent->Children.Remove(Data);
	Data->Parent = nullptr;
}

bool FPsDataFriend::IsChanged(UPsData* Data)
{
	return Data->bChanged;
}

void FPsDataFriend::SetIsChanged(UPsData* Data, bool NewValue)
{
	Data->bChanged = NewValue;
}

TArray<TUniquePtr<FAbstractDataMemory>>& FPsDataFriend::GetMemory(UPsData* Data)
{
	return Data->Memory;
}
} // namespace FDataReflectionTools

/***********************************
* PSDATA!
***********************************/

UPsData::UPsData(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, DataKey()
	, Parent(nullptr)
	, BroadcastInProgress(0)
	, bChanged(false)
{
	FDataReflection::AddToQueue(this);
}

void UPsData::PostInitProperties()
{
	Super::PostInitProperties();
	FDataReflection::RemoveFromQueue(this);
	FDataReflection::Fill(this);
}

/***********************************
 * Event system
 ***********************************/

void UPsData::Broadcast(UPsDataEvent* Event) const
{
	const bool bDeferredEventProcessing = false;
	if (bDeferredEventProcessing)
	{
		TWeakObjectPtr<UPsData> WeakPtr(const_cast<UPsData*>(this));
		Event->AddToRoot();
		AsyncTask(ENamedThreads::GameThread, [WeakPtr, Event]() {
			Event->RemoveFromRoot();
			if (WeakPtr.IsValid())
			{
				WeakPtr->BroadcastInternal(Event);
			}
		});
	}
	else
	{
		BroadcastInternal(Event);
	}
}

void UPsData::Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	BindInternal(Type, Delegate);
}

void UPsData::Bind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	BindInternal(Type, Delegate);
}

void UPsData::Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const
{
	UnbindInternal(Type, Delegate);
}

void UPsData::Unbind(const FString& Type, const FPsDataDelegate& Delegate) const
{
	UnbindInternal(Type, Delegate);
}

void UPsData::Bind(int32 Hash, const FPsDataDynamicDelegate& Delegate) const
{
	TSharedPtr<const FDataField> Field = FDataReflection::GetFieldByHash(GetClass(), Hash);
	check(Field.IsValid());
	BindInternal(Field->GenerateChangePropertyEventName(), Delegate);
}

void UPsData::Bind(int32 Hash, const FPsDataDelegate& Delegate) const
{
	TSharedPtr<const FDataField> Field = FDataReflection::GetFieldByHash(GetClass(), Hash);
	check(Field.IsValid());
	BindInternal(Field->GenerateChangePropertyEventName(), Delegate);
}

void UPsData::Unbind(int32 Hash, const FPsDataDynamicDelegate& Delegate) const
{
	TSharedPtr<const FDataField> Field = FDataReflection::GetFieldByHash(GetClass(), Hash);
	check(Field.IsValid());
	UnbindInternal(Field->GenerateChangePropertyEventName(), Delegate);
}

void UPsData::Unbind(int32 Hash, const FPsDataDelegate& Delegate) const
{
	TSharedPtr<const FDataField> Field = FDataReflection::GetFieldByHash(GetClass(), Hash);
	check(Field.IsValid());
	UnbindInternal(Field->GenerateChangePropertyEventName(), Delegate);
}

void UPsData::BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	BindInternal(Type, Delegate);
}

void UPsData::BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate)
{
	BindInternal(Type, Delegate);
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
			const FDelegateWrapper& Wrapper = *ArrayIt;
			if (!Wrapper.DynamicDelegate.IsBound() && !Wrapper.Delegate.IsBound())
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

void UPsData::BroadcastInternal(UPsDataEvent* Event, UClass* Previous) const
{
	++BroadcastInProgress;

	if (Event->Target == nullptr)
	{
		Event->Target = const_cast<UPsData*>(this);
	}

	if (!Event->bStopImmediate)
	{
		auto Find = Delegates.Find(Event->Type);
		if (Find)
		{
			TArray<FDelegateWrapper> Copy = *Find;
			for (FDelegateWrapper& Wrapper : Copy)
			{
				bool bExecute = true;
				if (Wrapper.Field.IsValid())
				{
					bExecute = false;
					if (Previous == nullptr)
					{
						if (Event->Type == Wrapper.Field->GenerateChangePropertyEventName())
						{
							bExecute = true;
						}
					}
					else if (Wrapper.Field->Context->IsData())
					{
						if (Wrapper.Field->Context->GetUE4Type() == Previous)
						{
							bExecute = true;
						}
					}
				}
				if (bExecute)
				{
					Wrapper.DynamicDelegate.ExecuteIfBound(Event);
					Wrapper.Delegate.ExecuteIfBound(Event);
					if (Event->bStopImmediate)
					{
						break;
					}
				}
			}
		}
	}

	if (!Event->bStop && Event->bBubbles && Parent)
	{
		Parent->BroadcastInternal(Event, GetClass());
	}

	--BroadcastInProgress;

	UpdateDelegates();
}

void UPsData::BindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (!Delegate.IsBound())
	{
		return;
	}

	Delegates.FindOrAdd(Type).Add(FDelegateWrapper(Delegate, Field));
	UpdateDelegates();
}

void UPsData::BindInternal(const FString& Type, const FPsDataDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (!Delegate.IsBound())
	{
		return;
	}

	Delegates.FindOrAdd(Type).Add(FDelegateWrapper(Delegate, Field));
	UpdateDelegates();
}

void UPsData::UnbindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, TSharedPtr<const FDataField> Field) const
{
	if (Delegate.IsBound())
	{
		auto Find = Delegates.Find(Type);
		if (Find)
		{
			for (FDelegateWrapper& Wrapper : *Find)
			{
				if (Wrapper.DynamicDelegate == Delegate && Wrapper.Field == Field)
				{
					Wrapper.DynamicDelegate.Unbind();
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
		auto Find = Delegates.Find(Type);
		if (Find)
		{
			for (FDelegateWrapper& Wrapper : *Find)
			{
				if (Wrapper.Delegate.GetHandle() == Delegate.GetHandle() && Wrapper.Field == Field)
				{
					Wrapper.Delegate.Unbind();
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
	for (auto& Pair : FDataReflection::GetFields(this->GetClass()))
	{
		Memory[Pair.Value->Index]->Serialize(this, Pair.Value, Serializer);
	}
}

void UPsData::DataDeserialize(FPsDataDeserializer* Deserializer)
{
	for (auto& Pair : FDataReflection::GetFields(this->GetClass()))
	{
		if (Deserializer->Has(Pair.Value))
		{
			Memory[Pair.Value->Index]->Deserialize(this, Pair.Value, Deserializer);
		}
	}
}

/***********************************
 * Data property
 ***********************************/

FString UPsData::GetDataKey() const
{
	return DataKey;
}

UPsData* UPsData::GetParent() const
{
	return Parent;
}

UPsData* UPsData::GetRoot() const
{
	UPsData* Root = const_cast<UPsData*>(this);
	while (Root->Parent != nullptr)
	{
		Root = Root->Parent;
	}
	return Root;
}

FString UPsData::GetHash() const
{
	//TODO: Sort keys
	//TODO: Streaming
	FPsDataBinarySerializer Serializer;
	DataSerialize(&Serializer);

	uint8 Digest[16];
	FMD5 Md5Gen;
	Md5Gen.Update((uint8*)Serializer.GetBuffer().GetData(), Serializer.GetBuffer().Num());
	Md5Gen.Final(Digest);

	return FString::Printf(
		TEXT("%08x%08x%08x%08"),
		(static_cast<uint32>(Digest[0]) << 24) | (static_cast<uint32>(Digest[1]) << 16) | (static_cast<uint32>(Digest[2]) << 8) | static_cast<uint32>(Digest[3]),
		(static_cast<uint32>(Digest[4]) << 24) | (static_cast<uint32>(Digest[5]) << 16) | (static_cast<uint32>(Digest[6]) << 8) | static_cast<uint32>(Digest[7]),
		(static_cast<uint32>(Digest[8]) << 24) | (static_cast<uint32>(Digest[9]) << 16) | (static_cast<uint32>(Digest[10]) << 8) | static_cast<uint32>(Digest[11]),
		(static_cast<uint32>(Digest[12]) << 24) | (static_cast<uint32>(Digest[13]) << 16) | (static_cast<uint32>(Digest[14]) << 8) | static_cast<uint32>(Digest[15]));
}

void UPsData::Reset()
{
	for (const auto& Pair : FDataReflection::GetFields(GetClass()))
	{
		const auto& Field = Pair.Value;
		Memory[Field->Index]->Reset(this, Field);
	}
}
