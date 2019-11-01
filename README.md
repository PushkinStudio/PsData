# PsData

[![statusIcon](https://teamcity.ufna.dev/app/rest/builds/buildType:(id:Pushkin_PsData_ClangFormatCheck)/statusIcon.svg)](https://teamcity.ufna.dev/viewType.html?buildTypeId=Pushkin_PsData_ClangFormatCheck&guest=1)

Flexible data model plugin for Unreal Engine 4

## Examples

Define PsData classes:

```cpp
// Assume that you project called 'Foo'

#include "PsDataAPI.h"

// Enum

UENUM(BlueprintType)
enum class EFooDamageType : uint8
{
    None = 0,
    Pure = 1,
    Physical = 2,
    Psi = 3,
    Pierce = 4
};
DESCRIBE_ENUM(EFooDamageType);

// Character data class

UCLASS()
class UFooCharacterData : public UPsData
{
    GENERATED_BODY()

    /** Weapon damage type */
    DPROP(EFooDamageType, DamageType);

    /** Current health value */
    /** DMETA(Event) means that this property can fire events when it will changed */
    DMETA(Event)
    DPROP(int32, Health);

    /** Equipmented weapon */
    /** This is property with other PsData class type */
    DPROP(UFooWeaponData*, EquipmentedWeapon);

    /** Current location on the battlefield */
    /** DMETA(Strict) means that this property was be constructed by default and cannot be null or be reset */
    DMETA(Strict)
    DPROP(UFooPointData*, Location);

    /** Abilities */
    /** This is array property */
    DARRAY(UFooBattleAbilityData*, BattleAbilities);

    /** Link to the character prototype */
    DPROP(FString, CharacterProtoId);
    DLINK(UFooCharacterProtoData, CharacterProtoId, Prototypes.Characters);

    /** Link to the current battle target */
    /** DMETA(Nullable) means that this link can be null */
    DPROP(FString, TargetId);
    DMETA(Nullable)
    DLINK(UFooCharacterData, TargetId, Game.Battle.Characters);

protected:
    /** Some kind of constructor */
    void InitProperties() override
    {
        SetDamageType(EFooDamageType::Physical);
    }
};

UCLASS()
class UFooPointData : public UPsData
{
    GENERATED_BODY()

    DMETA(Event, Bubbles)
    DPROP(int32, X);

    DMETA(Event, Bubbles)
    DPROP(int32, Y);

public:
    FIntPoint GetIntPoint() const
    {
        return FIntPoint{GetX(), GetY()};
    }
};
```

Root of the model:

```cpp
// Root of the data model

UCLASS(Blueprintable, BlueprintType)
class UFooRootData : public UPsData
{
    /** Game state */
    DMETA(Strict)
    DPROP(UFooGameData*, Game);


    /** Static world data */
    DMETA(Strict)
    DPROP_CONST(UFooPrototypesData*, Prototypes, FFooPrototypesDataAccessor);
};


////////////////////////////////////////////////////////
// Accessors for mutation of root data const members

class FFooPrototypesDataAccessor
{
private:
    // Only classes that need modifying static world data
    friend UFooDataController;

    static UFooPrototypesData* GetMutablePrototypes(UFooRootData* RootData)
    {
        return RootData->GetMutablePrototypes();
    }
};
```

Work with the model:

```cpp
void UFooDataController::Init()
{
    // Create root node
    RootData = NewObject<UFooRootData>();

    // Get pointer to the mutable data
    auto PrototypesData = FFooPrototypesDataAccessor::GetMutablePrototypes(RootData);

    // Deserialize from UDataTable asset PrototypesDataTableAsset
    auto Deserializer = FPsDataTableDeserializer(PrototypesDataTableAsset, PrototypesData->GetCharacters());
    PrototypesData->DataDeserialize(&Deserializer);

    auto Characters = PrototypesData->GetCharacters();
    for (auto const Proto : Characters) {
        auto GameCharacter = NewObject<UFooCharacterData>();
        GameCharacter->SetCharacterProtoId(Proto->GetDataKey());
        // Setup character...

        // Add into the game state
        RootData->GetGame()->GetCharacters().Add(GameCharacter);
    }
    //...
}
```
