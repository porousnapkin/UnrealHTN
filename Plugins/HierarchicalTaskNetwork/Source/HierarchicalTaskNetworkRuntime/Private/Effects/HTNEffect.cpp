// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/HTNEffect.h"

#include "HTNLogging.h"

UHTNEffect::UHTNEffect()
	: DebugColor(FLinearColor::Green)
{
}

void UHTNEffect::ApplyEffect_Implementation(UHTNWorldState* WorldState) const
{
	// Base implementation does nothing
	// Derived classes should override this to implement specific effect logic
}

FString UHTNEffect::GetDescription_Implementation() const
{
	// Default description uses the class name
	return FString::Printf(TEXT("Effect: %s"), *GetClass()->GetName());
}

bool UHTNEffect::ValidateEffect_Implementation() const
{
	// Base validation just checks if the object is valid
	return IsValid(this);
}