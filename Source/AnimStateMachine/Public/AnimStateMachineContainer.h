// Copyright BanMing

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"

#include "AnimStateMachineContainer.generated.h"

class UAnimInstance;
class UAnimStateMachine;
class UAnimState;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType)
class ANIMSTATEMACHINE_API UAnimStateMachineContainer : public UObject
{
	GENERATED_BODY()
public:
	bool Initialize(UAnimInstance* AnimInstance);
	void BeginPlay();
	void UpdateAnimation(const float DeltaSeconds);
	void PostEvaluateAnimation();
	void Teardown();
	virtual UWorld* GetWorld() const override;

public:
	UAnimStateMachine* GetMachineInstanceByIndex(int32 MachineIndex) const;
	UAnimStateMachine* GetMachineInstanceByName(const FName& MachineName) const;
	UAnimState* GetStateInstanceByName(const FName& MachineName, const FName& StateName) const;
	UAnimState* GetStateInstanceByIndex(int32 MachineIndex, int32 StateIndex) const;
	FName GetMachineCurrentStateName(const FName& MachineName) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSubclassOf<UAnimStateMachine>> AnimStateMachineClasses;

protected:
	bool bInitialized = false;

	UPROPERTY()
	TWeakObjectPtr<UAnimInstance> CachedAnimInstance;

	UPROPERTY()
	TArray<TObjectPtr<UAnimStateMachine>> MachineInstances;

	UPROPERTY()
	TMap<int32, TObjectPtr<UAnimStateMachine>> CacheIndexToMachineMap;

	UPROPERTY()
	TMap<FName, TObjectPtr<UAnimStateMachine>> CacheNameToMachineMap;
};
