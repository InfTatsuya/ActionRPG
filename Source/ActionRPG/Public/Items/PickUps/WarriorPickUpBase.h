// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WarriorPickUpBase.generated.h"

class USphereComponent;

UCLASS()
class ACTIONRPG_API AWarriorPickUpBase : public AActor
{
	GENERATED_BODY()

public:
	AWarriorPickUpBase();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pick Up Interact")
	TObjectPtr<USphereComponent> SphereCollisionComponent;

	UFUNCTION()
	virtual void OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
