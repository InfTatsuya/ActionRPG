// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"
#include "Items/Weapons/WarriorWeaponBase.h"

#include "DebugHeader.h"
#include "Components/BoxComponent.h"

void UPawnCombatComponent::RegisterSpawnedWeapon(const FGameplayTag& InWeaponTagToRegister,
                                                 AWarriorWeaponBase* InWeaponToRegister, bool bRegisterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTagToRegister), TEXT("A tag named %s has already been added as carried weapon"),*InWeaponTagToRegister.ToString());
	check(InWeaponToRegister);

	CharacterCarriedWeaponMap.Emplace(InWeaponTagToRegister, InWeaponToRegister);

	InWeaponToRegister->OnWeaponHitTargetDelegate.BindUObject(this, &UPawnCombatComponent::OnHitTargetActor);
	InWeaponToRegister->OnWeaponPullFromTargetDelegate.BindUObject(this, &UPawnCombatComponent::OnWeaponPulledFromTargetActor);
	
	if(bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTagToRegister;
	}
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(const FGameplayTag& InWeaponTag) const
{
	if(CharacterCarriedWeaponMap.Contains(InWeaponTag))
	{
		if(AWarriorWeaponBase* const* FoundWeapon = CharacterCarriedWeaponMap.Find(InWeaponTag))
		{
			return *FoundWeapon;
		}
	}

	return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if(!CurrentEquippedWeaponTag.IsValid())
	{
		return nullptr;
	}

	return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToogleDamageType)
{
	AWarriorWeaponBase* CurrentWeapon = GetCharacterCurrentEquippedWeapon();
	if(!CurrentWeapon)
	{
		DebugHeader::Print(TEXT("Dont have any weapon"));
		return;
	}
	
	if(bShouldEnable)
	{
		CurrentWeapon->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		CurrentWeapon->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlappedActors.Empty();
	}
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{
}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}
