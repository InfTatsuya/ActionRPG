// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "WarriorGameplayAbility.generated.h"

class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;

UENUM(BlueprintType)
enum class EWarriorGameplayAbilityActivationPolicy : uint8
{
	OnTrigger UMETA(DisplayName = "On Trigger"),
	OnGiven UMETA(DisplayName = "On Given"),

	MAX UMETA(DisplayName = "DefaultMAX")
};

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

	//Implement UGameplayAbility
public:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

protected:
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//End UGameplayAbility

	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	EWarriorGameplayAbilityActivationPolicy ActivationPolicy = EWarriorGameplayAbilityActivationPolicy::OnTrigger;

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo() const;

	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo() const;
};
