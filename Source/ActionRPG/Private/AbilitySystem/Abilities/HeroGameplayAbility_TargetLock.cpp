// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"
#include "Characters/WarriorHeroCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/WarriorWidgetBase.h"
#include "Controllers/WarriorHeroController.h"

#include "DebugHeader.h"
#include "EnhancedInputSubsystems.h"
#include "WarriorFunctionLibrary.h"
#include "WarriorGameplayTags.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/SizeBox.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                                      const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                                      const FGameplayEventData* TriggerEventData)
{
	TryLockOnTarget();
	InitTargetLockMovement();
	InitTargetLockMappingContext();
	
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	ResetTargetLockMappingContext();
	CleanUp();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	SwitchDelayTimeCounter -= DeltaTime;

	//DebugHeader::Print("TargetLock Switch Delay Time:", SwitchDelayTimeCounter);
	
	if(!CurrentLockedTarget ||
		UWarriorFunctionLibrary::NativeDoesActorHasTag(CurrentLockedTarget, WarriorGameplayTags::Shared_Status_Dead) ||
		UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Dead))
	{
		CancelTargetLockAbility();
		return;
	}

	SetTargetLockWidgetViewportPosition();

	bool bShouldOverrideRotation =
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Rolling) &&
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Blocking);

	if(bShouldOverrideRotation)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetHeroCharacterFromActorInfo()->GetActorLocation(), CurrentLockedTarget->GetActorLocation());
		LookAtRot -= FRotator(TargetLockCameraOffsetDistance, 0.f, 0.f);
		const FRotator CurrentRot = GetHeroControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, TargetLockRotationInterpSpeed);

		GetHeroControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}

void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InInputTag)
{
	if(SwitchDelayTimeCounter > 0.f) return;

	SwitchDelayTimeCounter = SwitchDelayTime;
	
	GetAvailableTargetsToLock();

	TArray<AActor*> ActorsOnLeft;
	TArray<AActor*> ActorsOnRight;
	AActor* NewTargetActor = nullptr;

	GetAvailableActorsAroundLockedTarget(ActorsOnLeft, ActorsOnRight);
	
	if(InInputTag == WarriorGameplayTags::Player_Event_SwitchTarget_Right)
	{
		NewTargetActor = GetNearestActorFromAvailableTargets(ActorsOnRight);
	}
	else
	{
		NewTargetActor = GetNearestActorFromAvailableTargets(ActorsOnLeft);
	}

	if(NewTargetActor)
	{
		CurrentLockedTarget = NewTargetActor;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableTargetsToLock();

	if(AvailableTargetsArray.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	CurrentLockedTarget = GetNearestActorFromAvailableTargets(AvailableTargetsArray);
	if(!CurrentLockedTarget)
	{
		CancelTargetLockAbility();
	}
	else
	{
		DrawTargetLockWidget();

		SetTargetLockWidgetViewportPosition();
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableTargetsToLock()
{
	AvailableTargetsArray.Empty();
	
	AWarriorHeroCharacter* HeroCharacter = GetHeroCharacterFromActorInfo();
	TArray<FHitResult> HitResultsArray;
	
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		HeroCharacter,
		HeroCharacter->GetActorLocation(),
		HeroCharacter->GetActorLocation() + HeroCharacter->GetActorForwardVector() * BoxTraceDistance,
		BoxTraceSize / 2.f,
		HeroCharacter->GetActorForwardVector().ToOrientationRotator(),
		TraceChannels,
		false,
		TArray<AActor*>(),
		bDrawDebugPersistant ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		HitResultsArray,
		true
		);

	for(const FHitResult& HitResult : HitResultsArray)
	{
		if(AActor* HitActor = HitResult.GetActor())
		{
			if(HitActor != HeroCharacter)
			{
				AvailableTargetsArray.AddUnique(HitActor);

				//DebugHeader::Print(HitActor->GetActorNameOrLabel());
			}
		}
	}
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundLockedTarget(TArray<AActor*>& OutActorsOnLeft,
	TArray<AActor*>& OutActorsOnRight)
{
	if(!CurrentLockedTarget || AvailableTargetsArray.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}

	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (PlayerLocation - CurrentLockedTarget->GetActorLocation()).GetSafeNormal();

	for(auto AvailableActor : AvailableTargetsArray)
	{
		if(!AvailableActor || AvailableActor == CurrentLockedTarget) continue;
		
		const FVector PlayerToAvailableNormalized = (PlayerLocation - AvailableActor->GetActorLocation()).GetSafeNormal();
		const FVector CrossProduct = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);

		if(CrossProduct.Z > 0.f)
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestActorFromAvailableTargets(const TArray<AActor*>& InAvailableTargets)
{
	float MinDistance = FLT_MAX;
	return UGameplayStatics::FindNearestActor(GetHeroCharacterFromActorInfo()->GetActorLocation(), InAvailableTargets, MinDistance);
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}

void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableTargetsArray.Empty();
	CurrentLockedTarget = nullptr;

	if(DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}

	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;

	CachedDefaultWalkSpeed = -1.f;
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if(!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forget assign TargetLockWidgetClass in blueprint"));

		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetHeroControllerFromActorInfo(), TargetLockWidgetClass);

		if(DrawnTargetLockWidget)
		{
			DrawnTargetLockWidget->AddToViewport();
		}
	}
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetViewportPosition()
{
	if(!DrawnTargetLockWidget || !CurrentLockedTarget)
	{
		CancelTargetLockAbility();
		return;
	}

	if(TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
		[this](UWidget* FoundWidget)
			{
				if(USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			});
	}

	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(GetHeroControllerFromActorInfo(), CurrentLockedTarget->GetActorLocation(), ScreenPosition, true);
	ScreenPosition -= TargetLockWidgetSize / 2.f;

	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	CachedDefaultWalkSpeed = GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	if(CachedDefaultWalkSpeed > 0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultWalkSpeed;
	}
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(InputSubsystem && TargetLockMappingContext);

	InputSubsystem->AddMappingContext(TargetLockMappingContext, 3);
	
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if(!GetHeroControllerFromActorInfo()) return;

	const ULocalPlayer* LocalPlayer = GetHeroControllerFromActorInfo()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(InputSubsystem && TargetLockMappingContext);

	InputSubsystem->RemoveMappingContext(TargetLockMappingContext);
}
