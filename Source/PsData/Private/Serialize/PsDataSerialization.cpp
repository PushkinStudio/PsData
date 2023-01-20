// Copyright 2015-2023 MY.GAMES. All Rights Reserved.

#include "Serialize/PsDataSerialization.h"

#include "PsData.h"

/***********************************
 * FPsDataAllocator
 ***********************************/

FPsDataAllocator::FPsDataAllocator(UClass* InClass, UPsData* InParent)
	: Class(InClass)
	, Parent(InParent)
{
}

FPsDataAllocator::FPsDataAllocator(UField* InField, UPsData* InParent)
	: Class(CastChecked<UClass>(InField))
	, Parent(InParent)
{
}

FPsDataAllocator::FPsDataAllocator()
	: Class(nullptr)
	, Parent(nullptr)
{
}

UPsData* FPsDataAllocator::operator()() const
{
	if (Class != nullptr)
	{
		return NewObject<UPsData>(Parent, Class);
	}
	else
	{
		checkNoEntry();
		return nullptr;
	}
}

/***********************************
 * FPsDataSerializer
 ***********************************/

FPsDataSerializer::FPsDataSerializer()
	: bWriteDefaults(false)
{
}

/***********************************
 * FPsDataDeserializer
 ***********************************/

FPsDataDeserializer::FPsDataDeserializer()
{
}
