// Copyright BanMing

#pragma once
#include "AnimStateRelevancyStatus.h"
#include "CoreMinimal.h"
#include "Interface/AnimStateMachineInterface.h"
#include "UObject/Object.h"

#include "AnimStateMachine.generated.h"

class UAnimInstance;
struct FBakedAnimationStateMachine;
struct FAnimNode_StateMachine;
class UAnimState;
class APawn;
class ACharacter;

/**
 *
 */
UCLASS(Blueprintable, BlueprintType, HideDropdown)
class ANIMSTATEMACHINE_API UAnimStateMachine : public UObject, public IAnimStateMachineInterface
{
	GENERATED_BODY()

#pragma region Life Cycle
public:
	// Inherited via IAnimStateMachineInterface
	virtual bool Initialize(UAnimInstance* AnimInstance) override;
	virtual void BeginPlay() override;
	virtual void UpdateAnimation(const float DeltaSeconds) override;
	virtual void PostEvaluateAnimation() override;

	virtual void Teardown() override;
	virtual UWorld* GetWorld() const override;

protected:
	virtual void Reset() override;
#pragma endregion

#pragma region Get Common Info
public:
	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	FName GetMachineName() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	int32 GetMachineIndex() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	UAnimInstance* GetCachedAnimInstance() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	AActor* GetOwningActor() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	APawn* GetPawnOwner() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	ACharacter* GetCharacterOwner() const;
#pragma endregion

#pragma region Get Machine State Info
public:
	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	float GetMachineWeight() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	bool HasStateChanged() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	bool IsRelevant() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	bool IsBlendingOut() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	bool IsBlendingIn() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	bool CanBeConsideredActive() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	UAnimState* GetStateInstanceByName(FName StateName) const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	UAnimState* GetStateInstanceByIndex(int32 StateIndex) const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	UAnimState* GetCurrentStateInstance() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	FName GetCurrentStateName() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	int32 GetCurrentStateIndex() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	float GetCurrentStateElapsedTime() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	float GetCurrentStateStateWeight() const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	float GetStateStateWeightByName(FName StateName) const;

	UFUNCTION(BlueprintPure, Category = "AnimStateMachine")
	float GetStateStateWeightByIndex(int32 StateIndex) const;

	FName GetStateNameByStateIndex(const int32 StateIndex) const;
	int32 GetStateIndexByStateName(const FName& StateName) const;

	float NativeGetMachineWeight() const;
	float NativeGetStateWeight(int32 StateIndex) const;
	int32 NativeGetStateIndexByName(const FName& StateName) const;
#pragma endregion

#pragma region Update State Machine Info
protected:
	void UpdateStateWeight();
	void CheckStateMachineBecomeRelevant();
	void CheckStateChanged();
	void CheckStateMachineBlendingOut();
	void CheckStateMachineActive();
#pragma endregion

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimStateMachine")
	FName MachineName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimStateMachine")
	FGameplayTag MachineTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimStateMachine")
	TArray<TSubclassOf<UAnimState>> AnimStateClasses;

protected:
	UPROPERTY()
	TArray<UAnimState*> StateInstances;

	UPROPERTY()
	TMap<int32, UAnimState*> IndexToStateInstanceMap;

	UPROPERTY()
	TMap<FName, UAnimState*> NameToStateInstanceMap;

	UPROPERTY()
	TObjectPtr<UAnimState> PreviousStateInstance = nullptr;

	UPROPERTY()
	TObjectPtr<UAnimState> CurrentStateInstance = nullptr;

	UPROPERTY()
	TWeakObjectPtr<UAnimInstance> CachedAnimInstance;

protected:
	const FBakedAnimationStateMachine* BakedAnimationStateMachine = nullptr;
	const FAnimNode_StateMachine* NodeInstance = nullptr;

	int32 MachineIndex = INDEX_NONE;

	FAnimStateRelevancyStatus RelevancyStatus;

	bool bInitialized = false;

	FString CachedMachineFullNameString;

	TArray<FName> StateNames;
	TMap<int32, float> StateIndexToStateWeightMap;
	TMap<int32, FName> StateIndexToStateNameMap;
	TMap<FName, int32> StateNameToStateIndexMap;

	bool bCanBeConsideredActive = false;

	bool bHasStateChanged = false;

	FName CurrentStateName = NAME_None;
	FName PreviousStateName = NAME_None;
	int32 CurrentStateIndex = INDEX_NONE;

	float CurrentStateElapsedTime = 0.f;
};
