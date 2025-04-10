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

	template<class UserObject, typename CallbackFunc>
	void BindAbilityInputAction(UDataAsset_InputConfig* InputConfigData, UserObject* ContextObject, CallbackFunc InPressedFunc, CallbackFunc InReleasedFunc);
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

template <class UserObject, typename CallbackFunc>
void UWarriorInputComponent::BindAbilityInputAction(UDataAsset_InputConfig* InputConfigData, UserObject* ContextObject,
	CallbackFunc InPressedFunc, CallbackFunc InReleasedFunc)
{
	checkf(InputConfigData,TEXT("Input config data asset is null,can not proceed with binding"));

	for(const FWarriorInputActionConfig& InputActionConfig : InputConfigData->AbilityInputActionsArray)
	{
		if(!InputActionConfig.IsValid()) continue;

		BindAction(InputActionConfig.InputAction, ETriggerEvent::Started, ContextObject, InPressedFunc, InputActionConfig.InputTag);
		BindAction(InputActionConfig.InputAction, ETriggerEvent::Completed, ContextObject, InReleasedFunc, InputActionConfig.InputTag);
	}
}
