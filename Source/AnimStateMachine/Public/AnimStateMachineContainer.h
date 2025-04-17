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
UCLASS(Abstract, Blueprintable, BlueprintType)
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
