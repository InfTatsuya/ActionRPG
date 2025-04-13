// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h" 
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "WarriorBaseCharacter.generated.h"

class UDataAsset_StartupDataBase;
class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;

UCLASS()
class ACTIONRPG_API AWarriorBaseCharacter : public ACharacter, public IAbilitySystemInterface, public IPawnCombatInterface, public IPawnUIInterface
{
	GENERATED_BODY()

public:
	
	AWarriorBaseCharacter();

	// IAbilitySystemInterface implementations
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// End interfaces

	// IPawnCombatInterface implementations
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	// End interfaces

	// IPawnUIInterface implementations
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	// End interfaces
	
protected:

	// APawn override
	virtual void PossessedBy(AController* NewController) override;
	// end APawn

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UWarriorAbilitySystemComponent> WarriorAbilitySystemComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AbilitySystem")
	TObjectPtr<UWarriorAttributeSet> WarriorAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartupDataBase> CharacterStartUpData;

public:

	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent() const { return WarriorAbilitySystemComponent; }
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttribute() const { return WarriorAttributeSet; }
};
