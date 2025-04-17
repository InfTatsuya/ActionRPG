// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_RotateToFaceTarget.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName = "Native Rotate To Face Target";

	AnglePrecision = 10.f;
	RotationInterpSpeed = 10.f;

	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;

	INIT_TASK_NODE_NOTIFY_FLAGS();

	InTargetActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetActorKey), AActor::StaticClass());
}

void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if(UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetActorKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetTaskMemory);
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	const FString KeyDescription = InTargetActorKey.SelectedKeyName.ToString();
 
	return FString::Printf(TEXT("Smoothly rotates to face %s Key until the angle precision: %s is reached"),*KeyDescription,*FString::SanitizeFloat(AnglePrecision));
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetActorKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(TargetObject);

	APawn* OwningPawn = Cast<APawn>(OwnerComp.GetAIOwner()->GetPawn());

	FRotateToFaceTargetTaskMemory* TaskMemory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	if(!TaskMemory) return EBTNodeResult::Failed;
	
	TaskMemory->OwningPawn = OwningPawn;
	TaskMemory->TargetActor = TargetActor;
	if(!TaskMemory->IsValid())
	{
		return EBTNodeResult::Failed;
	}
	if(HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		TaskMemory->Reset();
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToFaceTargetTaskMemory* TaskMemory = CastInstanceNodeMemory<FRotateToFaceTargetTaskMemory>(NodeMemory);
	if(!TaskMemory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if(HasReachedAnglePrecision(TaskMemory->OwningPawn.Get(), TaskMemory->TargetActor.Get()))
	{
		TaskMemory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(TaskMemory->OwningPawn->GetActorLocation(), TaskMemory->TargetActor->GetActorLocation());
		const FRotator FinalRot = FMath::RInterpTo(TaskMemory->OwningPawn->GetActorRotation(), LookAtRot, DeltaSeconds, RotationInterpSpeed);

		TaskMemory->OwningPawn->SetActorRotation(FinalRot);
	}
}

bool UBTTask_RotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector QueryForwardVector = QueryPawn->GetActorForwardVector();
	const FVector QueryToTargetVector = (TargetActor->GetActorLocation() - QueryPawn->GetActorLocation()).GetSafeNormal();

	const float DotProduct = FVector::DotProduct(QueryForwardVector, QueryToTargetVector);
	const float AngleDiff = UKismetMathLibrary::DegAcos(DotProduct);

	return AngleDiff < AnglePrecision;
	
}
