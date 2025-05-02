// Fill out your copyright notice in the Description page of Project Settings.


#include "GameMode/WarriorSurvivalGameMode.h"

#include "DebugHeader.h"
#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

void AWarriorSurvivalGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(CurrentGameState == EWarriorSurvivalGameState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaSeconds;

		if(TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;
			SetSurvivalGameState(EWarriorSurvivalGameState::SpawningNewWave);
		}
	}

	if(CurrentGameState == EWarriorSurvivalGameState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaSeconds;

		if(TimePassedSinceStart >= SpawnEnemiesDelayTime)
		{
			CurrentAliveEnemiesCounter += TrySpawnNewEnemies();
			
			TimePassedSinceStart = 0.f;
			SetSurvivalGameState(EWarriorSurvivalGameState::InProgress);
		}
	}

	if(CurrentGameState == EWarriorSurvivalGameState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaSeconds;

		if(TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;
			CurrentWaveCount++;

			if(HasFinishedAllWaves())
			{
				SetSurvivalGameState(EWarriorSurvivalGameState::AllWavesDone);
			}
			else
			{
				SetSurvivalGameState(EWarriorSurvivalGameState::WaitSpawnNewWave);

				PreloadEnemiesNextWave();
			}
		}
	}
}

void AWarriorSurvivalGameMode::RegisterSummonEnemies(const TArray<AWarriorEnemyCharacter*>& EnemiesArray)
{
	for(AWarriorEnemyCharacter* SummonEnemy : EnemiesArray)
	{
		if(!SummonEnemy) continue;
		
		CurrentAliveEnemiesCounter++;
		SummonEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
	}
}

void AWarriorSurvivalGameMode::BeginPlay()
{
	Super::BeginPlay();

	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign enemy wave spawner table in game mode BP"));
	
	TotalWaveToSpawn = EnemyWaveSpawnerDataTable->GetRowNames().Num();

	SetSurvivalGameState(EWarriorSurvivalGameState::WaitSpawnNewWave);

	PreloadEnemiesNextWave();
}

void AWarriorSurvivalGameMode::SetSurvivalGameState(EWarriorSurvivalGameState NewGameState)
{
	CurrentGameState = NewGameState;

	OnSurvivalGameStateChangedDelegate.Broadcast(CurrentGameState);
}

bool AWarriorSurvivalGameMode::HasFinishedAllWaves() const
{
	return CurrentWaveCount > TotalWaveToSpawn;
}

void AWarriorSurvivalGameMode::PreloadEnemiesNextWave()
{
	if(HasFinishedAllWaves()) return;

	for(const auto& WaveInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if(WaveInfo.SoftEnemyClassToSpawn.IsNull()) continue;
		if(PreloadedEnemiesMap.Contains(WaveInfo.SoftEnemyClassToSpawn)) continue;

		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			WaveInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[this, WaveInfo]()
				{
					if(UClass* LoadedClass = WaveInfo.SoftEnemyClassToSpawn.Get())
					{
						PreloadedEnemiesMap.Emplace(WaveInfo.SoftEnemyClassToSpawn, LoadedClass);
					}
				}));
	}
}

FWarriorEnemyWaveSpawnerTableRow* AWarriorSurvivalGameMode::GetCurrentWaveSpawnerTableRow() const
{
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));
	FWarriorEnemyWaveSpawnerTableRow* FoundRow = EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, "");

	checkf(FoundRow, TEXT("Row Data for wave %d is invalid"), CurrentWaveCount);

	return FoundRow;
}

int32 AWarriorSurvivalGameMode::TrySpawnNewEnemies()
{
	if(TargetPointsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPointsArray);
	}

	checkf(!TargetPointsArray.IsEmpty(), TEXT("Could not found any target point in level: %s"), *GetWorld()->GetName());

	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for(const auto& WaveInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if(WaveInfo.SoftEnemyClassToSpawn.IsNull()) continue;

		const int32 NumToSpawn = FMath::RandRange(WaveInfo.MinPerSpawnCount, WaveInfo.MaxPerSpawnCount);
		UClass* LoadedEnemyClass = PreloadedEnemiesMap.FindChecked(WaveInfo.SoftEnemyClassToSpawn);

		for(int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomIndex = FMath::RandRange(0, TargetPointsArray.Num() - 1);
			const FVector OriginLocation = TargetPointsArray[RandomIndex]->GetActorLocation();
			const FRotator SpawnRotator = TargetPointsArray[RandomIndex]->GetActorForwardVector().ToOrientationRotator();

			FVector SpawnLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(this, OriginLocation, SpawnLocation, 400.f);
			SpawnLocation += FVector(0.f, 0.f, 150.f); //lift actor up 150cm to prevent stuck in ground!!

			AWarriorEnemyCharacter* SpawnedEnemy = GetWorld()->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass, SpawnLocation, SpawnRotator, SpawnParameters);
			if(SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
				
				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}

			if(!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}
		}
	}

	return EnemiesSpawnedThisTime;
}

bool AWarriorSurvivalGameMode::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisWaveCounter < GetCurrentWaveSpawnerTableRow()->TotalEnemySpawnInWave;
}

void AWarriorSurvivalGameMode::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentAliveEnemiesCounter--;

	if(ShouldKeepSpawnEnemies())
	{
		CurrentAliveEnemiesCounter += TrySpawnNewEnemies();
	}
	else if(CurrentAliveEnemiesCounter <= 0)
	{
		CurrentAliveEnemiesCounter = 0;
		TotalSpawnedEnemiesThisWaveCounter = 0;

		SetSurvivalGameState(EWarriorSurvivalGameState::WaveCompleted);
	}
}
