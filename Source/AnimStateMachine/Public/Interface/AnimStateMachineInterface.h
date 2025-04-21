// Copyright BanMing

#pragma once

#include "CoreMinimal.h"
#include "Interface/ShareLifeInterface.h"
#include "UObject/Interface.h"

#include "AnimStateMachineInterface.generated.h"
class UAnimState;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimStateMachineInterface : public UShareLifeInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ANIMSTATEMACHINE_API IAnimStateMachineInterface : public IShareLifeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool Initialize(UAnimInstance* AnimInstance) = 0;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "AnimStateMachine")
	void OnStateMachineStateChanged(UAnimState* PreviousAnimState, UAnimState* CurrentAnimState);

	UFUNCTION(BlueprintNativeEvent, Category = "AnimStateMachine")
	void OnStateMachineBecomeActive();

	UFUNCTION(BlueprintNativeEvent, Category = "AnimStateMachine")
	void OnStateMachineBecomeInactive();

	UFUNCTION(BlueprintNativeEvent, Category = "AnimStateMachine")
	void OnStateMachinePostEvaluateAnimation();

	UFUNCTION(BlueprintNativeEvent, Category = "AnimStateMachine")
	void OnStateMachineTeardown();
};
