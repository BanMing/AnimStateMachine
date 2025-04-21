// Copyright BanMing

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "ShareLifeInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UShareLifeInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ANIMSTATEMACHINE_API IShareLifeInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void BeginPlay() = 0;

	virtual void UpdateAnimation(const float DeltaSeconds) = 0;

	virtual void PostEvaluateAnimation() = 0;

	virtual void Teardown() = 0;

	virtual UWorld* GetWorld() const = 0;

protected:
	virtual void Reset() = 0;

protected:
	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnInitialize();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnBeginPlay();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnBegin();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnUpdateAnimation(const float DeltaSeconds);

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnPostEvaluateAnimation();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnEnd();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State")
	void OnTeardown();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State Machine")
	void OnStateMachineBecomeRelevant();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State Machine")
	void OnStateMachineStartBlendingOut();

	UFUNCTION(BlueprintNativeEvent, Category = "Anim State Machine")
	void OnStateMachineFinishingBlendingOut();
};
