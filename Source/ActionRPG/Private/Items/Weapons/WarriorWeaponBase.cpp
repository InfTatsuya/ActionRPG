// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/WarriorWeaponBase.h"

#include "DebugHeader.h"
#include "WarriorFunctionLibrary.h"
#include "Components/BoxComponent.h"


AWarriorWeaponBase::AWarriorWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	WeaponCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	WeaponCollisionBox->SetupAttachment(GetRootComponent());
	WeaponCollisionBox->SetBoxExtent(FVector(20.f));
	WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxBeginOverlapped);
	WeaponCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnCollisionBoxEndOverlapped);
}

void AWarriorWeaponBase::OnCollisionBoxBeginOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OwningWeaponPawn = GetInstigator<APawn>();
	checkf(OwningWeaponPawn, TEXT("Weapon without Owning Pawn"));

	if(APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if(UWarriorFunctionLibrary::IsTargetPawnHostile(OwningWeaponPawn, HitPawn))
		{
			OnWeaponHitTargetDelegate.ExecuteIfBound(OtherActor);
		}
	}
}

void AWarriorWeaponBase::OnCollisionBoxEndOverlapped(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* OwningWeaponPawn = GetInstigator<APawn>();
	checkf(OwningWeaponPawn, TEXT("Weapon without Owning Pawn"));

	if(APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if(UWarriorFunctionLibrary::IsTargetPawnHostile(OwningWeaponPawn, HitPawn))
		{
			OnWeaponPullFromTargetDelegate.ExecuteIfBound(OtherActor);
		}
	}
}


