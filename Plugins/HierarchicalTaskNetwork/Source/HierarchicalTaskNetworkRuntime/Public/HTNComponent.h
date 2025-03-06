// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNDFSPlanner.h"
#include "Components/ActorComponent.h"
#include "HTNWorldStateStruct.h"
#include "HTNExecutionContext.h"
#include "HTNPlan.h"
#include "HTNComponent.generated.h"

class UHTNPlanExecutor;

/**
 * Component that manages HTN planning and plan execution for an actor.
 * This component integrates with the HTN system to drive AI behavior.
 */
UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    /** Default constructor */
    UHTNComponent();

    //~ Begin UActorComponent Interface
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    //~ End UActorComponent Interface

    /**
     * Generates a new plan with the current world state.
     * 
     * @param GoalTasks - The goal tasks to plan for
     * @return True if a plan was successfully generated, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool GeneratePlan(const TArray<class UHTNTask*>& GoalTasks);

    /**
     * Checks if the current plan is still valid.
     * 
     * @return True if the plan is valid, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool IsPlanValid() const;

    /**
     * Tries to replan if the current plan is invalid.
     * 
     * @param GoalTasks - The goal tasks to plan for
     * @return True if replanning was successful or not needed, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool TryReplan(const TArray<class UHTNTask*>& GoalTasks);

    /**
     * Checks if the current plan needs to be replanned.
     * This checks for significant world state changes that invalidate the plan.
     * 
     * @return True if replanning is needed, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool NeedsReplan() const;

    /**
     * Attempts to replan automatically if conditions indicate it's necessary.
     * This is useful to call periodically to keep plans valid with changing world states.
     * 
     * @return True if replanning was successful or not needed, false if replanning failed
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool AutoReplan();

    /**
     * Configures if automatic replanning should be done during ticking.
     * 
     * @param bEnable - Whether to enable automatic replanning
     * @param CheckInterval - How often to check for replan needs (in seconds)
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    void SetAutoReplanEnabled(bool bEnable, float CheckInterval = 0.5f);

    /**
     * Handles basic error recovery when the plan fails
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    void HandlePlanFailure();

    /**
     * Gets the current execution context.
     * 
     * @return The execution context
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    UHTNExecutionContext* GetExecutionContext() const;

    /**
     * Gets the world state.
     * 
     * @return The world state
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    UHTNWorldState* GetWorldState() const;

    /**
     * Sets the world state.
     * 
     * @param InWorldState - The world state to use
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    void SetWorldState(UHTNWorldState* InWorldState);

    /**
     * Gets the current plan.
     * 
     * @return The current plan
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    const FHTNPlan& GetCurrentPlan() const;

    /**
     * Checks if a plan is currently executing.
     * 
     * @return True if a plan is executing, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool IsExecutingPlan() const;

    /**
     * Aborts the current plan execution.
     * 
     * @param bFailTasks - Whether to mark remaining tasks as failed
     * @return True if a plan was aborted, false if there was no plan
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN")
    bool AbortPlan(bool bFailTasks = true);

    /**
     * Creates a debug string representation of the component's state.
     * 
     * @return A debug string representation
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN|Debug")
    FString GetDebugInfo() const;

     /**
     * Enable or disable debug visualization for this component.
     * 
     * @param bEnable - Whether to enable debug visualization
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN|Debug")
    void SetDebugVisualization(bool bEnable);
       
    /**
     * Creates a visualization component and attaches it to the owner of this component.
     * 
     * @return The created visualization component
     */
    UFUNCTION(BlueprintCallable, Category = "AI|HTN|Debug")
    class UHTNDebugVisualizationComponent* CreateVisualizationComponent();

protected:
    /** World state for planning and execution */
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI|HTN")
    UHTNWorldState* WorldState;

    /** Execution context */
    UPROPERTY(BlueprintReadOnly, Category = "AI|HTN")
    UHTNExecutionContext* ExecutionContext;

    /** Plan executor */
    UPROPERTY(BlueprintReadOnly, Category = "AI|HTN")
    UHTNPlanExecutor* PlanExecutor;

    /** Whether to enable debug output */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN|Debug")
    bool bDebugOutput;

    UPROPERTY(BlueprintReadOnly, Category = "AI|HTN")
    UHTNDFSPlanner* Planner;

   private:
    /** Initializes the component */
    void Initialize();

    /** Outputs a debug message */
    void DebugMessage(const FString& Message) const;
    
    /** Whether automatic replanning is enabled */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN", meta = (AllowPrivateAccess = "true"))
    bool bAutoReplanEnabled;
    
    /** How often to check for replan needs (in seconds) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|HTN", meta = (AllowPrivateAccess = "true", ClampMin = "0.1"))
    float ReplanCheckInterval;
    
    /** Time of the last replan check */
    float LastReplanCheckTime;
    
    /** The goal tasks used for the current plan (saved for replanning) */
    UPROPERTY()
    TArray<UHTNTask*> CurrentGoalTasks;
    
    /** Number of consecutive plan failures */
    int32 ConsecutivePlanFailures;

    FHTNPlan EmptyPlan;
};