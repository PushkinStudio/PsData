// Copyright 2015-2018 Mail.Ru Group. All Rights Reserved.

#include "PsData.h"
#include "PsDataCore.h"
#include "Async/Async.h"

DEFINE_LOG_CATEGORY(LogData);

namespace FDataReflectionTools
{
	void FPsDataFriend::ChangeDataName(class UPsData* Data, const FString& Name)
	{
		if (Data->Name != Name)
		{
			Data->Name = Name;
			Data->Broadcast(UPsEvent::ConstructEvent(TEXT("NameChanged"), false));
		}
	}
	
	void FPsDataFriend::AddChild(class UPsData* Parent, class UPsData* Data)
	{
		if (Data->Parent != nullptr)
		{
			UE_LOG(LogData, Fatal, TEXT("Child already added"));
			return;
		}
		
		Data->Parent = Parent;
		Parent->Children.Add(Data);
		Data->Broadcast(UPsEvent::ConstructEvent(TEXT("Added"), false));
	}
	
	void FPsDataFriend::RemoveChild(class UPsData* Parent, class UPsData* Data)
	{
		if (Data->Parent != Parent)
		{
			UE_LOG(LogData, Fatal, TEXT("Child not added"));
			return;
		}
		
		Data->Parent = nullptr;
		Parent->Children.Remove(Data);
		Data->Broadcast(UPsEvent::ConstructEvent(TEXT("Removed"), false));
	}
}

UPsData::UPsData(const class FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
, Name()
, Parent(nullptr)
, BroadcastInProgress(0)
{
	FDataReflection::AddToQueue(this);
}

void UPsData::PostInitProperties()
{
	Super::PostInitProperties();
	FDataReflection::RemoveFromQueue(this);
	FDataReflection::Fill(this);
}

void UPsData::Broadcast(UPsEvent* Event)
{
	const bool bDeferredEventProcessing = (FDataReflection::GetDataAccess() != nullptr && FDataReflection::GetDataAccess()->DeferredEventProcessing());
	if (bDeferredEventProcessing)
	{
		TWeakObjectPtr<UPsData> WeakPtr(this);
		Event->AddToRoot();
		AsyncTask(ENamedThreads::GameThread, [WeakPtr, Event]()
		{
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

void UPsData::Bind(FString Type, const FPsDataDynamicDelegate& Delegate)
{
	if (!Delegate.IsBound())
	{
		return;
	}
	
	DynamicDelegates.FindOrAdd(Type).Add(Delegate);
	UpdateDelegates();
}

void UPsData::Bind(FString Type, const FPsDataDelegate& Delegate)
{
	if (!Delegate.IsBound())
	{
		return;
	}
	
	Delegates.FindOrAdd(Type).Add(Delegate);
	UpdateDelegates();
}

void UPsData::Unbind(FString Type, const FPsDataDynamicDelegate& Delegate)
{
	if (Delegate.IsBound())
	{
		auto Find = DynamicDelegates.Find(Type);
		if (Find)
		{
			for (FPsDataDynamicDelegate& Item : *Find)
			{
				if (Item == Delegate)
				{
					Item.Unbind();
					break;
				}
			}
		}
	}
	
	UpdateDelegates();
}

void UPsData::Unbind(FString Type, const FPsDataDelegate& Delegate)
{
	if (Delegate.IsBound())
	{
		auto Find = Delegates.Find(Type);
		if (Find)
		{
			for (FPsDataDelegate& Item : *Find)
			{
				if (Item.GetHandle() == Delegate.GetHandle())
				{
					Item.Unbind();
					break;
				}
			}
		}
	}
	
	UpdateDelegates();
}

void UPsData::BlueprintBind(FString Type, const FPsDataDynamicDelegate& Delegate)
{
	Bind(Type, Delegate);
}

void UPsData::BlueprintUnbind(FString Type, const FPsDataDynamicDelegate& Delegate)
{
	Unbind(Type, Delegate);
}

void UPsData::UpdateDelegates()
{
	if (BroadcastInProgress > 0)
	{
		return;
	}
	
	for(auto MapIt = DynamicDelegates.CreateIterator(); MapIt; ++MapIt)
	{
		for(auto ArrayIt = MapIt->Value.CreateIterator(); ArrayIt; ++ArrayIt)
		{
			if (!(*ArrayIt).IsBound())
			{
				ArrayIt.RemoveCurrent();
			}
		}
		
		if (MapIt->Value.Num() == 0)
		{
			MapIt.RemoveCurrent();
		}
	}
	for(auto MapIt = Delegates.CreateIterator(); MapIt; ++MapIt)
	{
		for(auto ArrayIt = MapIt->Value.CreateIterator(); ArrayIt; ++ArrayIt)
		{
			if (!(*ArrayIt).IsBound())
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

void UPsData::BroadcastInternal(UPsEvent* Event)
{
	++BroadcastInProgress;
	
	if (Event->Target == nullptr)
	{
		Event->Target = this;
	}
	
	if (!Event->bStopImmediate)
	{
		auto Find = DynamicDelegates.Find(Event->Type);
		if (Find)
		{
			TSet<FPsDataDynamicDelegate> Copy = *Find;
			for(FPsDataDynamicDelegate& Delegate : Copy)
			{
				if (Delegate.IsBound())
				{
					Delegate.ExecuteIfBound(Event);
					if (Event->bStopImmediate)
					{
						break;
					}
				}
			}
		}
	}
	
	if (!Event->bStopImmediate)
	{
		auto Find = Delegates.Find(Event->Type);
		if (Find)
		{
			TArray<FPsDataDelegate> Copy = *Find;
			for(FPsDataDelegate& Delegate : Copy)
			{
				if (Delegate.IsBound())
				{
					Delegate.ExecuteIfBound(Event);
					if (Event->bStopImmediate)
					{
						break;
					}
				}
			}
		}
	}
	
	if (!Event->bStop)
	{
		if (Event->bBubbles && Parent)
		{
			Parent->BroadcastInternal(Event);
		}
	}
	
	--BroadcastInProgress;
	
	UpdateDelegates();
}

/***********************************
 * Serialize
 ***********************************/

#define SERIALIZE(Type) \
			if (Pair.Value.ContainerType == EDataContainerType::DCT_None) \
			{ \
				Type& Value = *FDataReflectionTools::UnsafeGet<Type>(this, Pair.Value.Offset); \
				if (Ser.IsSerialize()) \
				{ \
					Ser.Serialize(Alias, Value); \
				} \
				else \
				{ \
					Type NewValue = Ser.Deserialize(this, Alias, Value, Pair.Value.Class); \
					if (!Pair.Value.Meta.bStrict) \
						FDataReflectionTools::Set<Type>(this, Pair.Value.Name, NewValue); \
				} \
			} \
			else if (Pair.Value.ContainerType == EDataContainerType::DCT_Array) \
			{ \
				TArray<Type>& Value = *FDataReflectionTools::UnsafeGet<TArray<Type>>(this, Pair.Value.Offset); \
				if (Ser.IsSerialize()) \
				{ \
					Ser.Serialize(Alias, Value); \
				} \
				else \
				{ \
					TArray<Type> NewValue = Ser.Deserialize(this, Alias, Value, Pair.Value.Class); \
					FDataReflectionTools::Set<TArray<Type>>(this, Pair.Value.Name, NewValue); \
				} \
			} \
			else if (Pair.Value.ContainerType == EDataContainerType::DCT_Map) \
			{ \
				TMap<FString, Type>& Value = *FDataReflectionTools::UnsafeGet<TMap<FString, Type>>(this, Pair.Value.Offset); \
				if (Ser.IsSerialize()) \
				{ \
					Ser.Serialize(Alias, Value); \
				} \
				else \
				{ \
					TMap<FString, Type> NewValue = Ser.Deserialize(this, Alias, Value, Pair.Value.Class); \
					FDataReflectionTools::Set<TMap<FString, Type>>(this, Pair.Value.Name, NewValue); \
				} \
			}

void UPsData::DataSerialize(FDataSerializer& Ser)
{
	for(auto& Pair : FDataReflection::GetFields(this->GetClass()))
	{
		const FString& Alias = Pair.Value.Meta.Alias.IsEmpty() ? Pair.Value.Name : Pair.Value.Meta.Alias; \
		if (!Ser.Has(Alias))
		{
			continue;
		}
		
		if (Pair.Value.Type == EDataFieldType::DFT_Data)
		{
			SERIALIZE(UPsData*);
		}
		else if (Pair.Value.Type == EDataFieldType::DFT_float)
		{
			SERIALIZE(float);
		}
		else if (Pair.Value.Type == EDataFieldType::DFT_int32)
		{
			SERIALIZE(int32);
		}
		else if (Pair.Value.Type == EDataFieldType::DFT_String)
		{
			SERIALIZE(FString);
		}
		else if (Pair.Value.Type == EDataFieldType::DFT_bool)
		{
			SERIALIZE(bool);
		}
	}
}

/***********************************
 * Property
 ***********************************/

FString UPsData::GetName() const
{
	return Name;
}

UPsData* UPsData::GetDataProperty(const FString& PropertyName)
{
	UPsData* Default = nullptr;
	return FDataReflectionTools::Get<UPsData*>(this, PropertyName, Default);
}

void UPsData::SetDataProperty(const FString& PropertyName, UPsData* Value)
{
	FDataReflectionTools::Set<UPsData*>(this, PropertyName, Value);
}

int32 UPsData::GetIntProperty(const FString& PropertyName)
{
	int32 Default = 0;
	return FDataReflectionTools::Get<int32>(this, PropertyName, Default);
}

void UPsData::SetIntProperty(const FString& PropertyName, int32 Value)
{
	FDataReflectionTools::Set<int32>(this, PropertyName, Value);
}

float UPsData::GetFloatProperty(const FString& PropertyName)
{
	float Default = 0.f;
	return FDataReflectionTools::Get<float>(this, PropertyName, Default);
}

void UPsData::SetFloatProperty(const FString& PropertyName, float Value)
{
	FDataReflectionTools::Set<float>(this, PropertyName, Value);
}

FString UPsData::GetStringProperty(const FString& PropertyName)
{
	FString Default;
	return FDataReflectionTools::Get<FString>(this, PropertyName, Default);
}

void UPsData::SetStringProperty(const FString& PropertyName, const FString& Value)
{
	FDataReflectionTools::Set<FString>(this, PropertyName, Value);
}

bool UPsData::GetBoolProperty(const FString& PropertyName)
{
	bool Default = false;
	return FDataReflectionTools::Get<bool>(this, PropertyName, Default);
}

void UPsData::SetBoolProperty(const FString& PropertyName, bool Value)
{
	FDataReflectionTools::Set<bool>(this, PropertyName, Value);
}

/***********************************
 * Array property
 ***********************************/

TArray<UPsData*> UPsData::GetDataArrayProperty(const FString& PropertyName)
{
	TArray<UPsData*> Default;
	return FDataReflectionTools::Get<TArray<UPsData*>>(this, PropertyName, Default);
}

void UPsData::SetDataArrayProperty(const FString& PropertyName, const TArray<UPsData*>& Value)
{
	FDataReflectionTools::Set<TArray<UPsData*>>(this, PropertyName, Value);
}

TArray<int32> UPsData::GetIntArrayProperty(const FString& PropertyName)
{
	TArray<int32> Default;
	return FDataReflectionTools::Get<TArray<int32>>(this, PropertyName, Default);
}

void UPsData::SetIntArrayProperty(const FString& PropertyName, const TArray<int32>& Value)
{
	FDataReflectionTools::Set<TArray<int32>>(this, PropertyName, Value);
}

TArray<float> UPsData::GetFloatArrayProperty(const FString& PropertyName)
{
	TArray<float> Default;
	return FDataReflectionTools::Get<TArray<float>>(this, PropertyName, Default);
}

void UPsData::SetFloatArrayProperty(const FString& PropertyName, const TArray<float>& Value)
{
	FDataReflectionTools::Set<TArray<float>>(this, PropertyName, Value);
}

TArray<FString> UPsData::GetStringArrayProperty(const FString& PropertyName)
{
	TArray<FString> Default;
	return FDataReflectionTools::Get<TArray<FString>>(this, PropertyName, Default);
}

void UPsData::SetStringArrayProperty(const FString& PropertyName, const TArray<FString>& Value)
{
	FDataReflectionTools::Set<TArray<FString>>(this, PropertyName, Value);
}

TArray<bool> UPsData::GetBoolArrayProperty(const FString& PropertyName)
{
	TArray<bool> Default;
	return FDataReflectionTools::Get<TArray<bool>>(this, PropertyName, Default);
}

void UPsData::SetBoolArrayProperty(const FString& PropertyName, const TArray<bool>& Value)
{
	FDataReflectionTools::Set<TArray<bool>>(this, PropertyName, Value);
}

/***********************************
 * Map property
 ***********************************/

TMap<FString, UPsData*> UPsData::GetDataMapProperty(const FString& PropertyName)
{
	TMap<FString, UPsData*> Default;
	return FDataReflectionTools::Get<TMap<FString, UPsData*>>(this, PropertyName, Default);
}

void UPsData::SetDataMapProperty(const FString& PropertyName, const TMap<FString, UPsData*>& Value)
{
	FDataReflectionTools::Set<TMap<FString, UPsData*>>(this, PropertyName, Value);
}

TMap<FString, int32> UPsData::GetIntMapProperty(const FString& PropertyName)
{
	TMap<FString, int32> Default;
	return FDataReflectionTools::Get<TMap<FString, int32>>(this, PropertyName, Default);
}

void UPsData::SetIntMapProperty(const FString& PropertyName, const TMap<FString, int32>& Value)
{
	FDataReflectionTools::Set<TMap<FString, int32>>(this, PropertyName, Value);
}

TMap<FString, float> UPsData::GetFloatMapProperty(const FString& PropertyName)
{
	TMap<FString, float> Default;
	return FDataReflectionTools::Get<TMap<FString, float>>(this, PropertyName, Default);
}

void UPsData::SetFloatMapProperty(const FString& PropertyName, const TMap<FString, float>& Value)
{
	FDataReflectionTools::Set<TMap<FString, float>>(this, PropertyName, Value);
}

TMap<FString, FString> UPsData::GetStringMapProperty(const FString& PropertyName)
{
	TMap<FString, FString> Default;
	return FDataReflectionTools::Get<TMap<FString, FString>>(this, PropertyName, Default);
}

void UPsData::SetStringMapProperty(const FString& PropertyName, const TMap<FString, FString>& Value)
{
	FDataReflectionTools::Set<TMap<FString, FString>>(this, PropertyName, Value);
}

TMap<FString, bool> UPsData::GetBoolMapProperty(const FString& PropertyName)
{
	TMap<FString, bool> Default;
	return FDataReflectionTools::Get<TMap<FString, bool>>(this, PropertyName, Default);
}

void UPsData::SetBoolMapProperty(const FString& PropertyName, const TMap<FString, bool>& Value)
{
	FDataReflectionTools::Set<TMap<FString, bool>>(this, PropertyName, Value);
}
