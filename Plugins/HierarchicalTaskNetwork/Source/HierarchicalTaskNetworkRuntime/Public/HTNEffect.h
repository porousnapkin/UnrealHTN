// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNEffect.generated.h"

/**
 * Base class for HTN effects.
 * Effects represent changes to the world state that result from executing a task.
 * This is a forward declaration and will be fully implemented in a later step.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNEffect : public UObject
{
	GENERATED_BODY()

public:
	UHTNEffect();

	/**
	 * Applies this effect to the given world state.
	 * 
	 * @param WorldState - The world state to modify
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Effect")
	void ApplyEffect(UHTNWorldState* WorldState) const;
	virtual void ApplyEffect_Implementation(UHTNWorldState* WorldState) const;

	/**
	 * Gets a human-readable description of this effect.
	 * 
	 * @return String description of the effect
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Effect")
	FString GetDescription() const;
	virtual FString GetDescription_Implementation() const;
};