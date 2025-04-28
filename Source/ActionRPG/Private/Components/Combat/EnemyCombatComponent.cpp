// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyCombatComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Characters/WarriorEnemyCharacter.h"
#include "Components/BoxComponent.h"


void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	Super::OnHitTargetActor(HitActor);

	if (OverlappedActors.Contains(HitActor)) return;

	OverlappedActors.AddUnique(HitActor);

	bool bIsValidBlock = false;

	const bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHasTag(HitActor, WarriorGameplayTags::Player_Status_Blocking);
	const bool bIsAttackUnblockable = UWarriorFunctionLibrary::NativeDoesActorHasTag(GetOwningPawn(), WarriorGameplayTags::Enemy_Status_Unblockable);

	if(bIsPlayerBlocking && !bIsAttackUnblockable)
	{
		bIsValidBlock = UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	
	if(bIsValidBlock)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			EventData);
	}
	else
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarriorGameplayTags::Shared_Event_MeleeHit,
			EventData);
	}
}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	Super::ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);

	AWarriorEnemyCharacter* OwningEnemyCharacter = GetOwningPawn<AWarriorEnemyCharacter>();
 
	check(OwningEnemyCharacter);
 
	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter->GetRightHandCollisionBox();
 
	check(LeftHandCollisionBox && RightHandCollisionBox);
 
	switch (ToggleDamageType)
	{
	case EToggleDamageType::LeftHand:
		LeftHandCollisionBox->SetCollisionEnabled(bShouldEnable? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;
 
	case EToggleDamageType::RightHand:
		RightHandCollisionBox->SetCollisionEnabled(bShouldEnable? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;
 
	default:
		break;
	}
 
	if (!bShouldEnable)
	{
		OverlappedActors.Empty();
	}
}
