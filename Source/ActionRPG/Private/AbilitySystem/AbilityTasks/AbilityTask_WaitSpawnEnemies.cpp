// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"
#include "AbilitySystemComponent.h"
#include "NavigationSystem.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Engine/AssetManager.h"

UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility,
                                                                               FGameplayTag EventTag, TSoftClassPtr<AWarriorEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn,
                                                                               const FVector& SpawnOrigin, float RandomSpawnRadius)
{
	UAbilityTask_WaitSpawnEnemies* Node = NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	
	return Node;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	DelegateHandle = Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);
	
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* EventData)
{
	if(ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(CachedSoftEnemyClassToSpawn.ToSoftObjectPath(), FStreamableDelegate::CreateUObject(this, &ThisClass::OnLoadedEnemyClass));
	}
	else
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnDidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}

		EndTask();
	}
}

void UAbilityTask_WaitSpawnEnemies::OnLoadedEnemyClass()
{
	UClass* LoadedEnemyClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();

	if(!LoadedEnemyClass || !World)
	{
		if(ShouldBroadcastAbilityTaskDelegates())
		{
			OnDidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());

		}
		EndTask();

		return;
	}

	TArray<AWarriorEnemyCharacter*> SpawnedEnemies;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for(int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomReachablePointInRadius(this, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);
		RandomLocation += FVector(0.f, 0.f, 150.f); //to prevent spawned actor stuck in ground

		const FRotator EnemyRotator = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();
		
		AWarriorEnemyCharacter* NewEnemy = World->SpawnActor<AWarriorEnemyCharacter>(LoadedEnemyClass, RandomLocation, EnemyRotator, SpawnParams);

		if(NewEnemy)
		{
			SpawnedEnemies.Add(NewEnemy);
		}
	}

	if(ShouldBroadcastAbilityTaskDelegates())
	{
		if(SpawnedEnemies.IsEmpty())
		{
			OnDidNotSpawn.Broadcast(TArray<AWarriorEnemyCharacter*>());
		}
		else
		{
			OnFinishedSpawnEnemies.Broadcast(SpawnedEnemies);
		}
	}

	EndTask();
}
