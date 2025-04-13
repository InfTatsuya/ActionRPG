// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"

#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Controllers/WarriorHeroController.h"

AWarriorHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
	if(!CachedHeroCharacter.IsValid())
	{
		CachedHeroCharacter = Cast<AWarriorHeroCharacter>(CurrentActorInfo->AvatarActor);
	}

	return CachedHeroCharacter.IsValid() ? CachedHeroCharacter.Get() : nullptr;
}

AWarriorHeroController* UWarriorHeroGameplayAbility::GetHeroControllerFromActorInfo()
{
	if(!CachedHeroController.IsValid())
	{
		CachedHeroController = Cast<AWarriorHeroController>(CurrentActorInfo->PlayerController);
	}

	return CachedHeroController.IsValid() ? CachedHeroController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
	return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(
	TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag,
	int32 InUsedComboCount)
{
	check(EffectClass);

	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	ContextHandle.SetAbility(this);
	ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
	ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
	
	 FGameplayEffectSpecHandle EffectSpecHandle =  GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), ContextHandle);

	EffectSpecHandle.Data->SetSetByCallerMagnitude(WarriorGameplayTags::Shared_SetByCaller_BaseDamage, InWeaponBaseDamage);

	if(InCurrentAttackTypeTag.IsValid())
	{
		EffectSpecHandle.Data->SetSetByCallerMagnitude(InCurrentAttackTypeTag, InUsedComboCount);
	}

	return EffectSpecHandle;
}
