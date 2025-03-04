// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNCondition.h"

UHTNCondition::UHTNCondition()
{
	// Will be expanded with full implementation later
}

bool UHTNCondition::CheckCondition_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState) const
{
	// Base implementation - to be overridden by derived classes
	return true;
}

FString UHTNCondition::GetDescription_Implementation() const
{
	// Base implementation - to be overridden by derived classes
	return FString::Printf(TEXT("Condition: %s"), *GetClass()->GetName());
}