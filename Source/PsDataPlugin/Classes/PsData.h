// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataEvent.h"
#include "PsDataField.h"
#include "PsDataPromise.h"
#include "Serialize/PsDataSerialization.h"
#include "Serialize/Stream/PsDataMD5OutputStream.h"

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

#include "PsData.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogData, VeryVerbose, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FPsDataDynamicDelegate, UPsDataEvent*, Event); // TBaseDynamicDelegate
DECLARE_DELEGATE_OneParam(FPsDataDelegate, UPsDataEvent*);                       // TBaseDelegate

class UPsData;
class UPsDataRoot;

class PSDATAPLUGIN_API FDataDelegates
{
public:
	static FSimpleMulticastDelegate OnPostDataModuleInit;
	static TPsDataSimplePromise PostDataModuleInitPromise;

private:
	// This class is only for namespace use
	FDataDelegates() {}
};

/***********************************
* Abstract property
***********************************/

struct PSDATAPLUGIN_API FAbstractDataProperty
{
	FAbstractDataProperty() {}
	virtual ~FAbstractDataProperty() {}

	virtual void Serialize(const UPsData* Instance, FPsDataSerializer* Serializer) = 0;
	virtual void Deserialize(UPsData* Instance, FPsDataDeserializer* Deserializer) = 0;
	virtual void Reset(UPsData* Instance) = 0;
	virtual void Allocate(UPsData* Instance){};
	virtual TSharedPtr<const FDataField> GetField() const = 0;
};

/***********************************
* PsData friend
***********************************/

namespace PsDataTools
{
struct PSDATAPLUGIN_API FPsDataFriend
{
	static void ChangeDataName(UPsData* Data, const FString& Name, const FString& CollectionName);
	static void AddChild(UPsData* Parent, UPsData* Data);
	static void RemoveChild(UPsData* Parent, UPsData* Data);
	static void Changed(UPsData* Data, const TSharedPtr<const FDataField>& Field);
	static void InitProperties(UPsData* Data);
	static TArray<FAbstractDataProperty*>& GetProperties(UPsData* Data);
	static void Serialize(const UPsData* Data, FPsDataSerializer* Serializer);
	static void Deserialize(UPsData* Data, FPsDataDeserializer* Deserializer);
};
} // namespace PsDataTools

/***********************************
 * FDelegateWrapper
 ***********************************/

struct FDelegateWrapper
{
	FPsDataDynamicDelegate DynamicDelegate;
	FPsDataDelegate Delegate;
	TSharedPtr<const FDataField> Field;

	FDelegateWrapper(const FPsDataDynamicDelegate& InDynamicDelegate, TSharedPtr<const FDataField> InField = nullptr)
		: DynamicDelegate(InDynamicDelegate)
		, Field(InField)
	{
	}

	FDelegateWrapper(const FPsDataDelegate& InDelegate, TSharedPtr<const FDataField> InField = nullptr)
		: Delegate(InDelegate)
		, Field(InField)
	{
	}

	bool IsBound() const
	{
		return DynamicDelegate.IsBound() || Delegate.IsBound();
	}
};

/***********************************
 * FPsDataBind
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBind
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
};

/***********************************
 * FPsDataBindCollection
 ***********************************/

struct PSDATAPLUGIN_API FPsDataBindCollection
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
 * FPsDataReport
 ***********************************/

enum class PSDATAPLUGIN_API EPsDataReportType : uint8
{
	Logic = 0,
	Link = 1,
};

struct PSDATAPLUGIN_API FPsDataReport
{
	EPsDataReportType Type;
	FString SourcePath;
	FString Reason;
	FString LinkedPath;

	FPsDataReport(EPsDataReportType InType, const FString& InSourcePath, const FString& InReason);
	FPsDataReport(EPsDataReportType InType, const FString& InSourcePath, const FString& InReason, const FString& InLinkedPath);
};

/***********************************
* PSDATA!
***********************************/

UCLASS(BlueprintType, Blueprintable)
class PSDATAPLUGIN_API UPsData : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	friend struct PsDataTools::FPsDataFriend;

	/** Properties */
	TArray<FAbstractDataProperty*> Properties;

	/** Data name */
	FString DataKey;

	/** Data collection name */
	FString CollectionKey;

	/** Parent */
	UPROPERTY()
	TWeakObjectPtr<UPsData> Parent;

	/** Children */
	UPROPERTY()
	TSet<UPsData*> Children;

	/** Broadcast in progress (necessary for correct iteration by delegates) */
	mutable int32 BroadcastInProgress;

	/** Changed flag */
	bool bChanged;

	/** Map of delegat wrappers */
	mutable TMap<FString, TArray<TSharedRef<FDelegateWrapper>>> Delegates;

	/** Data hash */
	mutable TOptional<FPsDataMD5Hash> Hash;

private:
	/** Post init properties */
	virtual void PostInitProperties() override;

	/** Drop hash */
	void DropHash();

	/** Calculate hash */
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
	UFUNCTION(BlueprintCallable, meta = (Category = "PsData|Data"))
	bool IsBound(const FString& Type, bool bBubbles) const;

	/** Broadcat */
	void Broadcast(UPsDataEvent* Event) const;

	/** Bind */
	FPsDataBind Bind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	FPsDataBind Bind(const FString& Type, const FPsDataDelegate& Delegate) const;

	/** Bind */
	void Unbind(const FString& Type, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	void Unbind(const FString& Type, const FPsDataDelegate& Delegate) const;

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

private:
	template <typename T, bool bConst>
	friend struct FPsDataBaseMapProxy;
	template <typename T, bool bConst>
	friend struct FPsDataBaseArrayProxy;

	/** Update delegates */
	void UpdateDelegates() const;

	/** Broadcast internal */
	void BroadcastInternal(UPsDataEvent* Event, const UPsData* Previous = nullptr) const;

	/** Bind internal */
	FPsDataBind BindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, TSharedPtr<const FDataField> Field = nullptr) const;

	/** Bind internal */
	FPsDataBind BindInternal(const FString& Type, const FPsDataDelegate& Delegate, TSharedPtr<const FDataField> Field = nullptr) const;

	/** Unbind internal */
	void UnbindInternal(const FString& Type, const FPsDataDynamicDelegate& Delegate, TSharedPtr<const FDataField> Field = nullptr) const;

	/** Unbind internal */
	void UnbindInternal(const FString& Type, const FPsDataDelegate& Delegate, TSharedPtr<const FDataField> Field = nullptr) const;

public:
	/***********************************
	 * Serialize/Deserialize
	 ***********************************/

	/** Serialize */
	void DataSerialize(FPsDataSerializer* Serializer) const;

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

	/** Get data key */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	const FString& GetDataKey() const;

	/** Get collection key */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	const FString& GetCollectionKey() const;

	/** Get parent */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	UPsData* GetParent() const;

	/** Get root */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	UPsDataRoot* GetRoot() const;

	/** Has root */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	bool HasRoot() const;

	/** Get data hash. For example, the method can be used with the transaction system */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	FString GetHash() const;

	/** Get data path from root */
	UFUNCTION(BlueprintCallable, Category = "PsData|Data")
	FString GetPathFromRoot() const;

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
	TArray<FPsDataReport> Validation() const;
};
