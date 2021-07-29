// Copyright 2015-2021 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "CoreMinimal.h"

struct PSDATA_API FCustomThunkTemplates_PsData
{
public:
	template <typename T>
	static void GetProperty(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetProperty(UPsData* Target, int32 Index, T& Value)
	{
		PsDataTools::UnsafeSetByIndex<T>(Target, Index, Value);
	}

	template <typename T>
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<T>& Out)
	{
		TArray<T>* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetArrayProperty(UPsData* Target, int32 Index, typename PsDataTools::TConstRef<TArray<T>>::Type Value)
	{
		PsDataTools::UnsafeSetByIndex<TArray<T>>(Target, Index, Value);
	}

	template <typename T>
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, T>& Out)
	{
		TMap<FString, T>* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetMapProperty(UPsData* Target, int32 Index, typename PsDataTools::TConstRef<TMap<FString, T>>::Type Value)
	{
		PsDataTools::UnsafeSetByIndex<TMap<FString, T>>(Target, Index, Value);
	}
};

struct PSDATA_API FCustomThunkTemplates_PsDataPtr
{
public:
	template <typename T>
	static void GetProperty(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetProperty(UPsData* Target, int32 Index, T Value)
	{
		PsDataTools::UnsafeSetByIndex<T>(Target, Index, Value);
	}

	template <typename T>
	static void GetArrayProperty(UPsData* Target, int32 Index, TArray<T>& Out)
	{
		TArray<T>* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetArrayProperty(UPsData* Target, int32 Index, typename PsDataTools::TConstRef<TArray<T>>::Type Value)
	{
		PsDataTools::UnsafeSetByIndex<TArray<T>>(Target, Index, Value);
	}

	template <typename T>
	static void GetMapProperty(UPsData* Target, int32 Index, TMap<FString, T>& Out)
	{
		TMap<FString, T>* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetMapProperty(UPsData* Target, int32 Index, typename PsDataTools::TConstRef<TMap<FString, T>>::Type Value)
	{
		PsDataTools::UnsafeSetByIndex<TMap<FString, T>>(Target, Index, Value);
	}
};