#include "HTNPlannerBase.h"
#include "HTNLogging.h"
#include "HTNPrimitiveTask.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"


//////////////////////////////////////////////////////////////////////////
// FHTNPlannerResult Implementation
//////////////////////////////////////////////////////////////////////////

FHTNPlannerResult::FHTNPlannerResult()
    : bSuccess(false)
    , FailReason(EHTNPlannerFailReason::None)
    , NodesExplored(0)
    , PlansGenerated(0)
    , MaxDepthReached(0)
    , PlanningTime(0.0f)
{
}

FString FHTNPlannerResult::ToString() const
{
    FString Result;
    if (bSuccess)
    {
        Result = FString::Printf(TEXT("Planning Successful\n"));
        Result += Plan.ToString();
    }
    else
    {
        Result = FString::Printf(TEXT("Planning Failed: %s\n"), 
            *StaticEnum<EHTNPlannerFailReason>()->GetNameStringByValue(static_cast<int64>(FailReason)));
    }
    
    // Add metrics
    Result += FString::Printf(TEXT("\nMetrics:\n"));
    Result += FString::Printf(TEXT("  Nodes Explored: %d\n"), NodesExplored);
    Result += FString::Printf(TEXT("  Plans Generated: %d\n"), PlansGenerated);
    Result += FString::Printf(TEXT("  Max Depth Reached: %d\n"), MaxDepthReached);
    Result += FString::Printf(TEXT("  Planning Time: %.4f seconds\n"), PlanningTime);
    
    // Add debug info if available
    if (!DebugInfo.IsEmpty())
    {
        Result += FString::Printf(TEXT("\nDebug Info:\n%s\n"), *DebugInfo);
    }
    
    return Result;
}

//////////////////////////////////////////////////////////////////////////
// FHTNPlanningConfig Implementation
//////////////////////////////////////////////////////////////////////////

FHTNPlanningConfig::FHTNPlanningConfig()
    : MaxSearchDepth(10)
    , PlanningTimeout(1.0f)
    , MaxPlansToConsider(100)
    , bUseHeuristics(true)
    , HeuristicWeight(0.5f)
    , bCacheDecompositions(true)
    , bDetailedDebugging(false)
{
}

UHTNPlannerBase::UHTNPlannerBase()
{
    // Initialize with default configuration
    Configuration = FHTNPlanningConfig();
}

UHTNPlannerBase::~UHTNPlannerBase()
{
    // No specific cleanup needed
}

FHTNPlannerResult UHTNPlannerBase::GeneratePlan(
    const UHTNWorldState* WorldState,
    const TArray<UHTNTask*>& GoalTasks,
    const FHTNPlanningConfig& Config)
{
    // Base implementation just returns a failed result
    UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("GeneratePlan called on base planner class. This should be overridden by derived classes."));
    
    FHTNPlannerResult Result;
    Result.bSuccess = false;
    Result.FailReason = EHTNPlannerFailReason::UnexpectedError;
    
    return Result;
}

bool UHTNPlannerBase::ValidatePlan(
    const FHTNPlan& Plan,
    const UHTNWorldState* WorldState)
{
    // Base implementation just returns false
    UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ValidatePlan called on base planner class. This should be overridden by derived classes."));
    return false;
}

FHTNPlannerResult UHTNPlannerBase::GeneratePartialPlan(
    const FHTNPlan& ExistingPlan,
    const UHTNWorldState* WorldState,
    const TArray<UHTNTask*>& GoalTasks,
    const FHTNPlanningConfig& Config)
{
    // Base implementation just returns a failed result
    UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("GeneratePartialPlan called on base planner class. This should be overridden by derived classes."));
    
    FHTNPlannerResult Result;
    Result.bSuccess = false;
    Result.FailReason = EHTNPlannerFailReason::UnexpectedError;
    
    return Result;
}

void UHTNPlannerBase::ConfigurePlanner(const FHTNPlanningConfig& NewConfig)
{
    // Update configuration
    Configuration = NewConfig;
    
    UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("Planner configured with MaxSearchDepth=%d, Timeout=%.2fs"), 
        Configuration.MaxSearchDepth, Configuration.PlanningTimeout);
}