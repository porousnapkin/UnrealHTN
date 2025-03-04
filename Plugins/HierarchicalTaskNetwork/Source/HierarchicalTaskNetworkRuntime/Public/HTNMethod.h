// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNMethod.generated.h"

/**
 * Forward declaration for the HTNMethod class.
 * This will be fully implemented in a later step, but we need the declaration
 * to reference it in the task interfaces.
 * 
 * An HTN Method represents a specific way to decompose a compound task into subtasks.
 * Compound tasks can have multiple methods representing different approaches to achieve the same goal.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNMethod : public UObject
{
	GENERATED_BODY()

public:
	UHTNMethod() {}
};