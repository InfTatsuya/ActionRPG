// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WarriorBaseCharacter.h"
#include "WarriorEnemyCharacter.generated.h"

class UWidgetComponent;
class UEnemyUIComponent;
class UEnemyCombatComponent;

UCLASS()
class ACTIONRPG_API AWarriorEnemyCharacter : public AWarriorBaseCharacter
{
	GENERATED_BODY()

public:

	AWarriorEnemyCharacter();

	// IPawnCombatInterface implementations
	virtual UPawnCombatComponent* GetPawnCombatComponent() const override;
	// End interfaces

	// IPawnUIInterface implementations
	virtual UPawnUIComponent* GetPawnUIComponent() const override;
	virtual UEnemyUIComponent* GetEnemyUIComponent() const override;
	// End interfaces

protected:

	virtual void BeginPlay() override;

	// APawn override
	virtual void PossessedBy(AController* NewController) override;
	// end APawn

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<UEnemyCombatComponent> EnemyCombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UEnemyUIComponent> EnemyUIComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UWidgetComponent> EnemyHealthWidgetComponent;

private:

	void InitEnemyStartupData();

public:

	FORCEINLINE UEnemyCombatComponent* GetEnemyCombatComponent() const { return EnemyCombatComponent; }
}; 
	
