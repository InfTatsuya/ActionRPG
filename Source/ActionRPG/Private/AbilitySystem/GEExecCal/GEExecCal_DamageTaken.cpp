// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GEExecCal/GEExecCal_DamageTaken.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAttributeSet.h"

struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower);
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken);

	FWarriorDamageCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false);
	}
};

static const FWarriorDamageCapture& GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}

UGEExecCal_DamageTaken::UGEExecCal_DamageTaken()
{
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

void UGEExecCal_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParams.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().AttackPowerDef, EvaluateParams, SourceAttackPower);
	//DebugHeader::Print(TEXT("SourceAttackPower"),SourceAttackPower);
	
	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(GetWarriorDamageCapture().DefensePowerDef, EvaluateParams, TargetDefensePower);
	//DebugHeader::Print(TEXT("TargetDefensePower"),TargetDefensePower);
	
	float BaseDamage = 0.f;
	int32 UsedComboCountLight = 0;
	int32 UsedComboCountHeavy = 0;

	for(const TPair<FGameplayTag, float>& TagMagnitude : EffectSpec.SetByCallerTagMagnitudes)
	{
		if(TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = TagMagnitude.Value;
			//DebugHeader::Print(TEXT("BaseDamage"),BaseDamage);
		}

		if(TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedComboCountLight = FMath::RoundToInt32(TagMagnitude.Value);
			//DebugHeader::Print(TEXT("UsedComboCountLight"),UsedComboCountLight);
		}

		if(TagMagnitude.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedComboCountHeavy = FMath::RoundToInt32(TagMagnitude.Value);
			//DebugHeader::Print(TEXT("UsedComboCountHeavy"),UsedComboCountHeavy);
		}
	}

	if(UsedComboCountLight > 0)
	{
		const float IncreaseDamageLightPercent = (UsedComboCountLight - 1) * 0.1f + 1.f;
		BaseDamage *= IncreaseDamageLightPercent;

		//DebugHeader::Print(TEXT("LightComboDamage"), BaseDamage);
	}

	if(UsedComboCountHeavy > 0)
	{
		const float IncreaseDamageHeavyPercent = UsedComboCountHeavy * 0.25f + 1.f;
		BaseDamage *= IncreaseDamageHeavyPercent;

		//DebugHeader::Print(TEXT("HeavyComboDamage"), BaseDamage);
	}

	const float FinalDamage = BaseDamage * SourceAttackPower / TargetDefensePower;
	//DebugHeader::Print(TEXT("FinalDamage"), FinalDamage);
	
	if(FinalDamage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetWarriorDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamage)
				);
	}
}
