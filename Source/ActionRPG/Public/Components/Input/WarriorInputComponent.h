// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "WarriorInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONRPG_API UWarriorInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserObject, typename CallbackFunc>
	void BindNativeInputAction(UDataAsset_InputConfig* InputConfigData, const FGameplayTag& InGameplayTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func);
};

template <class UserObject, typename CallbackFunc>
void UWarriorInputComponent::BindNativeInputAction(UDataAsset_InputConfig* InputConfigData,
	const FGameplayTag& InGameplayTag, ETriggerEvent TriggerEvent, UserObject* ContextObject, CallbackFunc Func)
{
	checkf(InputConfigData, TEXT("InputConfigData is NULL"));

	if(UInputAction* FoundInputAction = InputConfigData->FindNativeInputActionByTag(InGameplayTag))
	{
		BindAction(FoundInputAction, TriggerEvent, ContextObject, Func);
	}
}
