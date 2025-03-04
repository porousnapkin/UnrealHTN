// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNCondition.h"
#include "HTNMethod.generated.h"

class UHTNTask;

/**
 * Represents a specific way to decompose a compound task into subtasks.
 * Compound tasks can have multiple methods representing different approaches to achieve the same goal.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNMethod : public UObject
{
    GENERATED_BODY()

public:
    UHTNMethod();

    /**
     * Checks if this method is applicable in the given world state.
     * 
     * @param WorldState - The world state to check against
     * @return True if the method is applicable, false otherwise
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Method")
    bool IsApplicable(const UHTNWorldState* WorldState) const;
    virtual bool IsApplicable_Implementation(const UHTNWorldState* WorldState) const;

    /**
     * Gets the subtasks this method provides for decomposition.
     * 
     * @return Array of subtasks
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Method")
    const TArray<UHTNTask*>& GetSubtasks() const { return Subtasks; }

    /**
     * Gets a human-readable description of this method.
     * 
     * @return String description of the method
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Method")
    FString GetDescription() const;
    virtual FString GetDescription_Implementation() const;

    /**
     * Validates that this method is set up correctly.
     * 
     * @return True if valid, false otherwise
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Method")
    bool ValidateMethod() const;
    virtual bool ValidateMethod_Implementation() const;
 
    /** Display name of the method */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Method")
    FName MethodName;

    /** Detailed description of what this method does */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Method", meta = (MultiLine = true))
    FString Description;

    /** Priority of this method (higher values are tried first) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Method", meta = (ClampMin = "0.0"))
    float Priority;
 
    /** Conditions that must be satisfied for this method to be applicable */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Method|Conditions")
    TArray<UHTNCondition*> Conditions;

    /** Subtasks that this method provides for decomposition */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Method|Subtasks")
    TArray<UHTNTask*> Subtasks;
};