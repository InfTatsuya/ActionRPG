// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PawnUIComponent.h"
#include "EnemyUIComponent.generated.h"


class UWarriorWidgetBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONRPG_API UEnemyUIComponent : public UPawnUIComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void RegisterDrawnWidget(UWarriorWidgetBase* InDrawnWidget);

	UFUNCTION(BlueprintCallable)
	void RemoveDrawnWidgetIfAny();

private:

	TArray<UWarriorWidgetBase*> EnemyDrawnWidgets;
};
