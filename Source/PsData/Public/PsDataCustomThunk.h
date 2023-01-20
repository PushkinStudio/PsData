// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#pragma once

#include "PsDataCore.h"
#include "PsDataLink.h"

#include "CoreMinimal.h"

struct PSDATA_API FCustomThunkTemplates_PsData
{
	template <typename T>
	static void GetProperty(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetProperty(UPsData* Target, int32 Index, const T& Value)
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
	static void SetArrayProperty(UPsData* Target, int32 Index, PsDataTools::TConstRefType<TArray<T>, false> Value)
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
	static void SetMapProperty(UPsData* Target, int32 Index, PsDataTools::TConstRefType<TMap<FString, T>, false> Value)
	{
		PsDataTools::UnsafeSetByIndex<TMap<FString, T>>(Target, Index, Value);
	}

	template <typename T>
	static void GetLinkValue(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void GetArrayLinkValue(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
		Out = *Result;
	}
};

struct PSDATA_API FCustomThunkTemplates_PsDataPtr
{
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
	static void SetArrayProperty(UPsData* Target, int32 Index, PsDataTools::TConstRefType<TArray<T>, false> Value)
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
	static void SetMapProperty(UPsData* Target, int32 Index, PsDataTools::TConstRefType<TMap<FString, T>, false> Value)
	{
		PsDataTools::UnsafeSetByIndex<TMap<FString, T>>(Target, Index, Value);
	}

	template <typename T>
	static void GetLinkValue(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
		Out = *Result;
	}

	template <typename T>
	static void GetArrayLinkValue(UPsData* Target, int32 Index, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::UnsafeGetLinkValueByIndex(Target, Index, Result);
		Out = *Result;
	}
};
