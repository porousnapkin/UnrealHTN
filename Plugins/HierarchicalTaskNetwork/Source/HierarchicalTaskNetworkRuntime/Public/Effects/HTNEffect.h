// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNWorldStateStruct.h"
#include "HTNEffect.generated.h"

/**
 * Base class for HTN effects.
 * Effects represent changes to the world state that result from executing a task.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
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

	/**
	 * Validates that this effect is set up correctly.
	 * 
	 * @return True if the effect is valid, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Effect")
	bool ValidateEffect() const;
	virtual bool ValidateEffect_Implementation() const;
    
	/** Debug color for visualization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect|Debug")
	FLinearColor DebugColor;
};