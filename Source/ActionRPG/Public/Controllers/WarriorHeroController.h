// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/PlayerController.h"
#include "WarriorHeroController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWarriorHeroController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	AWarriorHeroController();

	//Implements IGenericTeamAgentInterface Interface
	virtual FGenericTeamId GetGenericTeamId() const override;
	//End Interface

private:

	FGenericTeamId HeroTeamId;
	
};
