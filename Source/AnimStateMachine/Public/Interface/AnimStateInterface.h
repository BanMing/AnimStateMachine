// Copyright BanMing

#pragma once

#include "CoreMinimal.h"
#include "Interface/ShareLifeInterface.h"
#include "UObject/Interface.h"

#include "AnimStateInterface.generated.h"

class UAnimStateMachine;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAnimStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ANIMSTATEMACHINE_API IAnimStateInterface : public IShareLifeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual bool Initialize(UAnimStateMachine* MachineOwner) = 0;

	virtual void End() = 0;

	virtual void StateMachineBecomeRelevant() = 0;

	virtual void StateMachineStartBlendingOut() = 0;

	virtual void StateMachineFinishingBlendingOut() = 0;
};
