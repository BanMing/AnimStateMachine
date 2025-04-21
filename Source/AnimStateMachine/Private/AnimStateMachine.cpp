// Copyright BanMing

#include "AnimStateMachine.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AnimState.h"
#include "AnimStateMachineLogChannels.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Animation/AnimStateMachineTypes.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"

#if ENABLE_ANIM_DEBUG
TAutoConsoleVariable<int32> CVarAnimStateMachineDebug(TEXT("a.AnimStateMachine.Debug"), 0, TEXT("Turn on visualization debugging for Animation State Machine"));
#endif

#pragma region Life Cycle

bool UAnimStateMachine::Initialize(UAnimInstance* AnimInstance)
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

	Reset();

	CachedAnimInstance = AnimInstance;

	CachedMachineFullNameString = FString::Printf(TEXT("(%s) in (%s)"), *MachineName.ToString(), *GetNameSafe(CachedAnimInstance.Get()));

	// Cache FBakedAnimationStateMachine
	AnimInstance->GetStateMachineIndexAndDescription(MachineName, MachineIndex, &BakedAnimationStateMachine);
	if (MachineIndex == INDEX_NONE)
	{
		UE_LOG(LogAnimStateMachine, Error, TEXT("Failed to find StateMachineIndex. (%s)"), *CachedMachineFullNameString);
		return false;
	}

	// Cache FAnimNode_StateMachine
	NodeInstance = AnimInstance->GetStateMachineInstance(MachineIndex);
	if (!NodeInstance)
	{
		UE_LOG(LogAnimStateMachine, Error, TEXT("Failed to find corresponding FAnimNode_StateMachine. (%s)"), *CachedMachineFullNameString);
		return false;
	}

	// Cache State Name
	for (int32 i = 0; i < BakedAnimationStateMachine->States.Num(); ++i)
	{
		const FName& StateName = BakedAnimationStateMachine->States[i].StateName;
		StateNames.Add(StateName);
		StateIndexToStateNameMap.Add(i, StateName);
		StateNameToStateIndexMap.Add(StateName, i);
	}
#if ENABLE_ANIM_DEBUG
	bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
	if (bDebugging)
	{
		UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineInitialize. (%s)"), *CachedMachineFullNameString);
	}
#endif
	OnInitialize();

	// Create AnimState Object instance
	for (TSubclassOf<UAnimState>& StateClass : AnimStateClasses)
	{
		if (StateClass)
		{
			UAnimState* NewStateInstance = NewObject<UAnimState>(this, StateClass);
			if (NewStateInstance->Initialize(this))
			{
				StateInstances.Add(NewStateInstance);
				IndexToStateInstanceMap.Add(NewStateInstance->GetStateIndex(), NewStateInstance);
				NameToStateInstanceMap.Add(NewStateInstance->GetStateName(), NewStateInstance);
			}
		}
	}

	bInitialized = true;
	return true;
}

void UAnimStateMachine::BeginPlay()
{
	OnBeginPlay();
	for (UAnimState* StateInstance : StateInstances)
	{
		StateInstance->BeginPlay();
	}
}

void UAnimStateMachine::UpdateAnimation(const float DeltaSeconds)
{
	if (IsRelevant() || bForceUpdateAnimation)
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimStateMachine, Log, TEXT("OnUpdateAnimation. (%s)"), *CachedMachineFullNameString);
		}
#endif

		OnUpdateAnimation(DeltaSeconds);

		for (UAnimState* StateInstance : StateInstances)
		{
			StateInstance->UpdateAnimation(DeltaSeconds);
		}
	}
}

void UAnimStateMachine::PostEvaluateAnimation()
{
	PreviousStateName = CurrentStateName;
	PreviousStateInstance = CurrentStateInstance;

	CurrentStateName = NodeInstance->GetCurrentStateName();
	CurrentStateIndex = NodeInstance->GetCurrentState();
	CurrentStateElapsedTime = NodeInstance->GetCurrentStateElapsedTime();
	CurrentStateInstance = GetStateInstanceByName(CurrentStateName);

	RelevancyStatus.UpdateWeight(NativeGetMachineWeight());

#if ENABLE_ANIM_DEBUG
	bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
	if (bDebugging)
	{
		UE_LOG(LogAnimStateMachine, Log, TEXT("PostEvaluateAnimation of (%s). RelevancyStatus: CurrentWeight(%f), PreviousWeight(%f)."), *CachedMachineFullNameString, RelevancyStatus.GetCurrentWeight(),
			RelevancyStatus.GetPreviousWeight());
	}
#endif

	// Update State Weight
	UpdateStateWeight();

	// Check State Machine Become Relevant
	CheckStateMachineBecomeRelevant();

	// Check State Machine Change State
	CheckStateChanged();

	// Check State Machine Start Blending Out
	CheckStateMachineBlendingOut();

	CheckStateMachineActive();

	OnStateMachinePostEvaluateAnimation();

	for (UAnimState* StateInstance : StateInstances)
	{
		StateInstance->PostEvaluateAnimation();
	}
}

void UAnimStateMachine::Reset()
{
	CachedAnimInstance.Reset();
	MachineIndex = INDEX_NONE;
	BakedAnimationStateMachine = nullptr;
	NodeInstance = nullptr;

	StateInstances.Empty();
	IndexToStateInstanceMap.Empty();
	NameToStateInstanceMap.Empty();

	PreviousStateName = NAME_None;
	PreviousStateInstance = nullptr;

	CurrentStateIndex = INDEX_NONE;
	CurrentStateInstance = nullptr;
	CurrentStateName = NAME_None;
	CurrentStateElapsedTime = 0.f;

	RelevancyStatus.Reset();
	bCanBeConsideredActive = false;
	bHasStateChanged = false;

	StateIndexToStateWeightMap.Empty();
	StateIndexToStateNameMap.Empty();
	StateNameToStateIndexMap.Empty();

	CachedMachineFullNameString.Reset();
}

void UAnimStateMachine::Teardown()
{
	for (UAnimState* StateInstance : StateInstances)
	{
		StateInstance->Teardown();
	}

	if (!IsUnreachable())
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineTeardown. (%s)"), *CachedMachineFullNameString);
		}
#endif
		OnStateMachineTeardown();
	}

	bInitialized = false;
	Reset();
}

UWorld* UAnimStateMachine::GetWorld() const
{
	if (const auto AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->GetWorld();
	}
	return nullptr;
}
#pragma endregion

#pragma region Get Common Info
FName UAnimStateMachine::GetMachineName() const
{
	return MachineName;
}

int32 UAnimStateMachine::GetMachineIndex() const
{
	return MachineIndex;
}

UAnimInstance* UAnimStateMachine::GetCachedAnimInstance() const
{
	return CachedAnimInstance.Get();
}

AActor* UAnimStateMachine::GetOwningActor() const
{
	if (const UAnimInstance* AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->GetOwningActor();
	}
	return nullptr;
}

APawn* UAnimStateMachine::GetPawnOwner() const
{
	if (const UAnimInstance* AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->TryGetPawnOwner();
	}
	return nullptr;
}

ACharacter* UAnimStateMachine::GetCharacterOwner() const
{
	if (const UAnimInstance* AnimInstance = CachedAnimInstance.Get())
	{
		return Cast<ACharacter>(AnimInstance->TryGetPawnOwner());
	}
	return nullptr;
}
#pragma endregion

#pragma region Get Machine State Info

float UAnimStateMachine::GetMachineWeight() const
{
	return RelevancyStatus.GetCurrentWeight();
}

bool UAnimStateMachine::HasStateChanged() const
{
	return bHasStateChanged;
}

bool UAnimStateMachine::IsRelevant() const
{
	return RelevancyStatus.IsRelevant();
}

bool UAnimStateMachine::IsBlendingOut() const
{
	return RelevancyStatus.IsBlendingOut();
}

bool UAnimStateMachine::IsBlendingIn() const
{
	return RelevancyStatus.IsBlendingIn();
}

bool UAnimStateMachine::CanBeConsideredActive() const
{
	return RelevancyStatus.CanBeConsideredActive();
}

UAnimState* UAnimStateMachine::GetStateInstanceByName(FName StateName) const
{
	return NameToStateInstanceMap.FindRef(StateName);
}

UAnimState* UAnimStateMachine::GetStateInstanceByIndex(int32 StateIndex) const
{
	return IndexToStateInstanceMap.FindRef(StateIndex);
}

UAnimState* UAnimStateMachine::GetCurrentStateInstance() const
{
	return CurrentStateInstance;
}

FName UAnimStateMachine::GetCurrentStateName() const
{
	return CurrentStateName;
}

int32 UAnimStateMachine::GetCurrentStateIndex() const
{
	return CurrentStateIndex;
}

float UAnimStateMachine::GetCurrentStateElapsedTime() const
{
	return CurrentStateElapsedTime;
}

float UAnimStateMachine::GetCurrentStateStateWeight() const
{
	return StateIndexToStateWeightMap.FindRef(CurrentStateIndex);
}

float UAnimStateMachine::GetStateStateWeightByName(FName StateName) const
{
	const int32 StateIndex = GetStateIndexByStateName(StateName);
	if (StateIndex != INDEX_NONE)
	{
		return StateIndexToStateWeightMap.FindRef(StateIndex);
	}
	return 0.0f;
}

float UAnimStateMachine::GetStateStateWeightByIndex(int32 StateIndex) const
{
	return StateIndexToStateWeightMap.FindRef(StateIndex);
}

FName UAnimStateMachine::GetStateNameByStateIndex(const int32 StateIndex) const
{
	if (const auto FoundPtr = StateIndexToStateNameMap.Find(StateIndex))
	{
		return *FoundPtr;
	}
	return NAME_None;
}

int32 UAnimStateMachine::GetStateIndexByStateName(const FName& StateName) const
{
	if (const auto FoundPtr = StateNameToStateIndexMap.Find(StateName))
	{
		return *FoundPtr;
	}
	return INDEX_NONE;
}

float UAnimStateMachine::NativeGetMachineWeight() const
{
	if (const auto AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->GetInstanceMachineWeight(MachineIndex);
	}
	return 0.f;
}

float UAnimStateMachine::NativeGetStateWeight(int32 StateIndex) const
{
	if (StateIndex == INDEX_NONE)
	{
		return 0.f;
	}

	if (const auto AnimInstance = CachedAnimInstance.Get())
	{
		return AnimInstance->GetInstanceStateWeight(MachineIndex, StateIndex);
	}
	return 0.f;
}

int32 UAnimStateMachine::NativeGetStateIndexByName(const FName& StateName) const
{
	if (BakedAnimationStateMachine)
	{
		return BakedAnimationStateMachine->FindStateIndex(StateName);
	}
	return INDEX_NONE;
}
#pragma endregion

#pragma region Update State Machine Info
void UAnimStateMachine::UpdateStateWeight()
{
	for (const auto StateName : StateNames)
	{
		int32 StateIndex = GetStateIndexByStateName(StateName);
		float StateWeight = NativeGetStateWeight(StateIndex);
		StateIndexToStateWeightMap.Add(StateIndex, StateWeight);
	}
}

void UAnimStateMachine::CheckStateMachineBecomeRelevant()
{
	if (RelevancyStatus.IsStartingBlendingIn())
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineBecomeRelevant. (%s)"), *CachedMachineFullNameString);
		}
#endif
		OnStateMachineBecomeRelevant();
		for (const auto StateInstance : StateInstances)
		{
			StateInstance->StateMachineBecomeRelevant();
		}
	}
}

void UAnimStateMachine::CheckStateMachineBlendingOut()
{
	if (RelevancyStatus.IsStartingBlendingOut())
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineBlendingOut. (%s)"), *CachedMachineFullNameString);
		}
#endif
		OnStateMachineStartBlendingOut();
		for (const auto StateInstance : StateInstances)
		{
			StateInstance->StateMachineStartBlendingOut();
		}
	}

	// Check State Machine Finishing Blending Out
	if (RelevancyStatus.IsFinishingBlendingOut())
	{
#if ENABLE_ANIM_DEBUG
		bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
		if (bDebugging)
		{
			UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineFinishingBlendingOut. (%s)"), *CachedMachineFullNameString);
		}
#endif
		OnStateMachineFinishingBlendingOut();
		for (const auto StateInstance : StateInstances)
		{
			StateInstance->StateMachineFinishingBlendingOut();
		}
	}
}

void UAnimStateMachine::CheckStateChanged()
{
	if (PreviousStateName != CurrentStateName)
	{
		bHasStateChanged = true;
		if (PreviousStateInstance)
		{
			PreviousStateInstance->End();
		}
		if (CurrentStateInstance)
		{
			CurrentStateInstance->Begin();
		}
		OnStateMachineStateChanged(PreviousStateInstance, CurrentStateInstance);
	}
	else
	{
		bHasStateChanged = false;
	}
}

void UAnimStateMachine::CheckStateMachineActive()
{
	const bool bPreviousCanBeConsideredActive = bCanBeConsideredActive;
	bCanBeConsideredActive = RelevancyStatus.CanBeConsideredActive();
	if (bPreviousCanBeConsideredActive != bCanBeConsideredActive)
	{
		if (bCanBeConsideredActive)
		{
#if ENABLE_ANIM_DEBUG
			bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
			if (bDebugging)
			{
				UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineBecomeActive. (%s)"), *CachedMachineFullNameString);
			}
#endif
			OnStateMachineBecomeActive();
			if (MachineTag.IsValid())
			{
				UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(GetOwningActor(), MachineTag.GetSingleTagContainer());
			}
		}
		else
		{
#if ENABLE_ANIM_DEBUG
			bool bDebugging = CVarAnimStateMachineDebug.GetValueOnAnyThread() == 1;
			if (bDebugging)
			{
				UE_LOG(LogAnimStateMachine, Log, TEXT("OnStateMachineBecomeInactive. (%s)"), *CachedMachineFullNameString);
			}
#endif
			OnStateMachineBecomeInactive();
			if (MachineTag.IsValid())
			{
				UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetOwningActor(), MachineTag.GetSingleTagContainer());
			}
		}
	}
}
#pragma endregion
