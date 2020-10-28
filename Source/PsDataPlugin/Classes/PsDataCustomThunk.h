// Copyright 2015-2020 Mail.Ru Group. All Rights Reserved.

#pragma once

#include "PsDataCore.h"

#include "CoreMinimal.h"

struct PSDATAPLUGIN_API FCustomThunkTemplates_PsData
{
public:
	template <typename T>
	static void GetProperty(UPsData* Target, int32 Hash, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::GetByHash(Target, Hash, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetProperty(UPsData* Target, int32 Hash, T& Value)
	{
		PsDataTools::SetByHash<T>(Target, Hash, Value);
	}

	template <typename T>
	static void GetArrayProperty(UPsData* Target, int32 Hash, TArray<T>& Out)
	{
		TArray<T>* Result = nullptr;
		PsDataTools::GetByHash(Target, Hash, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetArrayProperty(UPsData* Target, int32 Hash, typename PsDataTools::TConstRef<TArray<T>>::Type Value)
	{
		PsDataTools::SetByHash<TArray<T>>(Target, Hash, Value);
	}

	template <typename T>
	static void GetMapProperty(UPsData* Target, int32 Hash, TMap<FString, T>& Out)
	{
		TMap<FString, T>* Result = nullptr;
		PsDataTools::GetByHash(Target, Hash, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetMapProperty(UPsData* Target, int32 Hash, typename PsDataTools::TConstRef<TMap<FString, T>>::Type Value)
	{
		PsDataTools::SetByHash<TMap<FString, T>>(Target, Hash, Value);
	}
};

struct PSDATAPLUGIN_API FCustomThunkTemplates_PsDataPtr
{
public:
	template <typename T>
	static void GetProperty(UPsData* Target, int32 Hash, T& Out)
	{
		T* Result = nullptr;
		PsDataTools::GetByHash(Target, Hash, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetProperty(UPsData* Target, int32 Hash, T Value)
	{
		PsDataTools::SetByHash<T>(Target, Hash, Value);
	}

	template <typename T>
	static void GetArrayProperty(UPsData* Target, int32 Hash, TArray<T>& Out)
	{
		TArray<T>* Result = nullptr;
		PsDataTools::GetByHash(Target, Hash, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetArrayProperty(UPsData* Target, int32 Hash, typename PsDataTools::TConstRef<TArray<T>>::Type Value)
	{
		PsDataTools::SetByHash<TArray<T>>(Target, Hash, Value);
	}

	template <typename T>
	static void GetMapProperty(UPsData* Target, int32 Hash, TMap<FString, T>& Out)
	{
		TMap<FString, T>* Result = nullptr;
		PsDataTools::GetByHash(Target, Hash, Result);
		Out = *Result;
	}

	template <typename T>
	static void SetMapProperty(UPsData* Target, int32 Hash, typename PsDataTools::TConstRef<TMap<FString, T>>::Type Value)
	{
		PsDataTools::SetByHash<TMap<FString, T>>(Target, Hash, Value);
	}
};