// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnUIComponent.h"
#include "HeroUIComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeaponChangedDelegate, TSoftObjectPtr<UTexture2D>, SoftWeaponIcon);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API UHeroUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable)
	FOnPercentChangedDelegate OnCurrentRageChangedDelegate;

	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FOnEquippedWeaponChangedDelegate OnEquippedWeaponChangedDelegate;
};
