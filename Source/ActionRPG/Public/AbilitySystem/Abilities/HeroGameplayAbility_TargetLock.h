// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"

class UInputMappingContext;
class UWarriorWidgetBase;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InInputTag);
	
private:

	void TryLockOnTarget();
	void GetAvailableTargetsToLock();

	AActor* GetNearestActorFromAvailableTargets(const TArray<AActor*>& InAvailableTargets);
	void GetAvailableActorsAroundLockedTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight);

	void CancelTargetLockAbility();
	void CleanUp();

	void DrawTargetLockWidget();
	void SetTargetLockWidgetViewportPosition();

	void InitTargetLockMovement();
	void ResetTargetLockMovement();

	void InitTargetLockMappingContext();
	void ResetTargetLockMappingContext();
	
	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	float BoxTraceDistance = 5000.f;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	FVector BoxTraceSize = FVector(2000.f, 2000.f, 300.f);

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceChannels;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	bool bDrawDebugPersistant = false;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	TSubclassOf<UWarriorWidgetBase> TargetLockWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	float TargetLockRotationInterpSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	float TargetLockMaxWalkSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	TObjectPtr<UInputMappingContext> TargetLockMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Target Lock")
	float TargetLockCameraOffsetDistance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category="Target Lock")
	float SwitchDelayTime = 2.f;

	UPROPERTY()
	float SwitchDelayTimeCounter = 0.f;

	UPROPERTY()
	TArray<AActor*> AvailableTargetsArray;

	UPROPERTY()
	TObjectPtr<AActor> CurrentLockedTarget = nullptr;

	UPROPERTY()
	TObjectPtr<UWarriorWidgetBase> DrawnTargetLockWidget = nullptr;

	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;

	UPROPERTY()
	float CachedDefaultWalkSpeed = -1.f;
};
