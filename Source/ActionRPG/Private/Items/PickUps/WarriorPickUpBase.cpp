// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUps/WarriorPickUpBase.h"

#include "Components/SphereComponent.h"


AWarriorPickUpBase::AWarriorPickUpBase()
{
	PrimaryActorTick.bCanEverTick = false;

	SphereCollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollisionComponent"));
	SetRootComponent(SphereCollisionComponent);
	SphereCollisionComponent->InitSphereRadius(60.f);
	SphereCollisionComponent->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnSphereCollisionBeginOverlap);
}

void AWarriorPickUpBase::OnSphereCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

