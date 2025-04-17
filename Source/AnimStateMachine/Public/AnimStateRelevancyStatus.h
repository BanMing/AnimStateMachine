// Copyright BanMing

#pragma once

#include "CoreMinimal.h"

#include "AnimStateRelevancyStatus.generated.h"

/**
 *
 */
USTRUCT()
struct FAnimStateRelevancyStatus
{
	GENERATED_BODY()

	FAnimStateRelevancyStatus();

	// Get the current weight
	float GetCurrentWeight() const;

	// Get the previous weight
	float GetPreviousWeight() const;

	// Node has just become relevant
	bool HasJustBecomeRelevant() const;

	// Node has weight and had zero weight last update
	bool IsStartingBlendingIn() const;

	// Node has full weight and had non-full weight last update
	bool IsFinishingBlendingIn() const;

	// Node has full weight and had full weight last update
	bool HasFullyBlendedIn() const;

	// Node has non-full weight and had full weight last update
	bool IsStartingBlendingOut() const;

	// Node has zero weight and had non-zero weight last update
	bool IsFinishingBlendingOut() const;

	// Node has zero weight
	bool HasFullyBlendedOut() const;

	bool IsRelevant() const;

	bool IsBlendingOut() const;

	bool IsBlendingIn() const;

	bool IsNotBlending() const;

	bool CanBeConsideredActive() const;

	// Update by current weight
	void UpdateWeight(const float InCurrentWeight);

	void Reset();

private:
	// Previous weight
	float PreviousWeight = 0.0f;

	// Current weight
	float CurrentWeight = 0.0f;
};
