// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/WarriorHeroCharacter.h"

#include "DebugHeader.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/LocalPlayer.h"
#include "EnhancedInputSubsystems.h"
#include "WarriorGameplayTags.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "Components/Combat/HeroCombatComponent.h"
#include "Components/Input/WarriorInputComponent.h"
#include "DataAssets/Input/DataAsset_InputConfig.h"
#include "DataAssets/StartupData/DataAsset_StartupDataBase.h"

AWarriorHeroCharacter::AWarriorHeroCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.0f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>(TEXT("HeroCombatComponent"));
}

void AWarriorHeroCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWarriorHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	checkf(InputConfigDataAsset, TEXT("Input Data Asset in not assigned"));

	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);

	checkf(SubSystem, TEXT("EnhancedInputLocalPlayerSubsystem is not valid"));

	SubSystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);

	UWarriorInputComponent* WarriorInputComponent = CastChecked<UWarriorInputComponent>(PlayerInputComponent);

	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::InputMove);
	WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::InputLook);

	WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset, this, &ThisClass::InputAbilityPressed, &AWarriorHeroCharacter::InputAbilityReleased);
}

void AWarriorHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if(!CharacterStartUpData.IsNull())
	{
		if(UDataAsset_StartupDataBase* LoadedData = CharacterStartUpData.LoadSynchronous())
		{
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
		}
	}
}

#pragma region Input

void AWarriorHeroCharacter::InputMove(const FInputActionValue& InputValue)
{
	FVector2D InputMove = InputValue.Get<FVector2D>();
	FRotator ControllerRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);

	if(InputMove.Y != 0)
	{
		FVector ForwardVector = ControllerRotator.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardVector, InputMove.Y);
	}
	if(InputMove.X != 0)
	{
		FVector RightVector = ControllerRotator.RotateVector(FVector::RightVector);

		AddMovementInput(RightVector, InputMove.X);
	}
}

void AWarriorHeroCharacter::InputLook(const FInputActionValue& InputValue)
{
	FVector2D InputLook = InputValue.Get<FVector2D>();

	if(InputLook.X != 0)
	{
		AddControllerYawInput(InputLook.X);
	}

	if(InputLook.Y != 0)
	{
		AddControllerPitchInput(InputLook.Y);
	}
}

void AWarriorHeroCharacter::InputAbilityPressed(FGameplayTag InputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InputTag);
}

void AWarriorHeroCharacter::InputAbilityReleased(FGameplayTag InputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InputTag);
}

#pragma endregion
