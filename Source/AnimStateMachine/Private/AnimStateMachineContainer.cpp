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

	bInitialized = true;
	return true;
}

void UAnimStateMachineContainer::BeginPlay()
{
	for (UAnimStateMachine* AnimStateMachine : MachineInstances)
	{
		if (AnimStateMachine)
		{
			AnimStateMachine->BeginPlay();
		}
	}
}

void UAnimStateMachineContainer::UpdateAnimation(const float DeltaSeconds)
{
	for (UAnimStateMachine* AnimStateMachine : MachineInstances)
	{
		if (AnimStateMachine)
		{
			AnimStateMachine->UpdateAnimation(DeltaSeconds);
		}
	}
}

void UAnimStateMachineContainer::PostEvaluateAnimation()
{
	for (UAnimStateMachine* AnimStateMachine : MachineInstances)
	{
		if (AnimStateMachine)
		{
			AnimStateMachine->PostEvaluateAnimation();
		}
	}
}

void UAnimStateMachineContainer::Teardown()
{
	for (UAnimStateMachine* AnimStateMachine : MachineInstances)
	{
		if (AnimStateMachine)
		{
			AnimStateMachine->Teardown();
		}
	}

	MachineInstances.Reset();
	CacheIndexToMachineMap.Empty();
	CacheNameToMachineMap.Empty();
	bInitialized = false;
}

UWorld* UAnimStateMachineContainer::GetWorld() const
{
	if (UAnimInstance* AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->GetWorld();
	}
	return nullptr;
}

UAnimStateMachine* UAnimStateMachineContainer::GetMachineInstanceByIndex(int32 MachineIndex) const
{
	return CacheIndexToMachineMap.FindRef(MachineIndex);
}

UAnimStateMachine* UAnimStateMachineContainer::GetMachineInstanceByName(const FName& MachineName) const
{
	return CacheNameToMachineMap.FindRef(MachineName);
}

UAnimState* UAnimStateMachineContainer::GetStateInstanceByName(const FName& MachineName, const FName& StateName) const
{
	if (const UAnimStateMachine* StateMachine = GetMachineInstanceByName(MachineName))
	{
		return StateMachine->GetStateInstanceByName(StateName);
	}

	return nullptr;
}

UAnimState* UAnimStateMachineContainer::GetStateInstanceByIndex(int32 MachineIndex, int32 StateIndex) const
{
	if (const UAnimStateMachine* StateMachine = GetMachineInstanceByIndex(MachineIndex))
	{
		return StateMachine->GetStateInstanceByIndex(StateIndex);
	}
	return nullptr;
}

FName UAnimStateMachineContainer::GetMachineCurrentStateName(const FName& MachineName) const
{
	if (const UAnimStateMachine* StateMachine = GetMachineInstanceByName(MachineName))
	{
		return StateMachine->GetCurrentStateName();
	}
	return NAME_None;
}
