// Copyright BanMing

#include "AnimStateRelevancyStatus.h"

FAnimStateRelevancyStatus::FAnimStateRelevancyStatus()
{
	Reset();
}

float FAnimStateRelevancyStatus::GetCurrentWeight() const
{
	return CurrentWeight;
}

float FAnimStateRelevancyStatus::GetPreviousWeight() const
{
	return PreviousWeight;
}

bool FAnimStateRelevancyStatus::HasJustBecomeRelevant() const
{
	return IsStartingBlendingIn();
}

bool FAnimStateRelevancyStatus::IsStartingBlendingIn() const
{
	return FAnimWeight::IsFullWeight(PreviousWeight) && !FAnimWeight::IsFullWeight(CurrentWeight);
}

bool FAnimStateRelevancyStatus::IsFinishingBlendingIn() const
{
	return !FAnimWeight::IsFullWeight(PreviousWeight) && FAnimWeight::IsFullWeight(CurrentWeight);
}

bool FAnimStateRelevancyStatus::HasFullyBlendedIn() const
{
	return FAnimWeight::IsFullWeight(PreviousWeight) && FAnimWeight::IsFullWeight(CurrentWeight);
}

bool FAnimStateRelevancyStatus::IsStartingBlendingOut() const
{
	return FAnimWeight::IsFullWeight(PreviousWeight) && !FAnimWeight::IsFullWeight(CurrentWeight);
}

bool FAnimStateRelevancyStatus::IsFinishingBlendingOut() const
{
	return FAnimWeight::IsRelevant(PreviousWeight) && !FAnimWeight::IsRelevant(CurrentWeight);
}

bool FAnimStateRelevancyStatus::HasFullyBlendedOut() const
{
	return !FAnimWeight::IsRelevant(PreviousWeight) && !FAnimWeight::IsRelevant(CurrentWeight);
}

bool FAnimStateRelevancyStatus::IsRelevant() const
{
	return FAnimWeight::IsRelevant(CurrentWeight);
}

bool FAnimStateRelevancyStatus::IsBlendingOut() const
{
	return CurrentWeight < PreviousWeight;
}

bool FAnimStateRelevancyStatus::IsBlendingIn() const
{
	return PreviousWeight < CurrentWeight;
}

bool FAnimStateRelevancyStatus::IsNotBlending() const
{
	return CurrentWeight == PreviousWeight;
}

bool FAnimStateRelevancyStatus::CanBeConsideredActive() const
{
	return IsRelevant() && !IsBlendingOut();
}

void FAnimStateRelevancyStatus::UpdateWeight(const float InCurrentWeight)
{
	PreviousWeight = CurrentWeight;
	CurrentWeight = InCurrentWeight;
}

void FAnimStateRelevancyStatus::Reset()
{
	PreviousWeight = 0.f;
	CurrentWeight = 0.f;
}
