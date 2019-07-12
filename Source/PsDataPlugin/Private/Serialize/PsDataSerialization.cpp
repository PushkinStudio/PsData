// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataSerialization.h"

#include "PsData.h"

/***********************************
 * FPsDataSerializer
 ***********************************/

FPsDataSerializer::FPsDataSerializer(bool InUseSortedKeys)
	: bUseSortedKeys(InUseSortedKeys)
{
}

bool FPsDataSerializer::UseSortedKeys()
{
	return bUseSortedKeys;
}

/***********************************
 * FPsDataDeserializer
 ***********************************/

FPsDataDeserializer::FPsDataDeserializer()
{
}
