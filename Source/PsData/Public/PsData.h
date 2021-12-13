// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataPromise.h"
#include "Serialize/FPsDataImprintSerializer.h"
#include "Serialize/PsDataSerialization.h"
#include "Serialize/Stream/PsDataMD5OutputStream.h"

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

#include "PsData.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogData, VeryVerbose, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FPsDataDynamicDelegate, UPsDataEvent*, Event);
DECLARE_DELEGATE_OneParam(FPsDataDelegate, UPsDataEvent*);

DECLARE_DELEGATE(FPsDataAsyncSerializeDelegate);

class UPsData;
class UPsDataRoot;
struct FAbstractDataProperty;
struct FAbstractDataLinkProperty;

namespace PsDataTools
{
struct FClassFields;
}

class PSDATA_API FDataDelegates
{
public:
	static FSimpleMulticastDelegate OnPostDataModuleInit;
	static TPsDataSimplePromise PostDataModuleInitPromise;

private:
	// This class is only for namespace use
	FDataDelegates() {}
};

/***********************************
 * FDelegateWrapper
 ***********************************/

struct FDelegateWrapper
{
	FPsDataDynamicDelegate DynamicDelegate;
	FPsDataDelegate Delegate;
	const FDataField* Field;
	bool bForced;

	FDelegateWrapper(const FPsDataDynamicDelegate& InDynamicDelegate, const FDataField* InField = nullptr)
		: DynamicDelegate(InDynamicDelegate)
		, Field(InField)
		, bForced(false)
	{
	}

	FDelegateWrapper(const FPsDataDelegate& InDelegate, const FDataField* InField = nullptr)
		: Delegate(InDelegate)
		, Field(InField)
		, bForced(false)
	{
	}

	bool IsBound() const
	{
		return DynamicDelegate.IsBound() || Delegate.IsBound();
	}

	bool IsForced() const
	{
		return bForced && IsBound();
	}
};

/***********************************
 * FPsDataBind
 ***********************************/

struct PSDATA_API FPsDataBind
{
private:
	friend UPsData;
	friend struct FPsDataBindCollection;

	/** Delegate wrapper */
	TSharedPtr<FDelegateWrapper> DelegateWrapper;

	/** Constructor */
	FPsDataBind(TSharedRef<FDelegateWrapper> InWrapper);

public:
	/** Empty constructor */
	FPsDataBind();

	/** Unbind */
	void Unbind();

	/** Ignore field metadata rule (Field->Meta.bEvent) */
	FPsDataBind Force() const;
};

/***********************************
 * FPsDataBindCollection
 ***********************************/

struct PSDATA_API FPsDataBindCollection
{
private:
	/** Collection */
	TArray<TSharedRef<FDelegateWrapper>> Collection;

public:
	/** Constructor */
	FPsDataBindCollection();

	~FPsDataBindCollection()
	{
		Unbind();
	}

	/** Add */
	void Add(const FPsDataBind& Bind);

	/** Unbind */
	void Unbind();
};

/***********************************
 * PsData friend
 ***********************************/

namespace PsDataTools
{
struct PSDATA_API FPsDataFriend
{
	static void ChangeDataName(UPsData* Data, const FString& Name, const FString& CollectionName);
	static void AddChild(UPsData* Parent, UPsData* Data);
	static void RemoveChild(UPsData* Parent, UPsData* Data);
	static void Changed(UPsData* Data, const FDataField* Field);
	static void InitProperties(UPsData* Data);
	static TArray<FAbstractDataProperty*>& GetProperties(UPsData* Data);
	static FAbstractDataProperty* GetProperty(UPsData* Data, int32 Index);
	static const FAbstractDataProperty* GetProperty(const UPsData* Data, int32 Index);
	static TArray<FAbstractDataLinkProperty*>& GetLinks(UPsData* Data);
	static FAbstractDataLinkProperty* GetLink(UPsData* Data, int32 Index);
	static const FAbstractDataLinkProperty* GetLink(const UPsData* Data, int32 Index);
	static void Serialize(const UPsData* Data, FPsDataSerializer* Serializer);
	static void Deserialize(UPsData* Data, FPsDataDeserializer* Deserializer);
	static const FPsDataImprint& GetImprint(const UPsData* Data);
	static const TSet<UPsData*>& GetChildren(const UPsData* Data);
	static FPsDataBind BindInternal(const UPsData* Data, const FString& Type, const FPsDataDynamicDelegate& Delegate, const FDataField* Field = nullptr);
	static FPsDataBind BindInternal(const UPsData* Data, const FString& Type, const FPsDataDelegate& Delegate, const FDataField* Field = nullptr);
	static void UnbindInternal(const UPsData* Data, const FString& Type, const FPsDataDynamicDelegate& Delegate, const FDataField* Field = nullptr);
	static void UnbindInternal(const UPsData* Data, const FString& Type, const FPsDataDelegate& Delegate, const FDataField* Field = nullptr);
};
} // namespace PsDataTools

/***********************************
 * Abstract property
 ***********************************/

struct PSDATA_API FAbstractDataProperty
{
	FAbstractDataProperty() {}
	virtual ~FAbstractDataProperty() {}
	virtual void Serialize(FPsDataSerializer* Serializer) = 0;
	virtual void Deserialize(FPsDataDeserializer* Deserializer) = 0;
	virtual void Reset() = 0;
	virtual bool IsDefault() const = 0;
	virtual void Allocate() {}
	virtual const FDataField* GetField() const = 0;
	virtual UPsData* GetOwner() = 0;
	virtual UPsData* GetOwner() const = 0;
};

/***********************************
 * Abstract link
 ***********************************/

struct PSDATA_API FAbstractDataLinkProperty
{
	FAbstractDataLinkProperty() {}
	virtual ~FAbstractDataLinkProperty() {}

	virtual void Validate(TArray<FString>& OutResult) const = 0;
	virtual const FDataLink* GetLink() const = 0;
	virtual UPsData* GetOwner() = 0;
	virtual UPsData* GetOwner() const = 0;
};

/***********************************
 * PSDATA!
 ***********************************/

UCLASS(BlueprintType, Blueprintable)
class PSDATA_API UPsData : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	friend struct PsDataTools::FPsDataFriend;
	friend class UPsDataRoot;

	/** Properties */
	TArray<FAbstractDataProperty*> Properties;

	/** Links */
	TArray<FAbstractDataLinkProperty*> Links;

	/** Data full key */
	FString FullKey;

	/** Data key */
	FString DataKey;

	/** Data collection key */
	FString CollectionKey;

	/** Parent */
	UPROPERTY()
	UPsData* Parent;

	/** Parent */
	UPROPERTY()
	UPsDataRoot* Root;

	/** Children */
	UPROPERTY()
	TSet<UPsData*> Children;

	/** Broadcast in progress (necessary for correct iteration by delegates) */
	mutable int32 BroadcastInProgress;

	/** Changed flag */
	bool bChanged;

	/** Map of delegate wrappers */
	mutable TMap<FString, TArray<TSharedRef<FDelegateWrapper>>> Delegates;

	/** Data hash */
	mutable TOptional<FPsDataMD5Hash> Hash;

	/** Data imprint */
	mutable FPsDataImprint Imprint;

	/** Async serialize buffer size after concatenation */
	mutable int32 SerializeBufferSize;

	/** Class fields */
	const PsDataTools::FClassFields* ClassFields;

	/** Post init properties */
	virtual void PostInitProperties() override;

	/** Add child */
	void AddChild(UPsData* Child);

	/** Remove child */
	void RemoveChild(UPsData* Child);

	/** Change name */
	void ChangeName(const FString& Name, const FString& CollectionName);

	/** Changed */
	void Changed(const FDataField* Field);

	/** Add to root data */
	void AddToRootData();

	/** Remove from root data */
	void RemoveFromRootData();

	/** Drop imprint */
	void DropImprint() const;

	/** Drop hash */
	void DropHash() const;

	/** Calculate cache */
	void CalculateImprint() const;

	/** Calculate cache */
	void CalculateHash() const;

protected:
	/** Init properties */
	virtual void InitProperties();

	/** Post Deserialize */
	virtual void PostDeserialize();

	/***********************************
	 * Event system
	 ***********************************/
public:
	/** Checks if delegates exist for EventType */
	UFUNCTION(BlueprintCallable, meta = (Category = "PsData|Data"))
	bool IsBound(const FString& EventType, bool bBubbles) const;

	/** Broadcast */
	void Broadcast(UPsDataEvent* Event) const;

	/** Bind */
	FPsDataBind Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	FPsDataBind Bind(const FString& Type, const FPsDataDelegate& Delegate) const;

	/** Bind */
	void Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const;

	/** Unbind all */
	void UnbindAll(UObject* Object) const;

protected:
	/** Bind */
	FPsDataBind Bind(int32 FieldHash, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	FPsDataBind Bind(int32 FieldHash, const FPsDataDelegate& Delegate) const;

	/** Bind */
	void Unbind(int32 FieldHash, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	void Unbind(int32 FieldHash, const FPsDataDelegate& Delegate) const;

public:
	/** Blueprint bind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Bind", Category = "PsData|Data"))
	void BlueprintBind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

	/** Blueprint unbind */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Unbind", Category = "PsData|Data"))
	void BlueprintUnbind(const FString& Type, const FPsDataDynamicDelegate& Delegate);

	/** Blueprint unbind all */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "UnbindAll", Category = "PsData|Data"))
	void BlueprintUnbindAll(UObject* Object);

private:
	/** Checks if forced delegates exist for EventType */
	bool HasForcedDelegates(const FString& EventType) const;

	/** Update delegates */
	void UpdateDelegates() const;

	/** Broadcast with previous */
	void Broadcast(UPsDataEvent* Event, const UPsData* Previous) const;

	/** Broadcast internal */
	void BroadcastInternal(UPsDataEvent* Event, const UPsData* Previous) const;

	/** Bind internal */
	FPsDataBind BindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, const FDataField* Field = nullptr) const;

	/** Bind internal */
	FPsDataBind BindInternal(const FString& Type, const FPsDataDelegate& Delegate, const FDataField* Field = nullptr) const;

	/** Unbind internal */
	void UnbindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, const FDataField* Field = nullptr) const;

	/** Unbind internal */
	void UnbindInternal(const FString& Type, const FPsDataDelegate& Delegate, const FDataField* Field = nullptr) const;

	/** Unbind all internal */
	void UnbindAllInternal(UObject* Object) const;

public:
	/***********************************
	 * Serialize/Deserialize
	 ***********************************/

	/** Serialize */
	void DataSerialize(FPsDataSerializer* Serializer) const;

	/** Async Serialize */
	void DataSerializeAsync(FPsDataSerializer* Serializer, FPsDataAsyncSerializeDelegate CallbackDelegate) const;

	/** Deserialize */
	void DataDeserialize(FPsDataDeserializer* Deserializer, bool bPatch = false);

private:
	/** Serialize */
	void DataSerializeInternal(FPsDataSerializer* Serializer) const;

	/** Deserialize */
	void DataDeserializeInternal(FPsDataDeserializer* Deserializer);

public:
	/***********************************
	 * Data property
	 ***********************************/

	/** Get full key */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	const FString& GetFullKey() const;

	/** Get data key */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	const FString& GetDataKey() const;

	/** Get collection key */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	const FString& GetCollectionKey() const;

	/** Get data path from root */
	void GetPathFromRoot(FString& OutPath) const;

	/** Get data path from root */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	FString GetPathFromRoot() const;

	/** Get parent */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	UPsData* GetParent() const;

	/** Get root */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	virtual UPsDataRoot* GetRoot() const;

	/** Has root */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	bool HasRoot() const;

	/** Get data hash. For example, the method can be used with the transaction system */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	FString GetHash() const;

	/** Data contains in collection */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	bool InCollection() const;

public:
	/***********************************
	 * Utility
	 ***********************************/

	/** Reset */
	void Reset();

	/** Make copy from current object */
	UPsData* Copy() const;

	/** Make copy from current object (templatized version) */
	template <typename T>
	T* Copy() const
	{
		return CastChecked<T>(Copy());
	}

	/** Validation */
	void Validation(TArray<FString>& OutResult) const;
	TArray<FString> Validation() const;
};
