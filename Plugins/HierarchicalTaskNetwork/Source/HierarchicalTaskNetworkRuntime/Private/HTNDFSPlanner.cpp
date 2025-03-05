// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNDFSPlanner.h"

#include "Tasks/HTNCompoundTask.h"
#include "HTNLogging.h"
#include "HTNWorldStateStruct.h"

UHTNDFSPlanner::UHTNDFSPlanner()
{
    // Initialize with default configuration
    Configuration = FHTNPlanningConfig();
}

UHTNDFSPlanner::~UHTNDFSPlanner()
{
    // No specific cleanup needed
}

FHTNPlannerResult UHTNDFSPlanner::GeneratePlan(
    const UHTNWorldState* WorldState,
    const TArray<UHTNTask*>& GoalTasks,
    const FHTNPlanningConfig& Config)
{
    // Start with empty plan
    FHTNPlan ResultPlan;
    
    // Validate inputs
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Invalid world state provided"));
        return CreatePlannerResult(false, ResultPlan, EHTNPlannerFailReason::UnexpectedError);
    }
    
    if (GoalTasks.Num() == 0)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: No goal tasks provided"));
        return CreatePlannerResult(true, ResultPlan, EHTNPlannerFailReason::None);
    }
    
    // Set up configuration and metrics
    Configuration = Config;
    Metrics.Reset();
    
    if (Configuration.bDetailedDebugging)
    {
        UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNDFSPlanner: Starting plan generation for %d goal tasks"), GoalTasks.Num());
        Metrics.AppendDebugInfo(FString::Printf(TEXT("Starting plan generation for %d goal tasks"), GoalTasks.Num()), Configuration.bDetailedDebugging);
        Metrics.AppendDebugInfo(TEXT("Initial world state:"), Configuration.bDetailedDebugging);
        Metrics.AppendDebugInfo(WorldState->ToString(), Configuration.bDetailedDebugging);
    }
    
    // Clone the world state to avoid modifying the original
    UHTNWorldState* WorkingState = WorldState->Clone();
    
    // Set up empty plan
    TArray<UHTNPrimitiveTask*> CurrentPlan;
    
    // Start recursive search
    bool bSuccess = FindPlanDFS(WorkingState, GoalTasks, CurrentPlan, 0, ResultPlan);
    
    // Finalize metrics
    Metrics.Finish();
    
    if (bSuccess)
    {
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNDFSPlanner: Plan generation successful with %d tasks"), ResultPlan.Tasks.Num());
            Metrics.AppendDebugInfo(FString::Printf(TEXT("Plan generation successful with %d tasks"), ResultPlan.Tasks.Num()), Configuration.bDetailedDebugging);
            Metrics.AppendDebugInfo(TEXT("Final plan:"), Configuration.bDetailedDebugging);
            Metrics.AppendDebugInfo(ResultPlan.ToString(), Configuration.bDetailedDebugging);
        }
        
        return CreatePlannerResult(true, ResultPlan, EHTNPlannerFailReason::None);
    }
    else
    {
        EHTNPlannerFailReason FailReason = EHTNPlannerFailReason::NoValidPlan;
        
        if (ShouldAbortPlanning(0))
        {
            if (FPlatformTime::Seconds() - Metrics.StartTime >= Configuration.PlanningTimeout)
            {
                FailReason = EHTNPlannerFailReason::Timeout;
            }
            else
            {
                FailReason = EHTNPlannerFailReason::MaxDepthReached;
            }
        }
        
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: Plan generation failed: %s"), 
                   *StaticEnum<EHTNPlannerFailReason>()->GetNameStringByValue(static_cast<int64>(FailReason)));
            Metrics.AppendDebugInfo(FString::Printf(TEXT("Plan generation failed: %s"), 
                   *StaticEnum<EHTNPlannerFailReason>()->GetNameStringByValue(static_cast<int64>(FailReason))), Configuration.bDetailedDebugging);
        }
        
        return CreatePlannerResult(false, ResultPlan, FailReason);
    }
}

bool UHTNDFSPlanner::ValidatePlan(
    const FHTNPlan& Plan,
    const UHTNWorldState* WorldState)
{
    // Empty plans are considered valid
    if (Plan.IsEmpty())
    {
        return true;
    }
    
    // Ensure world state is valid
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Invalid world state provided for plan validation"));
        return false;
    }
    
    // Create a working copy of the world state
    UHTNWorldState* WorkingState = WorldState->Clone();
    
    // Check each task in sequence
    for (UHTNPrimitiveTask* Task : Plan.Tasks)
    {
        if (!Task)
        {
            UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Plan contains null task"));
            return false;
        }
        
        // Check if the task is applicable in the current world state
        if (!Task->IsApplicable(WorkingState))
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Task %s is not applicable in current world state during validation"), 
                   *Task->ToString());
            return false;
        }
        
        // Apply the task's effects to update the world state
        if (!ApplyTaskEffects(WorkingState, Task))
        {
            UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Failed to apply effects for task %s during validation"), 
                   *Task->ToString());
            return false;
        }
    }
    
    return true;
}

FHTNPlannerResult UHTNDFSPlanner::GeneratePartialPlan(
    const FHTNPlan& ExistingPlan,
    const UHTNWorldState* WorldState,
    const TArray<UHTNTask*>& GoalTasks,
    const FHTNPlanningConfig& Config)
{
    // Start with the existing plan
    FHTNPlan ResultPlan = ExistingPlan;
    
    // Validate inputs
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Invalid world state provided for partial planning"));
        return CreatePlannerResult(false, ResultPlan, EHTNPlannerFailReason::UnexpectedError);
    }
    
    if (GoalTasks.Num() == 0)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: No goal tasks provided for partial planning"));
        return CreatePlannerResult(true, ResultPlan, EHTNPlannerFailReason::None);
    }
    
    // Set up configuration and metrics
    Configuration = Config;
    Metrics.Reset();
    
    if (Configuration.bDetailedDebugging)
    {
        UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNDFSPlanner: Starting partial plan generation from existing plan with %d tasks"), 
               ExistingPlan.Tasks.Num());
        Metrics.AppendDebugInfo(FString::Printf(TEXT("Starting partial plan generation from existing plan with %d tasks"), 
               ExistingPlan.Tasks.Num()), Configuration.bDetailedDebugging);
    }
    
    // Clone the world state to avoid modifying the original
    UHTNWorldState* WorkingState = WorldState->Clone();
    
    // Apply the effects of all tasks in the existing plan to get the updated world state
    for (UHTNPrimitiveTask* Task : ExistingPlan.Tasks)
    {
        if (Task)
        {
            ApplyTaskEffects(WorkingState, Task);
        }
    }
    
    // Set up current plan with existing tasks
    TArray<UHTNPrimitiveTask*> CurrentPlan = ExistingPlan.Tasks;
    
    // Start recursive search to extend the plan
    bool bSuccess = FindPlanDFS(WorkingState, GoalTasks, CurrentPlan, 0, ResultPlan);
    
    // Finalize metrics
    Metrics.Finish();
    
    if (bSuccess)
    {
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNDFSPlanner: Partial plan generation successful with %d total tasks"), 
                   ResultPlan.Tasks.Num());
            Metrics.AppendDebugInfo(FString::Printf(TEXT("Partial plan generation successful with %d total tasks"), 
                   ResultPlan.Tasks.Num()), Configuration.bDetailedDebugging);
        }
        
        return CreatePlannerResult(true, ResultPlan, EHTNPlannerFailReason::None);
    }
    else
    {
        EHTNPlannerFailReason FailReason = EHTNPlannerFailReason::NoValidPlan;
        
        if (ShouldAbortPlanning(0))
        {
            if (FPlatformTime::Seconds() - Metrics.StartTime >= Configuration.PlanningTimeout)
            {
                FailReason = EHTNPlannerFailReason::Timeout;
            }
            else
            {
                FailReason = EHTNPlannerFailReason::MaxDepthReached;
            }
        }
        
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: Partial plan generation failed: %s"), 
                   *StaticEnum<EHTNPlannerFailReason>()->GetNameStringByValue(static_cast<int64>(FailReason)));
            Metrics.AppendDebugInfo(FString::Printf(TEXT("Partial plan generation failed: %s"), 
                   *StaticEnum<EHTNPlannerFailReason>()->GetNameStringByValue(static_cast<int64>(FailReason))), Configuration.bDetailedDebugging);
        }
        
        return CreatePlannerResult(false, ResultPlan, FailReason);
    }
}

void UHTNDFSPlanner::ConfigurePlanner(const FHTNPlanningConfig& NewConfig)
{
    Configuration = NewConfig;
    
    if (Configuration.bDetailedDebugging)
    {
        UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNDFSPlanner: Planner configured with MaxSearchDepth=%d, Timeout=%.2fs"), 
               Configuration.MaxSearchDepth, Configuration.PlanningTimeout);
    }
}

bool UHTNDFSPlanner::FindPlanDFS(
    const UHTNWorldState* WorldState,
    const TArray<UHTNTask*>& RemainingTasks,
    const TArray<UHTNPrimitiveTask*>& CurrentPlan,
    int32 CurrentDepth,
    FHTNPlan& OutPlan)
{
    // Check for timeout or max depth
    if (ShouldAbortPlanning(CurrentDepth))
    {
        return false;
    }
    
    // Update metrics
    Metrics.NodesExplored++;
    Metrics.MaxDepthReached = FMath::Max(Metrics.MaxDepthReached, CurrentDepth);
    
    // If there are no more tasks to process, we've found a valid plan
    if (RemainingTasks.Num() == 0)
    {
        Metrics.PlansGenerated++;
        OutPlan = FHTNPlan(CurrentPlan);
        
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Found valid plan with %d tasks"), OutPlan.Tasks.Num());
            Metrics.AppendDebugInfo(FString::Printf(TEXT("Found valid plan with %d tasks at depth %d"), OutPlan.Tasks.Num(), CurrentDepth), Configuration.bDetailedDebugging);
        }
        
        return true;
    }
    
    // Process the first task in the list
    UHTNTask* CurrentTask = RemainingTasks[0];
    
    // Create remaining tasks list (everything except the current task)
    TArray<UHTNTask*> NewRemainingTasks = RemainingTasks;
    NewRemainingTasks.RemoveAt(0);
    
    // Process the current task
    return ProcessTask(WorldState, CurrentTask, NewRemainingTasks, CurrentPlan, CurrentDepth, OutPlan);
}

bool UHTNDFSPlanner::ProcessTask(
    const UHTNWorldState* WorldState,
    UHTNTask* Task,
    const TArray<UHTNTask*>& RemainingTasks,
    const TArray<UHTNPrimitiveTask*>& CurrentPlan,
    int32 CurrentDepth,
    FHTNPlan& OutPlan)
{
    if (!Task)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Null task encountered during planning"));
        return false;
    }
    
    if (Configuration.bDetailedDebugging)
    {
        UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Processing task %s at depth %d"), *Task->ToString(), CurrentDepth);
        Metrics.AppendDebugInfo(FString::Printf(TEXT("Processing task %s at depth %d"), *Task->ToString(), CurrentDepth), Configuration.bDetailedDebugging);
    }
    
    // Check if the task is applicable in the current world state
    if (!Task->IsApplicable(WorldState))
    {
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Task %s is not applicable"), *Task->ToString());
            Metrics.AppendDebugInfo(FString::Printf(TEXT("Task %s is not applicable in current world state"), *Task->ToString()), Configuration.bDetailedDebugging);
        }
        
        return false;
    }
    
    // Handle primitive tasks
    if (UHTNPrimitiveTask* PrimitiveTask = Cast<UHTNPrimitiveTask>(Task))
    {
        // Clone the world state to avoid modifying the original
        UHTNWorldState* NewWorldState = WorldState->Clone();
        
        // Apply the primitive task's effects to the world state
        if (!ApplyTaskEffects(NewWorldState, PrimitiveTask))
        {
            return false;
        }
        
        // Add the primitive task to the plan
        TArray<UHTNPrimitiveTask*> NewPlan = CurrentPlan;
        NewPlan.Add(PrimitiveTask);
        
        // Continue planning with the next task
        return FindPlanDFS(NewWorldState, RemainingTasks, NewPlan, CurrentDepth + 1, OutPlan);
    }
    // Handle compound tasks
    else if (UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(Task))
    {
        // Get all available decomposition methods
        TArray<UHTNMethod*> AvailableMethods;
        if (!CompoundTask->GetAvailableMethods(WorldState, AvailableMethods) || AvailableMethods.Num() == 0)
        {
            if (Configuration.bDetailedDebugging)
            {
                UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: No applicable methods for compound task %s"), 
                       *CompoundTask->ToString());
                Metrics.AppendDebugInfo(FString::Printf(TEXT("No applicable methods for compound task %s"), 
                       *CompoundTask->ToString()), Configuration.bDetailedDebugging);
            }
            
            return false;
        }
        
        // Try each method in order of priority (already sorted by GetAvailableMethods)
        for (UHTNMethod* Method : AvailableMethods)
        {
            if (Configuration.bDetailedDebugging)
            {
                UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Trying method %s for task %s"), 
                       *Method->GetDescription(), *CompoundTask->ToString());
                Metrics.AppendDebugInfo(FString::Printf(TEXT("Trying method %s for task %s"), 
                       *Method->GetDescription(), *CompoundTask->ToString()), Configuration.bDetailedDebugging);
            }
            
            // Apply the method to get subtasks
            TArray<UHTNTask*> Subtasks;
            if (!CompoundTask->ApplyMethod(Method, WorldState, Subtasks))
            {
                if (Configuration.bDetailedDebugging)
                {
                    UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Failed to apply method %s"), 
                           *Method->GetDescription());
                    Metrics.AppendDebugInfo(FString::Printf(TEXT("Failed to apply method %s"), 
                           *Method->GetDescription()), Configuration.bDetailedDebugging);
                }
                
                continue;
            }
            
            // Create a new task list with the subtasks followed by the remaining tasks
            TArray<UHTNTask*> NewTaskList = Subtasks;
            NewTaskList.Append(RemainingTasks);
            
            // Recursively plan with the new task list
            if (FindPlanDFS(WorldState, NewTaskList, CurrentPlan, CurrentDepth + 1, OutPlan))
            {
                return true;
            }
        }
        
        // If we've tried all methods and none worked, this branch fails
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: All methods failed for compound task %s"), 
                   *CompoundTask->ToString());
            Metrics.AppendDebugInfo(FString::Printf(TEXT("All methods failed for compound task %s"), 
                   *CompoundTask->ToString()), Configuration.bDetailedDebugging);
        }
        
        return false;
    }
    else
    {
        // Unknown task type
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Unknown task type: %s"), *Task->GetClass()->GetName());
        return false;
    }
}

bool UHTNDFSPlanner::ApplyTaskEffects(
    UHTNWorldState* WorldState,
    UHTNPrimitiveTask* Task)
{
    if (!Task)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Null task provided for applying effects"));
        return false;
    }
    
    if (Configuration.bDetailedDebugging)
    {
        UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("HTNDFSPlanner: Applying effects for task %s"), *Task->ToString());
        Metrics.AppendDebugInfo(FString::Printf(TEXT("Applying effects for task %s"), *Task->ToString()), Configuration.bDetailedDebugging);
    }
    
    // Get the task's expected effects
    UHTNWorldState* Effects = Task->GetExpectedEffects(WorldState);
    
    if (!Effects)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("HTNDFSPlanner: Failed to get expected effects for task %s"), *Task->ToString());
        return false;
    }
    
    // For each property in the effects, apply it to the world state
    TArray<FName> EffectPropertyNames = Effects->GetPropertyNames();
    for (const FName& PropertyName : EffectPropertyNames)
    {
        FHTNProperty PropertyValue;
        if (Effects->GetProperty(PropertyName, PropertyValue))
        {
            WorldState->SetProperty(PropertyName, PropertyValue);
        }
    }
    
    return true;
}

bool UHTNDFSPlanner::ShouldAbortPlanning(int32 CurrentDepth) const
{
    // Check for timeout
    if (Configuration.PlanningTimeout > 0.0f)
    {
        float ElapsedTime = FPlatformTime::Seconds() - Metrics.StartTime;
        if (ElapsedTime >= Configuration.PlanningTimeout)
        {
            if (Configuration.bDetailedDebugging)
            {
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: Planning timeout reached (%.2fs)"), ElapsedTime);
            }
            
            return true;
        }
    }
    
    // Check for maximum depth
    if (CurrentDepth >= Configuration.MaxSearchDepth)
    {
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: Maximum search depth reached (%d)"), CurrentDepth);
        }
        
        return true;
    }
    
    // Check for maximum plans
    if (Configuration.MaxPlansToConsider > 0 && Metrics.PlansGenerated >= Configuration.MaxPlansToConsider)
    {
        if (Configuration.bDetailedDebugging)
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("HTNDFSPlanner: Maximum number of plans reached (%d)"), Metrics.PlansGenerated);
        }
        
        return true;
    }
    
    return false;
}

FHTNPlannerResult UHTNDFSPlanner::CreatePlannerResult(bool Success, const FHTNPlan& Plan, EHTNPlannerFailReason FailReason)
{
    FHTNPlannerResult Result;
    Result.bSuccess = Success;
    Result.Plan = Plan;
    Result.FailReason = FailReason;
    Result.NodesExplored = Metrics.NodesExplored;
    Result.PlansGenerated = Metrics.PlansGenerated;
    Result.MaxDepthReached = Metrics.MaxDepthReached;
    Result.PlanningTime = Metrics.GetElapsedTime();
    Result.DebugInfo = Metrics.DebugInfo;
    
    return Result;
}