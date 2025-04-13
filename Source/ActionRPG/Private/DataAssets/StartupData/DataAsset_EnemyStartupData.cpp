// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartupData/DataAsset_EnemyStartupData.h"

#include "DebugHeader.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h"

void UDataAsset_EnemyStartupData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, int32 Level)
{
	Super::GiveToAbilitySystemComponent(InASC, Level);

	if(EnemyCombatAbilities.IsEmpty()) return;

	for(const auto& EnemyAbility : EnemyCombatAbilities)
	{
		if(!IsValid(EnemyAbility)) continue;

		FGameplayAbilitySpec AbilitySpec(EnemyAbility);
		AbilitySpec.SourceObject = InASC->GetAvatarActor();
		AbilitySpec.Level = Level;
		InASC->GiveAbility(AbilitySpec);
	}
}
