// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNWorldStateStruct.h"
#include "HTNCondition.generated.h"

/**
 * Base class for HTN conditions.
 * Conditions represent checks that must be satisfied for a task to be applicable.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNCondition : public UObject
{
	GENERATED_BODY()

public:
	UHTNCondition();

	/**
	 * Checks if this condition is satisfied in the given world state.
	 * 
	 * @param WorldState - The world state to check against
	 * @return True if the condition is satisfied, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Condition")
	bool CheckCondition(const UHTNWorldState* WorldState) const;
	virtual bool CheckCondition_Implementation(const UHTNWorldState* WorldState) const;

	/**
	 * Gets a human-readable description of this condition.
	 * 
	 * @return String description of the condition
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Condition")
	FString GetDescription() const;
	virtual FString GetDescription_Implementation() const;

	/**
	 * Validates that this condition is set up correctly.
	 * 
	 * @return True if the condition is valid, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Condition")
	bool ValidateCondition() const;
	virtual bool ValidateCondition_Implementation() const;
    
	/** Debug color for visualization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition|Debug")
	FLinearColor DebugColor;
};