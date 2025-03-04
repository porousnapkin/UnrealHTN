// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNEffect.h"

UHTNEffect::UHTNEffect()
{
	// Will be expanded with full implementation later
}

void UHTNEffect::ApplyEffect_Implementation(UHTNWorldState* WorldState) const
{
	// Base implementation - to be overridden by derived classes
	// Does nothing by default
}

FString UHTNEffect::GetDescription_Implementation() const
{
	// Base implementation - to be overridden by derived classes
	return FString::Printf(TEXT("Effect: %s"), *GetClass()->GetName());
}