// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataEvent.h"
#include "PsDataField.h"
#include "Serialize/PsDataSerialization.h"

#include "CoreMinimal.h"
#include "Delegates/Delegate.h"

#include "PsData.generated.h"

DEFINE_LOG_CATEGORY_STATIC(LogData, VeryVerbose, All);

DECLARE_DYNAMIC_DELEGATE_OneParam(FPsDataDynamicDelegate, UPsDataEvent*, Event); // TBaseDynamicDelegate
DECLARE_DELEGATE_OneParam(FPsDataDelegate, UPsDataEvent*);						 // TBaseDelegate

class UPsData;

class PSDATAPLUGIN_API FDataDelegates
{
public:
	static FSimpleMulticastDelegate OnPostDataModuleInit;

private:
	// This class is only for namespace use
	FDataDelegates() {}
};

/***********************************
* Abstract memory
***********************************/

struct PSDATAPLUGIN_API FAbstractDataMemory
{
	FAbstractDataMemory() {}
	virtual ~FAbstractDataMemory() {}

	virtual void Serialize(const UPsData* const Instance, const TSharedPtr<const FDataField>& Field, FPsDataSerializer* Serializer) const = 0;
	virtual void Deserialize(UPsData* Instance, const TSharedPtr<const FDataField>& Field, FPsDataDeserializer* Deserializer) = 0;
	virtual void Reset(UPsData* Instance, const TSharedPtr<const FDataField>& Field) = 0;
};

/***********************************
* PsData friend
***********************************/

namespace FDataReflectionTools
{
struct PSDATAPLUGIN_API FPsDataFriend
{
	static void ChangeDataName(UPsData* Data, const FString& Name, const FString& CollectionName);
	static void AddChild(UPsData* Parent, UPsData* Data);
	static void RemoveChild(UPsData* Parent, UPsData* Data);
	static bool IsChanged(UPsData* Data);
	static void SetIsChanged(UPsData* Data, bool NewValue);
	static void InitProperties(UPsData* Data);
	static TArray<TUniquePtr<FAbstractDataMemory>>& GetMemory(UPsData* Data);
};
} // namespace FDataReflectionTools

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
 * PsData check
 ***********************************/

namespace FDataReflectionTools
{
template <typename T>
struct TIsPsData
{
private:
	static constexpr bool IsPsData(UPsData* Object) { return true; }

public:
	static constexpr bool Value = IsPsData((T*)nullptr);
};

template <typename T>
struct TIsPsData<T*>
{
private:
	static constexpr bool IsPsData(UPsData* Object) { return true; }

public:
	static constexpr bool Value = IsPsData((T*)nullptr);
};
} // namespace FDataReflectionTools

/***********************************
* PSDATA!
***********************************/

UCLASS(BlueprintType, Blueprintable)
class PSDATAPLUGIN_API UPsData : public UObject
{
	GENERATED_UCLASS_BODY()

private:
	friend struct FDataReflectionTools::FPsDataFriend;

	/** Memory */
	TArray<TUniquePtr<FAbstractDataMemory>> Memory;

	/** Data name */
	FString DataKey;

	/** Data collection name */
	FString CollectionKey;

	/** Parent */
	UPROPERTY()
	UPsData* Parent;

	/** Children */
	UPROPERTY()
	TSet<UPsData*> Children;

	/** Broadcast in progress (necessary for correct iteration by delegates) */
	mutable int32 BroadcastInProgress;

	/** Changed flag */
	bool bChanged;

	/** Map of delegat wrappers */
	mutable TMap<FString, TArray<TSharedRef<FDelegateWrapper>>> Delegates;

private:
	/** Post init properties */
	virtual void PostInitProperties() override;

protected:
	/** Init properties */
	virtual void InitProperties();

	/** Post Deserialize */
	virtual void PostDeserialize();

	/***********************************
	 * Event system
	 ***********************************/
public:
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
	FPsDataBind Bind(int32 Hash, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	FPsDataBind Bind(int32 Hash, const FPsDataDelegate& Delegate) const;

	/** Bind */
	void Unbind(int32 Hash, const FPsDataDynamicDelegate& Delegate) const;

	/** Bind */
	void Unbind(int32 Hash, const FPsDataDelegate& Delegate) const;

protected:
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
	UPsData* GetRoot() const;

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
