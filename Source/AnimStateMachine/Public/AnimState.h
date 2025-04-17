// Copyright BanMing

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/AnimStateInterface.h"
#include "UObject/Object.h"

#include "AnimState.generated.h"

class UAnimStateMachine;
/**
 *
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class ANIMSTATEMACHINE_API UAnimState : public UObject, public IAnimStateInterface
{
	GENERATED_BODY()
#pragma region Life Cycle
public:
	// Inherited via IAnimStateInterface
	virtual bool Initialize(UAnimStateMachine* MachineOwner) override;
	virtual void BeginPlay() override;
	void Begin();
	virtual void UpdateAnimation(const float DeltaSeconds) override;
	virtual void PostEvaluateAnimation() override;
	virtual void Teardown() override;
	virtual void End() override;
	virtual void StateMachineBecomeRelevant() override;
	virtual void StateMachineStartBlendingOut() override;
	virtual void StateMachineFinishingBlendingOut() override;
	virtual UWorld* GetWorld() const override;

protected:
	virtual void Reset() override;
#pragma endregion

#pragma region Get Common Info
public:
	UFUNCTION(BlueprintPure, Category = "AnimState")
	int32 GetStateIndex() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	FName GetStateName() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	UAnimInstance* GetCachedAnimInstance() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	AActor* GetOwningActor() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	APawn* GetPawnOwner() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	ACharacter* GetCharacterOwner() const;
#pragma endregion
public:
	UFUNCTION(BlueprintPure, Category = "AnimState")
	UAnimState* GetSilingStateInstance(FName InStateName) const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	bool IsActive() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	bool IsRelevant() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	bool IsCurrentAndRelevant() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	bool IsMachineOwnerBlendingOut() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	float GetElapsedTime() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	float GetWeight() const;

	UFUNCTION(BlueprintPure, Category = "AnimState")
	FGameplayTag GetStateTag() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimState")
	bool bForceUpdateAnimation = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimState")
	FName StateName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimState")
	FGameplayTag StateTag;

protected:
	TWeakObjectPtr<UAnimStateMachine> CachedMachineOwner;

	int32 StateIndex = INDEX_NONE;

	FString CachedStateFullNameString;

	bool bInitialized = false;
};
