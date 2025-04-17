// Copyright BanMing

#include "AnimState.h"

#include "AnimStateMachine.h"
#include "AnimStateMachineLogChannels.h"

#if ENABLE_ANIM_DEBUG
TAutoConsoleVariable<int32> CVarAnimStateDebug(TEXT("a.AnimState.Debug"), 0, TEXT("Turn on visualization debugging for Animation State"));
#endif

#pragma region Life Cycle
bool UAnimState::Initialize(UAnimStateMachine* MachineOwner)
{
	if (!MachineOwner)
	{
		UE_LOG(LogAnimState, Error, TEXT("Initializing with null StateMachineOwner!  (%s)"), *this->GetName());
		return false;
	}

	if (bInitialized)
	{
		Teardown();
	}

	Reset();

	StateIndex = MachineOwner->NativeGetStateIndexByName(StateName);
	if (StateIndex == INDEX_NONE)
	{
		UE_LOG(LogAnimState, Error, TEXT("Can not find StateIndex by StateName (%s) in StateMachine (%s)"), *StateName.ToString(), *MachineOwner->GetMachineName().ToString());
		return false;
	}

	CachedMachineOwner = MachineOwner;

	CachedStateFullNameString = FString::Printf(TEXT("(%s : %s) in (%s)"), *MachineOwner->GetMachineName().ToString(), *StateName.ToString(), *GetNameSafe(GetCachedAnimInstance()));

#if ENABLE_ANIM_DEBUG
	bool bDebugging = CVarAnimStateDebug.GetValueOnAnyThread() == 1;
	if (bDebugging)
	{
		UE_LOG(LogAnimState, Error, TEXT("OnInitialize: (%s)"), *CachedStateFullNameString);
	}
#endif

	OnInitialize();

	bInitialized = true;

	return true;
}

void UAnimState::BeginPlay()
{
	OnBeginPlay();
}

void UAnimState::Begin()
{
#if ENABLE_ANIM_DEBUG
	bool bDebugging = CVarAnimStateDebug.GetValueOnAnyThread() == 1;
	if (bDebugging)
	{
		UE_LOG(LogAnimState, Error, TEXT("OnBegin: (%s)"), *CachedStateFullNameString);
	}
#endif
	OnBegin();
}

void UAnimState::UpdateAnimation(const float DeltaSeconds)
{
	if (IsActive() || bForceUpdateAnimation)
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimState, Error, TEXT("OnUpdateAnimation: (%s)"), *CachedStateFullNameString);
		}
#endif
		OnUpdateAnimation(DeltaSeconds);
	}
}

void UAnimState::PostEvaluateAnimation()
{
	if (IsActive() || bForceUpdateAnimation)
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimState, Error, TEXT("OnPostEvaluateAnimation: (%s)"), *CachedStateFullNameString);
		}
#endif
		OnPostEvaluateAnimation();
	}
}

void UAnimState::StateMachineBecomeRelevant()
{
	OnStateMachineBecomeRelevant();
}

void UAnimState::StateMachineStartBlendingOut()
{
	OnStateMachineFinishingBlendingOut();
}

void UAnimState::StateMachineFinishingBlendingOut()
{
	OnStateMachineFinishingBlendingOut();
}

void UAnimState::Teardown()
{
	if (!IsUnreachable())
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimState, Error, TEXT("OnTeardown: (%s)"), *CachedStateFullNameString);
		}
#endif
		OnTeardown();
	}
	bInitialized = false;
	Reset();
}

void UAnimState::Reset()
{
	CachedMachineOwner.Reset();
	StateIndex = INDEX_NONE;
	CachedStateFullNameString.Reset();
}

void UAnimState::End()
{
#if ENABLE_ANIM_DEBUG
	bool bDebugging = CVarAnimStateDebug.GetValueOnAnyThread() == 1;
	if (bDebugging)
	{
		UE_LOG(LogAnimState, Error, TEXT("OnEnd: (%s)"), *CachedStateFullNameString);
	}
#endif
	OnEnd();
}
UWorld* UAnimState::GetWorld() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetWorld();
	}
	return nullptr;
}
#pragma endregion

#pragma region Get Common Info
int32 UAnimState::GetStateIndex() const
{
	return StateIndex;
}

FName UAnimState::GetStateName() const
{
	return StateName;
}

UAnimInstance* UAnimState::GetCachedAnimInstance() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetCachedAnimInstance();
	}

	return nullptr;
}

AActor* UAnimState::GetOwningActor() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetOwningActor();
	}
	return nullptr;
}

APawn* UAnimState::GetPawnOwner() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetPawnOwner();
	}
	return nullptr;
}

ACharacter* UAnimState::GetCharacterOwner() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetCharacterOwner();
	}
	return nullptr;
}

UAnimState* UAnimState::GetSilingStateInstance(FName InStateName) const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetStateInstanceByName(InStateName);
	}
	return nullptr;
}

bool UAnimState::IsActive() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetCurrentStateName() == StateName;
	}
	return false;
}

bool UAnimState::IsRelevant() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->IsRelevant() && AnimStateMachine->GetStateStateWeightByIndex(StateIndex) > 0.f;
	}
	return false;
}

bool UAnimState::IsCurrentAndRelevant() const
{
	return IsActive() && IsRelevant();
}

bool UAnimState::IsMachineOwnerBlendingOut() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->IsBlendingOut();
	}
	return false;
}

float UAnimState::GetElapsedTime() const
{
	if (IsActive())
	{
		if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
		{
			return AnimStateMachine->GetCurrentStateElapsedTime();
		}
	}
	return 0.0f;
}

float UAnimState::GetWeight() const
{
	if (UAnimStateMachine* AnimStateMachine = CachedMachineOwner.Get())
	{
		return AnimStateMachine->GetStateStateWeightByIndex(StateIndex);
	}
	return 0.0f;
}

FGameplayTag UAnimState::GetStateTag() const
{
	return StateTag;
}

#pragma endregion
