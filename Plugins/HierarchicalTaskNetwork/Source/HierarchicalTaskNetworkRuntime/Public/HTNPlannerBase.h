// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNPlan.h"
#include "HTNTask.h"
#include "HTNPlannerBase.generated.h"

/**
 * Enum defining the possible reasons a planning operation might fail
 */
UENUM(BlueprintType)
enum class EHTNPlannerFailReason : uint8
{
    /** No failure, planning was successful */
    None UMETA(DisplayName = "None"),
    
    /** No applicable methods were found for a compound task */
    NoApplicableMethods UMETA(DisplayName = "No Applicable Methods"),
    
    /** A primitive task's preconditions were not satisfied */
    PreconditionFailed UMETA(DisplayName = "Precondition Failed"),
    
    /** Maximum planning depth was reached */
    MaxDepthReached UMETA(DisplayName = "Max Depth Reached"),
    
    /** Planning operation timed out */
    Timeout UMETA(DisplayName = "Timeout"),
    
    /** Maximum number of plans to consider was reached */
    MaxPlansReached UMETA(DisplayName = "Max Plans Reached"),
    
    /** No valid plan could be found */
    NoValidPlan UMETA(DisplayName = "No Valid Plan"),
    
    /** An unexpected error occurred during planning */
    UnexpectedError UMETA(DisplayName = "Unexpected Error")
};

/**
 * Structure containing the results of a planning operation.
 */
USTRUCT(BlueprintType)
struct HIERARCHICALTASKNETWORKRUNTIME_API FHTNPlannerResult
{
    GENERATED_BODY()

public:
    /** Default constructor */
    FHTNPlannerResult();
    
    /** The generated plan (only valid if bSuccess is true) */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner")
    FHTNPlan Plan;
    
    /** Whether the planning operation was successful */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner")
    uint8 bSuccess : 1;
    
    /** The reason for planning failure (only valid if bSuccess is false) */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner")
    EHTNPlannerFailReason FailReason;
    
    /** How many nodes were explored during planning */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner|Metrics")
    int32 NodesExplored;
    
    /** How many plans were generated and evaluated */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner|Metrics")
    int32 PlansGenerated;
    
    /** Maximum depth reached during planning */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner|Metrics")
    int32 MaxDepthReached;
    
    /** Time spent planning (in seconds) */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner|Metrics")
    float PlanningTime;
    
    /** Detailed information about the planning process for debugging */
    UPROPERTY(BlueprintReadOnly, Category = "HTN|Planner|Debug")
    FString DebugInfo;
    
    /**
     * Gets a string representation of the planning result for debugging.
     * 
     * @return A formatted string describing the planning result
     */
    FString ToString() const;
};

/**
 * Structure containing configuration parameters for planning operations.
 */
USTRUCT(BlueprintType)
struct HIERARCHICALTASKNETWORKRUNTIME_API FHTNPlanningConfig
{
    GENERATED_BODY()

public:
    /** Default constructor with reasonable defaults */
    FHTNPlanningConfig();
    
    /** Maximum depth for the planning search (to prevent infinite recursion) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config", meta = (ClampMin = "1", ClampMax = "100"))
    int32 MaxSearchDepth;
    
    /** Maximum time allowed for planning (in seconds, 0 = no limit) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config", meta = (ClampMin = "0.0"))
    float PlanningTimeout;
    
    /** Maximum number of plans to consider before selecting the best one (0 = no limit) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config", meta = (ClampMin = "0"))
    int32 MaxPlansToConsider;
    
    /** Whether to use heuristic optimizations during planning */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config")
    uint8 bUseHeuristics : 1;
    
    /** Weight for the heuristic function (0.0 = ignore heuristics, 1.0 = fully rely on heuristics) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config", meta = (ClampMin = "0.0", ClampMax = "1.0", EditCondition = "bUseHeuristics"))
    float HeuristicWeight;
    
    /** Whether to cache partial decompositions for reuse */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config")
    uint8 bCacheDecompositions : 1;
    
    /** Whether to enable detailed debugging output */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HTN|Planner|Config")
    uint8 bDetailedDebugging : 1;
};

/**
 * Base class for HTN planners.
 * Provides the foundation for different HTN planner implementations.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPlannerBase : public UObject
{
    GENERATED_BODY()

public:
    UHTNPlannerBase();
    virtual ~UHTNPlannerBase();
    
    /**
     * Generate a plan to achieve the specified goal tasks from the given world state.
     * 
     * @param WorldState - The initial world state
     * @param GoalTasks - The tasks to achieve (typically compound tasks that will be decomposed)
     * @param Config - Configuration parameters for the planning process
     * @return The result of the planning operation
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Planner", meta = (DisplayName = "Generate Plan"))
    virtual FHTNPlannerResult GeneratePlan(
        const UHTNWorldState* WorldState,
        const TArray<UHTNTask*>& GoalTasks,
        const FHTNPlanningConfig& Config);
    
    /**
     * Validate if a plan is still valid given the current world state.
     * 
     * @param Plan - The plan to validate
     * @param WorldState - The current world state
     * @return True if the plan is still valid, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Planner", meta = (DisplayName = "Validate Plan"))
    virtual bool ValidatePlan(
        const FHTNPlan& Plan,
        const UHTNWorldState* WorldState);
    
    /**
     * Generate a partial plan that extends an existing plan.
     * Useful for incremental planning during execution.
     * 
     * @param ExistingPlan - The existing plan to extend
     * @param WorldState - The current world state
     * @param GoalTasks - The tasks to achieve
     * @param Config - Configuration parameters for the planning process
     * @return The result of the planning operation
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Planner", meta = (DisplayName = "Generate Partial Plan"))
    virtual FHTNPlannerResult GeneratePartialPlan(
        const FHTNPlan& ExistingPlan,
        const UHTNWorldState* WorldState,
        const TArray<UHTNTask*>& GoalTasks,
        const FHTNPlanningConfig& Config);
    
    /**
     * Configure the planner with the provided parameters.
     * This allows changing the planning behavior without creating a new planner.
     * 
     * @param NewConfig - The new configuration parameters
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|Planner", meta = (DisplayName = "Configure Planner"))
    virtual void ConfigurePlanner(const FHTNPlanningConfig& NewConfig);

protected:
    /** Current configuration for the planner */
    UPROPERTY()
    FHTNPlanningConfig Configuration;
};