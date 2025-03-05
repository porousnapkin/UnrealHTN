// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNCondition.h"
#include "HTNLogging.h"

UHTNCondition::UHTNCondition()
	: DebugColor(FLinearColor::Yellow)
{
}

bool UHTNCondition::CheckCondition_Implementation(const UHTNWorldState* WorldState) const
{
	// Base implementation always returns true
	// Derived classes should override this to implement specific condition logic
	return true;
}

FString UHTNCondition::GetDescription_Implementation() const
{
	// Default description uses the class name
	return FString::Printf(TEXT("Condition: %s"), *GetClass()->GetName());
}

bool UHTNCondition::ValidateCondition_Implementation() const
{
	// Base validation just checks if the object is valid
	return IsValid(this);
}