// Copyright 2015-2026 Pushkin Studio. All Rights Reserved.

#include "PsDataTestModel.h"

#include "Async/TaskGraphInterfaces.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, PsDataTests);

namespace PsDataTest
{
UPsDataTestRoot* MakeModel()
{
	const auto Root = NewObject<UPsDataTestRoot>();

	const auto Rifle = NewObject<UPsDataTestWeaponProto>();
	Rifle->Damage = 30;
	Rifle->FireRate = FPsDataFixedPoint(TEXT("10.5"));
	Root->World->Weapons->Add(TEXT("rifle"), Rifle);

	const auto Pistol = NewObject<UPsDataTestWeaponProto>();
	Pistol->Damage = 12;
	Pistol->FireRate = FPsDataFixedPoint(4);
	Root->World->Weapons->Add(TEXT("pistol"), Pistol);

	const auto Alice = NewObject<UPsDataTestCharacter>();
	Alice->Name = TEXT("Alice");
	Alice->Team = EPsDataTestTeam::Red;
	Alice->Weapon = TEXT("rifle");
	Alice->Target = TEXT("bob");
	Alice->Tags->Add(TEXT("sniper"));
	Root->Battle->Characters->Add(TEXT("alice"), Alice);

	const auto Bob = NewObject<UPsDataTestCharacter>();
	Bob->Name = TEXT("Bob");
	Bob->Team = EPsDataTestTeam::Blue;
	Bob->Weapon = TEXT("pistol");
	Root->Battle->Characters->Add(TEXT("bob"), Bob);

	return Root;
}

void FlushDeferred()
{
	FTaskGraphInterface::Get().ProcessThreadUntilIdle(ENamedThreads::GameThread);
}
} // namespace PsDataTest
