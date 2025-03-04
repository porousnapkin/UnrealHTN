// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "HTNTaskInterface.h"
#include "HTNCondition.h"
#include "HTNEffect.h"
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
UCLASS(Abstract, BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPrimitiveTask : public UHTNTask, public IHTNPrimitiveTaskInterface
{
    GENERATED_BODY()

public:
    UHTNPrimitiveTask();

    //~ Begin UObject Interface
    virtual void PostInitProperties() override;
    //~ End UObject Interface

    //~ Begin IHTNTaskInterface
    virtual bool IsApplicable_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState) const override;
    virtual void GetExpectedEffects_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TScriptInterface<IHTNWorldStateInterface>& OutEffects) const override;
    virtual bool Decompose_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNPrimitiveTask*>& OutTasks) override;
    //~ End IHTNTaskInterface

    //~ Begin IHTNPrimitiveTaskInterface
    virtual bool Execute_Implementation(TScriptInterface<IHTNWorldStateInterface>& WorldState) override;
    virtual bool IsComplete_Implementation() const override;
    virtual EHTNTaskStatus GetStatus_Implementation() const override;
    //~ End IHTNPrimitiveTaskInterface

    /**
     * Called when the task is executed.
     * This is where the actual implementation of the task should go.
     * 
     * @param WorldState - The current world state
     * @return EHTNTaskStatus - The result of the execution
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
    EHTNTaskStatus ExecuteTask(TScriptInterface<IHTNWorldStateInterface>& WorldState);
    virtual EHTNTaskStatus ExecuteTask_Implementation(TScriptInterface<IHTNWorldStateInterface>& WorldState);

    /**
     * Called every tick while the task is executing.
     * Use this for ongoing tasks that need to update over time.
     * 
     * @param WorldState - The current world state
     * @param DeltaTime - Time in seconds since the last tick
     * @return EHTNTaskStatus - The current status of the task
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
    EHTNTaskStatus TickTask(TScriptInterface<IHTNWorldStateInterface>& WorldState, float DeltaTime);
    virtual EHTNTaskStatus TickTask_Implementation(TScriptInterface<IHTNWorldStateInterface>& WorldState, float DeltaTime);

    /**
     * Called when the task is ended, either through completion or abortion.
     * Use this for cleaning up resources.
     * 
     * @param WorldState - The current world state
     * @param FinalStatus - The final status of the task
     */
    UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
    void EndTask(TScriptInterface<IHTNWorldStateInterface>& WorldState, EHTNTaskStatus FinalStatus);
    virtual void EndTask_Implementation(TScriptInterface<IHTNWorldStateInterface>& WorldState, EHTNTaskStatus FinalStatus);

    /**
     * Aborts the execution of this task.
     * 
     * @param WorldState - The current world state
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual void AbortTask(TScriptInterface<IHTNWorldStateInterface>& WorldState);

    /**
     * Applies the expected effects of this task to the world state.
     * This is used during planning to simulate the execution of the task.
     * 
     * @param WorldState - The world state to modify
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Task")
    virtual void ApplyEffects(TScriptInterface<IHTNWorldStateInterface>& WorldState) const;

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

protected:
    /** Preconditions that must be satisfied for this task to be applicable */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Task|Preconditions")
    TArray<UHTNCondition*> Preconditions;

    /** Effects that will be applied to the world state when this task is executed */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Instanced, Category = "Task|Effects")
    TArray<UHTNEffect*> Effects;

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