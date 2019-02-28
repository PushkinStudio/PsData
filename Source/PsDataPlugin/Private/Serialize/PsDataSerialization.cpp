// Copyright 2015-2019 Mail.Ru Group. All Rights Reserved.

#include "Serialize/PsDataSerialization.h"

#include "PsData.h"

/***********************************
 * FPsDataSerializer
 ***********************************/

FPsDataSerializer::FPsDataSerializer()
{
}

/***********************************
 * FPsDataDeserializer
 ***********************************/

FPsDataDeserializer::FPsDataDeserializer()
{
}

bool FPsDataDeserializer::Has(const TSharedPtr<const FDataField>& Field)
{
	return false;
}
