// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "WarriorTypes/WarriorStructTypes.h"

void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InInputTag)
{
	if(!InInputTag.IsValid()) return;

	for(const auto& AbilitySpec : GetActivatableAbilities())
	{
		if(!AbilitySpec.DynamicAbilityTags.HasTagExact(InInputTag)) continue;

		TryActivateAbility(AbilitySpec.Handle);
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InInputTag)
{
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& DefaultAbilitySet,
	int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantedAbilitySpecHandle)
{
	if(DefaultAbilitySet.IsEmpty()) return;

	for(const auto& AbilitySet : DefaultAbilitySet)
	{
		if(!AbilitySet.IsValid()) continue;

		FGameplayAbilitySpec AbilitySpec(AbilitySet.AbilityToGrant);
		AbilitySpec.SourceObject = GetAvatarActor();
		AbilitySpec.Level = ApplyLevel;
		AbilitySpec.DynamicAbilityTags.AddTag(AbilitySet.InputTag);

		OutGrantedAbilitySpecHandle.AddUnique(GiveAbility(AbilitySpec));
	}
}

void UWarriorAbilitySystemComponent::RemoveGrantedWeaponAbility(
	TArray<FGameplayAbilitySpecHandle> InSpecHandlesToRemove)
{
	if(InSpecHandlesToRemove.IsEmpty()) return;

	for(const auto& AbilitySpecHandle : InSpecHandlesToRemove)
	{
		if(AbilitySpecHandle.IsValid())
		{
			ClearAbility(AbilitySpecHandle);
		}
	}

	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());

	TArray<FGameplayAbilitySpec*> FoundAbilitySpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundAbilitySpecs);

	if(!FoundAbilitySpecs.IsEmpty())
	{
		int32 RandomIndex = FMath::RandRange(0, FoundAbilitySpecs.Num() - 1);
		FGameplayAbilitySpec* AbilitySpec = FoundAbilitySpecs[RandomIndex];

		check(AbilitySpec);

		if(!AbilitySpec->IsActive())
		{
			return TryActivateAbility(AbilitySpec->Handle);
		}
	}

	return false;
}
