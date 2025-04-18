// Fill out your copyright notice in the Description page of Project Settings.


#include "DataAssets/StartupData/DataAsset_StartupDataBase.h"

#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"

void UDataAsset_StartupDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, int32 Level)
{
	check(InASC);

	GrantAbilities(ActivateOnGivenAbilities, InASC, Level);
	GrantAbilities(ReactiveAbilities, InASC, Level);

	if(!StartupGameplayEffects.IsEmpty())
	{
		for(const TSubclassOf<UGameplayEffect>& GameplayEffect : StartupGameplayEffects)
		{
			if(!GameplayEffect) continue;

			UGameplayEffect* EffectCDO = GameplayEffect->GetDefaultObject<UGameplayEffect>();
			InASC->ApplyGameplayEffectToSelf(EffectCDO, Level, InASC->MakeEffectContext());
		}
	}
}

void UDataAsset_StartupDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& AbilitiesArray,
	UWarriorAbilitySystemComponent* InASC, int32 Level)
{
	if(AbilitiesArray.IsEmpty())
	{
		return;
	}
	
	for(const auto& Ability : AbilitiesArray)
	{
		if(!Ability) continue;

		FGameplayAbilitySpec Spec(Ability);
		Spec.SourceObject = InASC->GetAvatarActor();
		Spec.Level = Level;

		InASC->GiveAbility(Spec);
	}
}
