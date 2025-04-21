// Copyright BanMing

#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"
#include "Interface/StateMachineAnimInstanceInterface.h"
#include "UObject/Object.h"

#include "StateMachineAnimInstance.generated.h"

class UAnimStateMachineContainer;
/**
 *
 */
UCLASS()
class ANIMSTATEMACHINE_API UStateMachineAnimInstance : public UAnimInstance, public IStateMachineAnimInstanceInterface
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativePostEvaluateAnimation() override;
	virtual void NativeUninitializeAnimation() override;

protected:
	// Inherited via IStateMachineAnimInstanceInterface
	void OnInitializeAimStateMachineContainer() override;
	void OnBeginPlayAimStateMachineContainer() override;
	void OnUpdateAimStateMachineContainer(float DeltaSeconds) override;
	void OnPostEvaluateAimStateMachineContainer() override;
	void OnTeardownAimStateMachineContainer() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim State Machine")
	TSubclassOf<UAnimStateMachineContainer> AnimStateMachineContainerCalss;

protected:
	UPROPERTY()
	TObjectPtr<UAnimStateMachineContainer> AnimStateMachineContainer;
};
