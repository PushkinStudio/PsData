// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#pragma once

#include "PsDataAPI.h"

#include "CoreMinimal.h"

#include "PsDataTestModel.generated.h"

/**
 * A small game model the automation tests run against. It is shaped like a real one on purpose:
 * static data ("World", filled once from tables) and live state ("Battle") under one root, with
 * links from the live state into the static data.
 *
 *   UPsDataTestRoot
 *   ├── World                   static data
 *   │   └── Weapons{key}        UPsDataTestWeaponProto
 *   └── Battle                  live state
 *       ├── Round
 *       └── Characters{key}     UPsDataTestCharacter
 *           ├── Location        UPsDataTestPoint (Strict: always allocated)
 *           ├── Weapon  ──link──> World.Weapons{Weapon}
 *           └── Target  ──link──> Battle.Characters{Target} (Nullable)
 */

UENUM(BlueprintType)
enum class EPsDataTestTeam : uint8
{
	None = 0,
	Red = 1,
	Blue = 2,
};
DESCRIBE_ENUM(EPsDataTestTeam);

UCLASS()
class UPsDataTestWeaponProto : public UPsData
{
	GENERATED_BODY()

public:
	DPROP(int32, Damage);

	DPROP(FPsDataFixedPoint, FireRate);

	/** Generates the PsDataTestWeaponProto_DataTableRow struct, so a DataTable can hold these rows */
	MAKE_TABLE_STRUCT({
		Damage = 0;
		FireRate = 0;
	});
};

UCLASS()
class UPsDataTestWorld : public UPsData
{
	GENERATED_BODY()

public:
	DMAP(UPsDataTestWeaponProto*, Weapons);
};

UCLASS()
class UPsDataTestPoint : public UPsData
{
	GENERATED_BODY()

public:
	DMETA(Event, Bubbles)
	DPROP(int32, X);

	DMETA(Event, Bubbles)
	DPROP(int32, Y);
};

UCLASS()
class UPsDataTestCharacter : public UPsData
{
	GENERATED_BODY()

public:
	DPROP(FString, Name);

	DMETA(Event, EventStorage)
	DPROP(int32, Health);

	DMETA(Alias = "hp_max")
	DPROP(int32, MaxHealth);

	/** No Event meta: changes are silent unless a listener binds with IgnoreFieldMeta */
	DPROP(int32, Kills);

	DMETA(Event)
	DPROP(EPsDataTestTeam, Team);

	DMETA(Strict)
	DPROP(UPsDataTestPoint*, Location);

	DARRAY(FString, Tags);

	DPROP(FString, Weapon);
	DLINK(UPsDataTestWeaponProto*, Weapon, World.Weapons);

	DPROP(FString, Target);
	DMETA(Nullable)
	DLINK(UPsDataTestCharacter*, Target, Battle.Characters);

protected:
	virtual void InitProperties() override
	{
		Health = 100;
		MaxHealth = 100;
	}
};

UCLASS()
class UPsDataTestBattle : public UPsData
{
	GENERATED_BODY()

public:
	DMETA(Event, Bubbles)
	DPROP(int32, Round);

	DMAP(UPsDataTestCharacter*, Characters);
};

UCLASS()
class UPsDataTestRoot : public UPsDataRoot
{
	GENERATED_BODY()

public:
	DMETA(Strict)
	DPROP(UPsDataTestWorld*, World);

	DMETA(Strict)
	DPROP(UPsDataTestBattle*, Battle);
};

namespace PsDataTest
{
/** Root with two weapons and two characters, alice -> bob; the shape most tests start from */
UPsDataTestRoot* MakeModel();

/** Runs the game-thread tasks PsData defers (the coalesced "Changed" event) */
void FlushDeferred();
} // namespace PsDataTest
