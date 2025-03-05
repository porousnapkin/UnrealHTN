// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNPlan.h"
#include "HTNTask.h"
#include "HTNPrimitiveTask.h"
#include "HTNPlanExecutor.generated.h"

/**
 * Delegate for plan execution events
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHTNPlanExecutionDelegate, const FHTNPlan&, Plan);

/**
 * Delegate for task execution events within a plan
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHTNPlanTaskExecutionDelegate, const FHTNPlan&, Plan, UHTNPrimitiveTask*, Task);

/**
 * Delegate for plan monitoring events (warnings, exceptions)
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHTNPlanExecutionMonitorDelegate, const FHTNPlan&, Plan, FName, MonitorType, const FString&, Message);

/**
 * Enum defining execution modes for the plan executor
 */
UENUM(BlueprintType)
enum class EHTNPlanExecutorMode : uint8
{
    /** Execute one task at a time, waiting for completion */
    Sequential UMETA(DisplayName = "Sequential"),
    
    /** Execute all tasks simultaneously if possible */
    Parallel UMETA(DisplayName = "Parallel"),
    
    /** Execute tasks based on their dependencies (partial ordering) */
    DependencyBased UMETA(DisplayName = "Dependency Based")
};

/**
 * Class responsible for executing HTN plans.
 * Manages the execution of tasks within a plan, handles execution state,
 * and provides callbacks for execution events.
 */
UCLASS(BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPlanExecutor : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    UHTNPlanExecutor();
    virtual ~UHTNPlanExecutor();

    //~ Begin FTickableGameObject Interface
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
    //~ End FTickableGameObject Interface

    /**
     * Start executing a plan.
     * 
     * @param InPlan - The plan to execute
     * @param WorldState - The initial world state to use
     * @param InOwner - The actor that owns this plan execution (typically the agent)
     * @return True if execution started successfully, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    bool StartPlan(const FHTNPlan& InPlan, UHTNWorldState* WorldState, AActor* InOwner = nullptr);

    /**
     * Pause the execution of the current plan.
     * 
     * @return True if the plan was paused, false if there was no plan or it was already paused
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    bool PausePlan();

    /**
     * Resume the execution of a paused plan.
     * 
     * @return True if the plan was resumed, false if there was no plan or it wasn't paused
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    bool ResumePlan();

    /**
     * Abort the execution of the current plan.
     * 
     * @param bFailTasks - Whether to mark remaining tasks as failed (true) or just abort them (false)
     * @return True if the plan was aborted, false if there was no plan
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    bool AbortPlan(bool bFailTasks = true);

    /**
     * Check if the executor is currently executing a plan.
     * 
     * @return True if a plan is being executed, false otherwise
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    bool IsExecutingPlan() const;

    /**
     * Get the current plan being executed.
     * 
     * @return The current plan
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    const FHTNPlan& GetCurrentPlan() const;

    /**
     * Get the world state being used for execution.
     * 
     * @return The world state
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    UHTNWorldState* GetWorldState() const;

    /**
     * Get the actor that owns this plan execution.
     * 
     * @return The owner actor
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    AActor* GetOwner() const;

    /**
     * Set the execution mode.
     * 
     * @param InExecutionMode - The new execution mode
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    void SetExecutionMode(EHTNPlanExecutorMode InExecutionMode);

    /**
     * Set the maximum time a task can execute before timing out.
     * 
     * @param InMaxTaskExecutionTime - The maximum time in seconds (0 = no limit)
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    void SetMaxTaskExecutionTime(float InMaxTaskExecutionTime);

    /**
     * Set whether to automatically abort the plan when a task fails.
     * 
     * @param bInAbortOnTaskFailure - Whether to abort on task failure
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    void SetAbortOnTaskFailure(bool bInAbortOnTaskFailure);

    /**
     * Get the current task being executed.
     * 
     * @return The current task, or nullptr if no task is being executed
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    UHTNPrimitiveTask* GetCurrentTask() const;

    /**
     * Check if a task is currently executing.
     * 
     * @param Task - The task to check
     * @return True if the task is currently executing, false otherwise
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    bool IsTaskExecuting(UHTNPrimitiveTask* Task) const;

    /**
     * Get the status of a task in the current plan.
     * 
     * @param Task - The task to check
     * @return The status of the task
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Execution")
    EHTNTaskStatus GetTaskStatus(UHTNPrimitiveTask* Task) const;

    /**
     * Execute the next task in the plan.
     * This is called automatically by the executor, but can be called manually
     * to force execution of the next task.
     * 
     * @return True if a task was executed, false if no tasks are left or execution failed
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Execution")
    bool ExecuteNextTask();

    /**
     * Create a string representation of the current execution state for debugging.
     * 
     * @return Debug string of the execution state
     */
    UFUNCTION(BlueprintPure, Category = "HTN|Debug")
    FString ToString() const;

public:
    /** Called when plan execution starts */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionDelegate OnPlanStarted;

    /** Called when plan execution completes successfully */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionDelegate OnPlanCompleted;

    /** Called when plan execution fails */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionDelegate OnPlanFailed;

    /** Called when plan execution is aborted */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionDelegate OnPlanAborted;

    /** Called when plan execution is paused */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionDelegate OnPlanPaused;

    /** Called when plan execution is resumed */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionDelegate OnPlanResumed;

    /** Called when a task starts execution */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanTaskExecutionDelegate OnTaskStarted;

    /** Called when a task completes successfully */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanTaskExecutionDelegate OnTaskSucceeded;

    /** Called when a task fails */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanTaskExecutionDelegate OnTaskFailed;

    /** Called when a task times out */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanTaskExecutionDelegate OnTaskTimeout;

    /** Called when a monitoring event occurs */
    UPROPERTY(BlueprintAssignable, Category = "HTN|Execution")
    FHTNPlanExecutionMonitorDelegate OnMonitorEvent;

protected:
    /** The plan being executed */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Execution")
    FHTNPlan CurrentPlan;

    /** The current execution mode */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTN|Execution")
    EHTNPlanExecutorMode ExecutionMode;

    /** The world state being used for execution */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Execution")
    UHTNWorldState* CurrentWorldState;

    /** The actor that owns this plan execution */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Execution")
    AActor* OwnerActor;

    /** The maximum time a task can execute before timing out (0 = no limit) */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTN|Execution", meta = (ClampMin = "0.0"))
    float MaxTaskExecutionTime;

    /** Whether to abort the plan when a task fails */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "HTN|Execution")
    bool bAbortOnTaskFailure;

    /** Tasks that are currently executing (for parallel execution) */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Execution")
    TArray<UHTNPrimitiveTask*> ExecutingTasks;

    /** Map of task execution start times (for timeout detection) */
    TMap<UHTNPrimitiveTask*, float> TaskStartTimes;

protected:
    /**
     * Called when a task completes execution.
     * 
     * @param Task - The task that completed
     * @param Status - The final status of the task
     */
    void OnTaskCompleted(UHTNPrimitiveTask* Task, EHTNTaskStatus Status);

    /**
     * Check if any tasks have timed out.
     * 
     * @param CurrentTime - The current time
     */
    void CheckTaskTimeouts(float CurrentTime);

    /**
     * Check if the plan has completed.
     * 
     * @return True if the plan has completed, false otherwise
     */
    bool CheckPlanCompletion();

    /**
     * Clean up resources when a plan completes or is aborted.
     */
    void CleanupPlan();

    /**
     * Apply the effects of a completed task to the world state.
     * 
     * @param Task - The completed task
     */
    void ApplyTaskEffects(UHTNPrimitiveTask* Task);

    /**
     * Log an execution message.
     * 
     * @param Message - The message to log
     * @param Verbosity - The verbosity level
     */
    void LogExecution(const FString& Message, ELogVerbosity::Type Verbosity = ELogVerbosity::Verbose);

    /**
     * Trigger a monitor event.
     * 
     * @param MonitorType - The type of event
     * @param Message - The event message
     */
    void TriggerMonitorEvent(FName MonitorType, const FString& Message);

private:
    /** Whether a plan is currently being executed */
    bool bIsExecuting;

    /** Whether execution is paused */
    bool bIsPaused;

    /** Timestamp when the plan started executing */
    float PlanStartTime;
};