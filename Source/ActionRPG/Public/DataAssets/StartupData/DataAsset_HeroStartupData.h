// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorTypes/WarriorStructTypes.h"
#include "DataAssets/StartupData/DataAsset_StartupDataBase.h"
#include "DataAsset_HeroStartupData.generated.h"


/**
 * 
 */
UCLASS()
class ACTIONRPG_API UDataAsset_HeroStartupData : public UDataAsset_StartupDataBase
{
	GENERATED_BODY()

public:

	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, int32 Level) override;
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "StartUpData", meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroAbilitySet> HeroStartUpAbilitySets;
};
