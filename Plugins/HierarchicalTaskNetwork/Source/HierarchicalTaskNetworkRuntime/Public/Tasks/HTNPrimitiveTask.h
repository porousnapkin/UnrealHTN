// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "Conditions/HTNCondition.h"
#include "Effects/HTNEffect.h"
#include "HTNPrimitiveTask.generated.h"

/**
 * Delegate for task execution events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHTNTaskExecutionDelegate, UHTNPrimitiveTask*, Task);

/**
 * Base class for primitive (directly executable) HTN tasks.
 * Primitive tasks represent atomic actions that can be executed directly,
 * without further decomposition.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPrimitiveTask : public UHTNTask
{
    GENERATED_BODY()

public:
    UHTNPrimitiveTask();

    //~ Begin UObject Interface
    virtual void PostInitProperties() override;
    //~ End UObject Interface

    //~ Begin UHTNTask Interface
    virtual bool IsApplicable(const UHTNWorldState* WorldState) const override;
    virtual UHTNWorldState* GetExpectedEffects(const UHTNWorldState* WorldState) const override;
    virtual bool Decompose(const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks) override;
    //~ End UHTNTask Interface

    /**
     * Begin execution of this primitive task.
     * 
     * @param ExecutionContext - The current world state to use during execution
     * @return True if execution started successfully, false if it failed immediately
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual bool Execute(UHTNExecutionContext* ExecutionContext);

    /**
     * Check if this task has completed execution.
     * 
     * @return True if the task has finished (either succeeded or failed), false if still in progress
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual bool IsComplete() const;

    /**
     * Get the current execution status of this task.
     * 
     * @return Current status (InProgress, Succeeded, Failed, or Invalid)
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual EHTNTaskStatus GetStatus() const;

    /**
     * Called when the task is executed.
     * This is where the actual implementation of the task should go.
     * 
     * @param ExecutionContext
     * @return EHTNTaskStatus - The result of the execution
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
    EHTNTaskStatus ExecuteTask(UHTNExecutionContext* ExecutionContext);
    virtual EHTNTaskStatus ExecuteTask_Implementation(UHTNExecutionContext* ExecutionContext);

    /**
     * Called every tick while the task is executing.
     * Use this for ongoing tasks that need to update over time.
     * 
     * @param ExecutionContext
     * @param DeltaTime - Time in seconds since the last tick
     * @return EHTNTaskStatus - The current status of the task
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
    EHTNTaskStatus TickTask(UHTNExecutionContext* ExecutionContext, float DeltaTime);
    virtual EHTNTaskStatus TickTask_Implementation(UHTNExecutionContext* ExecutionContext, float DeltaTime);

    /**
     * Called when the task is ended, either through completion or abortion.
     * Use this for cleaning up resources.
     * 
     * @param ExecutionContext
     * @param FinalStatus - The final status of the task
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
    void EndTask(UHTNExecutionContext* ExecutionContext, EHTNTaskStatus FinalStatus);
    virtual void EndTask_Implementation(UHTNExecutionContext* ExecutionContext, EHTNTaskStatus FinalStatus);

    /**
     * Aborts the execution of this task.
     * 
     * @param ExecutionContext - The current world state
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual void AbortTask(UHTNExecutionContext* ExecutionContext);

    /**
     * Applies the expected effects of this task to the world state.
     * This is used during planning to simulate the execution of the task.
     * 
     * @param ExecutionContext - The world state to modify
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual void ApplyEffects(UHTNExecutionContext* ExecutionContext) const;

    /**
     * Sets the status of this task.
     * 
     * @param NewStatus - The new status
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    void SetStatus(EHTNTaskStatus NewStatus);

    /**
     * Validates that the task is set up correctly.
     * Checks preconditions and effects for validity.
     */
    virtual bool ValidateTask_Implementation() const override;

public:
    /** Called when the task starts execution */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Task")
    FHTNTaskExecutionDelegate OnTaskStarted;

    /** Called when the task completes successfully */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Task")
    FHTNTaskExecutionDelegate OnTaskSucceeded;

    /** Called when the task fails */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Task")
    FHTNTaskExecutionDelegate OnTaskFailed;

    /** Called when the task is aborted */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Task")
    FHTNTaskExecutionDelegate OnTaskAborted;

    /** Preconditions that must be satisfied for this task to be applicable */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Task|Preconditions")
    TArray<UHTNCondition*> Preconditions;

    /** Effects that will be applied to the world state when this task is executed */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Task|Effects")
    TArray<UHTNEffect*> Effects;

protected:
    /** Current execution status */
    UPROPERTY(BlueprintReadOnly, Category = "Task")
    EHTNTaskStatus Status;

    /** Time when execution began */
    UPROPERTY(BlueprintReadOnly, Category = "Task")
    float ExecutionStartTime;

    /** Maximum time this task should take to execute (in seconds, 0 = no limit) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task", meta = (ClampMin = "0.0"))
    float MaxExecutionTime;

    /** Whether this task is currently executing */
    UPROPERTY(BlueprintReadOnly, Category = "Task")
    uint8 bIsExecuting : 1;

private:
    /** Helper function to broadcast execution events */
    void BroadcastTaskEvent(EHTNTaskStatus NewStatus);
};