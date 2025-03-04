// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNPlannerInterface.h"
#include "HTNTask.h"
#include "HTNCompoundTask.h"
#include "HTNPrimitiveTask.h"
#include "HTNDFSPlanner.generated.h"

/**
 * Depth-First Search planner implementation for the HTN system.
 * This planner recursively decomposes compound tasks into primitive tasks
 * to generate valid plans.
 */
UCLASS(Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNDFSPlanner : public UObject, public IHTNPlannerInterface
{
    GENERATED_BODY()

public:
    UHTNDFSPlanner();
    virtual ~UHTNDFSPlanner();

    //~ Begin IHTNPlannerInterface
    virtual FHTNPlannerResult GeneratePlan_Implementation(
        const UHTNWorldState* WorldState,
        const TArray<UHTNTask*>& GoalTasks,
        const FHTNPlanningConfig& Config) override;

    virtual bool ValidatePlan_Implementation(
        const FHTNPlan& Plan,
        const UHTNWorldState* WorldState) override;

    virtual FHTNPlannerResult GeneratePartialPlan_Implementation(
        const FHTNPlan& ExistingPlan,
        const UHTNWorldState* WorldState,
        const TArray<UHTNTask*>& GoalTasks,
        const FHTNPlanningConfig& Config) override;

    virtual void ConfigurePlanner_Implementation(const FHTNPlanningConfig& NewConfig) override;
    //~ End IHTNPlannerInterface

protected:
    /** Current configuration for the planner */
    UPROPERTY()
    FHTNPlanningConfig Configuration;

    /** Planning metrics for the most recent planning operation */
    struct FPlanningMetrics
    {
        int32 NodesExplored;
        int32 PlansGenerated;
        int32 MaxDepthReached;
        float StartTime;
        float EndTime;
        FString DebugInfo;

        FPlanningMetrics()
            : NodesExplored(0)
            , PlansGenerated(0)
            , MaxDepthReached(0)
            , StartTime(0.0f)
            , EndTime(0.0f)
        {
        }

        void Reset()
        {
            NodesExplored = 0;
            PlansGenerated = 0;
            MaxDepthReached = 0;
            StartTime = FPlatformTime::Seconds();
            EndTime = 0.0f;
            DebugInfo.Reset();
        }

        void Finish()
        {
            EndTime = FPlatformTime::Seconds();
        }

        float GetElapsedTime() const
        {
            return EndTime > StartTime ? EndTime - StartTime : 0.0f;
        }

        void AppendDebugInfo(const FString& Info, bool bDetailedDebugging)
        {
            if (bDetailedDebugging)
            {
                DebugInfo.Append(Info);
                DebugInfo.Append(TEXT("\n"));
            }
        }
    };

    /** Current metrics for the ongoing planning operation */
    FPlanningMetrics Metrics;

    /**
     * Recursive depth-first search function to find a valid plan.
     * 
     * @param WorldState - The current world state
     * @param RemainingTasks - Tasks that still need to be processed
     * @param CurrentPlan - The plan built so far
     * @param CurrentDepth - Current recursion depth
     * @param OutPlan - The resulting plan if successful
     * @return True if a valid plan was found, false otherwise
     */
    bool FindPlanDFS(
        const UHTNWorldState* WorldState,
        const TArray<UHTNTask*>& RemainingTasks,
        const TArray<UHTNPrimitiveTask*>& CurrentPlan,
        int32 CurrentDepth,
        FHTNPlan& OutPlan);

    /**
     * Process a single task during planning (either decompose or add to plan).
     * 
     * @param WorldState - The current world state
     * @param Task - The task to process
     * @param RemainingTasks - Remaining tasks after this one
     * @param CurrentPlan - The plan built so far
     * @param CurrentDepth - Current recursion depth
     * @param OutPlan - The resulting plan if successful
     * @return True if processing was successful, false otherwise
     */
    bool ProcessTask(
        const UHTNWorldState* WorldState,
        UHTNTask* Task,
        const TArray<UHTNTask*>& RemainingTasks,
        const TArray<UHTNPrimitiveTask*>& CurrentPlan,
        int32 CurrentDepth,
        FHTNPlan& OutPlan);

    /**
     * Apply a primitive task's expected effects to the world state.
     * 
     * @param WorldState - The world state to modify
     * @param Task - The primitive task to apply
     * @return True if effects were applied successfully, false otherwise
     */
    bool ApplyTaskEffects(
        UHTNWorldState* WorldState,
        UHTNPrimitiveTask* Task);

    /**
     * Check if planning should be aborted due to timeout or max depth.
     * 
     * @param CurrentDepth - Current recursion depth
     * @return True if planning should be aborted, false otherwise
     */
    bool ShouldAbortPlanning(int32 CurrentDepth) const;

    /**
     * Populate a planner result structure with the current metrics.
     * 
     * @param Success - Whether planning was successful
     * @param Plan - The generated plan (only valid if Success is true)
     * @param FailReason - The reason for failure (only valid if Success is false)
     * @return The populated planner result structure
     */
    FHTNPlannerResult CreatePlannerResult(bool Success, const FHTNPlan& Plan, EHTNPlannerFailReason FailReason);
};