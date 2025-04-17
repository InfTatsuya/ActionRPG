// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Interfaces/PawnCombatInterface.h"

UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetAbilitySystemComponentFromActor(AActor* InActor)
{
	check(InActor);

	return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetAbilitySystemComponentFromActor(InActor);

	if(!ASC->HasMatchingGameplayTag(TagToAdd))
	{
		ASC->AddLooseGameplayTag(TagToAdd);
	}
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetAbilitySystemComponentFromActor(InActor);

	if(ASC->HasMatchingGameplayTag(TagToRemove))
	{
		ASC->RemoveLooseGameplayTag(TagToRemove);
	}
}

bool UWarriorFunctionLibrary::NativeDoesActorHasTag(AActor* InActor, FGameplayTag TagToCheck)
{
	UWarriorAbilitySystemComponent* ASC = NativeGetAbilitySystemComponentFromActor(InActor);

	return ASC->HasMatchingGameplayTag(TagToCheck);
}

void UWarriorFunctionLibrary::BP_DoesActorHasTag(AActor* InActor, FGameplayTag TagToCheck,
	EWarriorConfirmType& OutConfirmType)
{
	OutConfirmType = NativeDoesActorHasTag(InActor, TagToCheck) ?  EWarriorConfirmType::Yes : EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
	check(InActor);

	if(IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
	{
		return PawnCombatInterface->GetPawnCombatComponent();
	}

	return nullptr;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor,
	EWarriorValidType& OutValidType)
{
	UPawnCombatComponent* FoundComponent = NativeGetPawnCombatComponentFromActor(InActor);
	OutValidType = FoundComponent ? EWarriorValidType::Valid : EWarriorValidType::Invalid;
	return FoundComponent;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(const APawn* QueryPawn, const APawn* TargetPawn)
{
	if(!QueryPawn || !TargetPawn) return false;

	IGenericTeamAgentInterface* QueryAgentInterface = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
	IGenericTeamAgentInterface* TargetAgentInterface = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());

	if(QueryAgentInterface && TargetAgentInterface)
	{
		return QueryAgentInterface->GetGenericTeamId() != TargetAgentInterface->GetGenericTeamId();
	}
	
	return false;
}
