// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/HeroCombatComponent.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Items/Weapons/WarriorHeroWeapon.h"

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCarriedWeaponByTag(const FGameplayTag& WeaponTag) const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCarriedWeaponByTag(WeaponTag));
}

AWarriorHeroWeapon* UHeroCombatComponent::GetHeroCurrentEquippedWeapon() const
{
	return Cast<AWarriorHeroWeapon>(GetCharacterCurrentEquippedWeapon());
}

float UHeroCombatComponent::GetHeroCurrentEquippedWeaponDamageAtLevel(float InLevel) const
{
	return GetHeroCurrentEquippedWeapon()->LinkedWeaponData.BaseWeaponDamage.GetValueAtLevel(InLevel);
}

void UHeroCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if(OverlappedActors.Contains(HitActor)) return;

	OverlappedActors.AddUnique(HitActor);

	FGameplayEventData EventData;
	EventData.Instigator = GetOwningPawn();
	EventData.Target = HitActor;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Shared_Event_MeleeHit,
		EventData);

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause,
		FGameplayEventData());
}

void UHeroCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		GetOwningPawn(),
		WarriorGameplayTags::Player_Event_HitPause,
		FGameplayEventData());
	
}
