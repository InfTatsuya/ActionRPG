// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorPickUpBase.h"
#include "WarriorStoneBase.generated.h"

class UGameplayEffect;
class UWarriorAbilitySystemComponent;

UCLASS()
class ACTIONRPG_API AWarriorStoneBase : public AWarriorPickUpBase
{
	GENERATED_BODY()

public:

	void Consume(UWarriorAbilitySystemComponent* InASC, int32 ApplyLevel);

protected:

	virtual void OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "On Consume Stone"))
	void BP_OnConsumeStone();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StoneGameplayEffectClass;
};
