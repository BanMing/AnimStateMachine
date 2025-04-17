// Copyright BanMing

#include "AnimStateMachineContainer.h"

#include "AnimState.h"
#include "AnimStateMachine.h"
#include "AnimStateMachineLogChannels.h"
#include "Animation/AnimInstance.h"

bool UAnimStateMachineContainer::Initialize(UAnimInstance* AnimInstance)
{
	if (!AnimInstance)
	{
		UE_LOG(LogAnimStateMachine, Error, TEXT("Initializing with null AnimInstance for (%s)"), *GetFullName());
		return false;
	}

	if (bInitialized)
	{
		Teardown();
	}

	CachedAnimInstance = AnimInstance;
	MachineInstances.Empty();
	for (TSubclassOf<UAnimStateMachine> StateMachineClass : AnimStateMachineClasses)
	{
		if (StateMachineClass)
		{
			UAnimStateMachine* NewAnimStateMachine = NewObject<UAnimStateMachine>(this, StateMachineClass);
			if (NewAnimStateMachine->Initialize(AnimInstance))
			{
				MachineInstances.Add(NewAnimStateMachine);
				CacheIndexToMachineMap.Add(NewAnimStateMachine->GetMachineIndex(), NewAnimStateMachine);
				CacheNameToMachineMap.Add(NewAnimStateMachine->GetMachineName(), NewAnimStateMachine);
			}
		}
	}

	return false;
}

void UAnimStateMachineContainer::BeginPlay()
{
}

void UAnimStateMachineContainer::UpdateAnimation(const float DeltaSeconds)
{
}

void UAnimStateMachineContainer::PostEvaluateAnimation()
{
}

void UAnimStateMachineContainer::Teardown()
{
}

UWorld* UAnimStateMachineContainer::GetWorld() const
{
	if (UAnimInstance* AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->GetWorld();
	}
	return nullptr;
}
