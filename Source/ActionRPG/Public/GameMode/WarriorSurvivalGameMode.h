// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameMode/WarriorBaseGameMode.h"
#include "WarriorSurvivalGameMode.generated.h"

class AWarriorEnemyCharacter;

UENUM(BlueprintType)
enum class EWarriorSurvivalGameState : uint8
{
	WaitSpawnNewWave,
	 SpawningNewWave,
	 InProgress,
	 WaveCompleted,
	 AllWavesDone,
	 PlayerDied
};

USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn;

	UPROPERTY(EditAnywhere)
	int32 MinPerSpawnCount = 1;

	UPROPERTY(EditAnywhere)
	int32 MaxPerSpawnCount = 3;
};

USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo> EnemyWaveSpawnerDefinitions;

	UPROPERTY(EditAnywhere)
	int32 TotalEnemySpawnInWave = 1;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvivalGameStateChangedDelegate, EWarriorSurvivalGameState, NewGameState);

/**
 * 
 */
UCLASS()
class ACTIONRPG_API AWarriorSurvivalGameMode : public AWarriorBaseGameMode
{
	GENERATED_BODY()

public:

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void RegisterSummonEnemies(const TArray<AWarriorEnemyCharacter*>& EnemiesArray);

protected:

	virtual void BeginPlay() override;

private:

	void SetSurvivalGameState(EWarriorSurvivalGameState NewGameState);

	bool HasFinishedAllWaves() const;
	void PreloadEnemiesNextWave();
	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow() const;
	int32 TrySpawnNewEnemies();
	bool ShouldKeepSpawnEnemies() const;

	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);
	
	UPROPERTY()
	EWarriorSurvivalGameState CurrentGameState;

	UPROPERTY(BlueprintAssignable, BlueprintCallable, meta=(AllowPrivateAccess= "true"))
	FOnSurvivalGameStateChangedDelegate OnSurvivalGameStateChangedDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Definitions", meta = (AllowPrivateAccess = "true"))
	UDataTable* EnemyWaveSpawnerDataTable;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Wave Definitions", meta = (AllowPrivateAccess = "true"))
	int32 TotalWaveToSpawn;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Wave Definitions", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWaveCount = 1;
 
	UPROPERTY()
	float TimePassedSinceStart = 0.f;

	UPROPERTY()
	int32 CurrentAliveEnemiesCounter = 0;
 
	UPROPERTY()
	int32 TotalSpawnedEnemiesThisWaveCounter = 0;
 
	UPROPERTY()
	TArray<AActor*> TargetPointsArray;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Definitions", meta = (AllowPrivateAccess = "true"))
	float SpawnNewWaveWaitTime = 5.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Definitions", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemiesDelayTime = 2.f;
 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Wave Definitions", meta = (AllowPrivateAccess = "true"))
	float WaveCompletedWaitTime = 5.f;

	UPROPERTY()
	TMap<TSoftClassPtr<AWarriorEnemyCharacter>, UClass*> PreloadedEnemiesMap;
	
};
