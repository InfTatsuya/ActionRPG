// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "WarriorAIController.generated.h"

class UAISenseConfig_Sight;
/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWarriorAIController : public AAIController
{
	GENERATED_BODY()

public:

	AWarriorAIController(const FObjectInitializer& ObjectInitializer);

	//Implements IGenericTeamAgentInterface Interface
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	//End Interface

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> EnemyPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAISenseConfig_Sight> AISenseConfig_Sight;

	UFUNCTION()
	virtual void OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance  Config")
	bool bEnableDetourCrowdAvoidance = true;

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance  Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance", UIMin = "1", UIMax = "4"))
	int32 DetourCrowdAvoidanceQuality = 4;

	UPROPERTY(EditDefaultsOnly, Category = "Detour Crowd Avoidance  Config", meta = (EditCondition = "bEnableDetourCrowdAvoidance"))
	float CollisionQueryRange = 600.f;
};
