// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "Items/Weapons/WarriorWeaponBase.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "WarriorHeroWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWarriorHeroWeapon : public AWarriorWeaponBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WeaponData")
	FWarriorHeroWeaponData LinkedWeaponData;

	UFUNCTION(BlueprintCallable)
	void AssignGrantedAbilitySpecHandlesArray(const TArray<FGameplayAbilitySpecHandle>& InArray);

	UFUNCTION(BlueprintPure)
	TArray<FGameplayAbilitySpecHandle> GetGrantedAbilitySpecHandles() const;
	
private:

	TArray<FGameplayAbilitySpecHandle> GrantedAbilitySpecHandles;
};
