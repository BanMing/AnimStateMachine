// Copyright BanMing

#pragma once

#include "CoreMinimal.h"
#include "Interface/ShareLifeInterface.h"
#include "UObject/Interface.h"

#include "StateMachineAnimInstanceInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStateMachineAnimInstanceInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ANIMSTATEMACHINE_API IStateMachineAnimInstanceInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void NativeInitializeAnimation() = 0;
	virtual void NativeBeginPlay() = 0;
	virtual void NativeUpdateAnimation(float DeltaSeconds) = 0;
	virtual void NativePostEvaluateAnimation() = 0;
	virtual void NativeUninitializeAnimation() = 0;

protected:
	virtual void OnInitializeAimStateMachineContainer() = 0;
	virtual void OnBeginPlayAimStateMachineContainer() = 0;
	virtual void OnUpdateAimStateMachineContainer(float DeltaSeconds) = 0;
	virtual void OnPostEvaluateAimStateMachineContainer() = 0;
	virtual void OnTeardownAimStateMachineContainer() = 0;
};