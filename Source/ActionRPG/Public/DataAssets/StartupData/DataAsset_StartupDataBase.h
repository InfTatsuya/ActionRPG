// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartupDataBase.generated.h"

class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UDataAsset_StartupDataBase : public UDataAsset
{
	GENERATED_BODY()

public:

	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, int32 Level = 1); 

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Abilities")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ActivateOnGivenAbilities;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Abilities")
	TArray<TSubclassOf<UWarriorGameplayAbility>> ReactiveAbilities;

	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& AbilitiesArray, UWarriorAbilitySystemComponent* InASC, int32 Level = 1);
};
