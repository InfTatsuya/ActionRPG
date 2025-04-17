// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "WarriorAbilitySystemComponent.generated.h"

struct FWarriorHeroAbilitySet;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:

	void OnAbilityInputPressed(const FGameplayTag& InInputTag);
	void OnAbilityInputReleased(const FGameplayTag& InInputTag);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel = "1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& DefaultAbilitySet, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandle);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemoveGrantedWeaponAbility( TArray<FGameplayAbilitySpecHandle> InSpecHandlesToRemove);

	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
};
