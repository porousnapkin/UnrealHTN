// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "HTNMethod.h"
#include "HTNCompoundTask.generated.h"

/**
 * Base class for compound HTN tasks.
 * Compound tasks represent higher-level tasks that must be decomposed into 
 * simpler tasks before they can be executed.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNCompoundTask : public UHTNTask
{
    GENERATED_BODY()

public:
    UHTNCompoundTask();

    //~ Begin UObject Interface
    virtual void PostInitProperties() override;
    virtual void BeginDestroy() override;
    //~ End UObject Interface

    //~ Begin UHTNTask Interface
    virtual bool Decompose(const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks) override;
    virtual bool IsApplicable(const UHTNWorldState* WorldState) const override;
    //~ End UHTNTask Interface

    /**
     * Get all methods (alternative decompositions) that are available for this task.
     * 
     * @param WorldState - The current world state
     * @param OutMethods - The available methods for this compound task
     * @return True if at least one method is available, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual bool GetAvailableMethods(const UHTNWorldState* WorldState, TArray<class UHTNMethod*>& OutMethods) const;

    /**
     * Apply a selected method to decompose this task into subtasks.
     * 
     * @param Method - The method to apply
     * @param WorldState - The current world state
     * @param OutTasks - The resulting tasks after decomposition
     * @return True if the method was successfully applied, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual bool ApplyMethod(class UHTNMethod* Method, const UHTNWorldState* WorldState, TArray<UHTNTask*>& OutTasks) const;

    /**
     * Validates that the compound task is set up correctly.
     * Checks all methods for validity.
     */
    virtual bool ValidateTask_Implementation() const override;

    /**
     * Gets all methods available for this compound task.
     * 
     * @return Array of all methods
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    const TArray<UHTNMethod*>& GetMethods() const { return Methods; }

    /**
     * Gets a string representation of the decomposition tree for debugging.
     * 
     * @param Indent - Current indentation level for formatting
     * @return String representation of the decomposition tree
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Debug")
    FString GetDecompositionTreeString(int32 Indent = 0) const;

protected:
    /** Methods that can be used to decompose this task */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Task|Methods")
    TArray<UHTNMethod*> Methods;

    /** Maximum depth for recursive decomposition to prevent infinite loops */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task|Settings", meta = (ClampMin = "1", ClampMax = "100"))
    int32 MaxDecompositionDepth;

    /** Currently active decomposition depth (for recursion tracking) */
    UPROPERTY(Transient)
    mutable int32 CurrentDecompositionDepth;

private:
    /**
     * Helper function to decompose a task recursively.
     * 
     * @param Task - The task to decompose
     * @param WorldState - The current world state
     * @param OutTasks - The resulting primitive tasks after decomposition
     * @param CurrentDepth - Current recursion depth
     * @return True if decomposition was successful, false otherwise
     */
    bool DecomposeTaskRecursively(UHTNTask* Task, const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks, int32 CurrentDepth) const;

    /**
     * Selects the best applicable method from available methods.
     * 
     * @param WorldState - The current world state
     * @return The best applicable method, or nullptr if none are applicable
     */
    UHTNMethod* SelectBestMethod(const UHTNWorldState* WorldState) const;
};