// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimInstance.h"
#include "WarriorBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	bool DoesOwnerHasTag(FGameplayTag TagToCheck) const;
};
