// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartupData/DataAsset_HeroStartupData.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"


void UDataAsset_HeroStartupData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, int32 Level)
{
	Super::GiveToAbilitySystemComponent(InASC, Level);

	for(const auto& HeroAbility : HeroStartUpAbilitySets)
	{
		if(!HeroAbility.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(HeroAbility.AbilityToGrant);
		AbilitySpec.SourceObject = InASC->GetAvatarActor();
		AbilitySpec.Level = Level;
		AbilitySpec.DynamicAbilityTags.AddTag(HeroAbility.InputTag);
		
		InASC->GiveAbility(AbilitySpec);
	}
}
