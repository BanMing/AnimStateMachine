// Copyright BanMing

#include "StateMachineAnimInstance.h"

#include "AnimStateMachineContainer.h"
void UStateMachineAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OnInitializeAimStateMachineContainer();
}

void UStateMachineAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	OnBeginPlayAimStateMachineContainer();
}

void UStateMachineAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	OnUpdateAimStateMachineContainer(DeltaSeconds);
}

void UStateMachineAnimInstance::NativePostEvaluateAnimation()
{
	Super::NativePostEvaluateAnimation();
	OnPostEvaluateAimStateMachineContainer();
}

void UStateMachineAnimInstance::NativeUninitializeAnimation()
{
	Super::NativeUninitializeAnimation();
	OnTeardownAimStateMachineContainer();
}

void UStateMachineAnimInstance::OnInitializeAimStateMachineContainer()
{
	if (!TryGetPawnOwner() || !GetWorld() || !GetWorld()->IsGameWorld())
	{
		return;
	}

	if (AnimStateMachineContainer)
	{
		AnimStateMachineContainer->Teardown();
		AnimStateMachineContainer = nullptr;
	}

	if (AnimStateMachineContainerCalss)
	{
		if (AnimStateMachineContainer = NewObject<UAnimStateMachineContainer>(this, AnimStateMachineContainerCalss))
		{
			if (!AnimStateMachineContainer->Initialize(this))
			{
				AnimStateMachineContainer = nullptr;
			}
		}
	}
}

void UStateMachineAnimInstance::OnBeginPlayAimStateMachineContainer()
{
	if (AnimStateMachineContainer)
	{
		AnimStateMachineContainer->BeginPlay();
	}
}

void UStateMachineAnimInstance::OnUpdateAimStateMachineContainer(float DeltaSeconds)
{
	if (AnimStateMachineContainer)
	{
		AnimStateMachineContainer->UpdateAnimation(DeltaSeconds);
	}
}

void UStateMachineAnimInstance::OnPostEvaluateAimStateMachineContainer()
{
	if (AnimStateMachineContainer)
	{
		AnimStateMachineContainer->PostEvaluateAnimation();
	}
}

void UStateMachineAnimInstance::OnTeardownAimStateMachineContainer()
{
	if (AnimStateMachineContainer)
	{
		AnimStateMachineContainer->Teardown();
	}
}
