// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/EnemyUIComponent.h"

#include "Widgets/WarriorWidgetBase.h"


void UEnemyUIComponent::RegisterDrawnWidget(UWarriorWidgetBase* InDrawnWidget)
{
	EnemyDrawnWidgets.Add(InDrawnWidget);
}

void UEnemyUIComponent::RemoveDrawnWidgetIfAny()
{
	if(EnemyDrawnWidgets.IsEmpty()) return;

	for(UWarriorWidgetBase* EnemyWidget : EnemyDrawnWidgets)
	{
		if(EnemyWidget)
		{
			EnemyWidget->RemoveFromParent();
		}
	}

	EnemyDrawnWidgets.Empty();
}
