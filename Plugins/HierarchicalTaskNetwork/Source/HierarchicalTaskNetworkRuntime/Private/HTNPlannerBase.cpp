// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNPlannerBase.h"
#include "HTNLogging.h"
#include "Serialization/JsonSerializer.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonReader.h"

//////////////////////////////////////////////////////////////////////////
// FHTNPlan Implementation
//////////////////////////////////////////////////////////////////////////

FHTNPlan::FHTNPlan()
    : TotalCost(0.0f)
    , CurrentTaskIndex(0)
    , bIsExecuting(false)
    , bIsComplete(false)
    , bFailed(false)
    , StartTime(0.0f)
    , EndTime(0.0f)
{
}

FHTNPlan::FHTNPlan(const TArray<UHTNPrimitiveTask*>& InTasks, float InTotalCost)
    : Tasks(InTasks)
    , TotalCost(InTotalCost)
    , CurrentTaskIndex(0)
    , bIsExecuting(false)
    , bIsComplete(false)
    , bFailed(false)
    , StartTime(0.0f)
    , EndTime(0.0f)
{
    // Calculate total cost if not provided
    if (InTotalCost == 0.0f)
    {
        for (const UHTNPrimitiveTask* Task : Tasks)
        {
            if (Task)
            {
                TotalCost += Task->GetCost();
            }
        }
    }
}

FHTNPlan::FHTNPlan(const FHTNPlan& Other)
    : Tasks(Other.Tasks)
    , TotalCost(Other.TotalCost)
    , CurrentTaskIndex(Other.CurrentTaskIndex)
    , bIsExecuting(Other.bIsExecuting)
    , bIsComplete(Other.bIsComplete)
    , bFailed(Other.bFailed)
    , StartTime(Other.StartTime)
    , EndTime(Other.EndTime)
{
}

FHTNPlan::FHTNPlan(FHTNPlan&& Other) noexcept
    : Tasks(MoveTemp(Other.Tasks))
    , TotalCost(Other.TotalCost)
    , CurrentTaskIndex(Other.CurrentTaskIndex)
    , bIsExecuting(Other.bIsExecuting)
    , bIsComplete(Other.bIsComplete)
    , bFailed(Other.bFailed)
    , StartTime(Other.StartTime)
    , EndTime(Other.EndTime)
{
    // Reset the moved-from object to a valid state
    Other.TotalCost = 0.0f;
    Other.CurrentTaskIndex = 0;
    Other.bIsExecuting = false;
    Other.bIsComplete = false;
    Other.bFailed = false;
    Other.StartTime = 0.0f;
    Other.EndTime = 0.0f;
}

FHTNPlan& FHTNPlan::operator=(const FHTNPlan& Other)
{
    if (this != &Other)
    {
        Tasks = Other.Tasks;
        TotalCost = Other.TotalCost;
        CurrentTaskIndex = Other.CurrentTaskIndex;
        bIsExecuting = Other.bIsExecuting;
        bIsComplete = Other.bIsComplete;
        bFailed = Other.bFailed;
        StartTime = Other.StartTime;
        EndTime = Other.EndTime;
    }
    return *this;
}

FHTNPlan& FHTNPlan::operator=(FHTNPlan&& Other) noexcept
{
    if (this != &Other)
    {
        Tasks = MoveTemp(Other.Tasks);
        TotalCost = Other.TotalCost;
        CurrentTaskIndex = Other.CurrentTaskIndex;
        bIsExecuting = Other.bIsExecuting;
        bIsComplete = Other.bIsComplete;
        bFailed = Other.bFailed;
        StartTime = Other.StartTime;
        EndTime = Other.EndTime;
        
        // Reset the moved-from object to a valid state
        Other.TotalCost = 0.0f;
        Other.CurrentTaskIndex = 0;
        Other.bIsExecuting = false;
        Other.bIsComplete = false;
        Other.bFailed = false;
        Other.StartTime = 0.0f;
        Other.EndTime = 0.0f;
    }
    return *this;
}

bool FHTNPlan::operator<(const FHTNPlan& Other) const
{
    // Lower cost plans are considered "better" (come first in priority queues)
    return TotalCost < Other.TotalCost;
}

bool FHTNPlan::operator==(const FHTNPlan& Other) const
{
    // Compare task pointers
    if (Tasks.Num() != Other.Tasks.Num())
    {
        return false;
    }
    
    for (int32 i = 0; i < Tasks.Num(); ++i)
    {
        if (Tasks[i] != Other.Tasks[i])
        {
            return false;
        }
    }
    
    return true;
}

bool FHTNPlan::operator!=(const FHTNPlan& Other) const
{
    return !(*this == Other);
}

FString FHTNPlan::ToString() const
{
    FString Result = FString::Printf(TEXT("HTN Plan (Cost: %.2f, Tasks: %d)\n"), TotalCost, Tasks.Num());
    
    // Add execution info if relevant
    if (bIsExecuting)
    {
        Result += FString::Printf(TEXT("Status: Executing (Task %d/%d)\n"), CurrentTaskIndex + 1, Tasks.Num());
    }
    else if (bIsComplete)
    {
        Result += TEXT("Status: Complete\n");
    }
    else if (bFailed)
    {
        Result += TEXT("Status: Failed\n");
    }
    else
    {
        Result += TEXT("Status: Not Started\n");
    }
    
    // Add task list
    Result += TEXT("Tasks:\n");
    for (int32 i = 0; i < Tasks.Num(); ++i)
    {
        const UHTNPrimitiveTask* Task = Tasks[i];
        FString TaskStatus;
        
        if (i < CurrentTaskIndex)
        {
            TaskStatus = TEXT("[✓] ");
        }
        else if (i == CurrentTaskIndex && bIsExecuting)
        {
            TaskStatus = TEXT("[►] ");
        }
        else
        {
            TaskStatus = TEXT("[ ] ");
        }
        
        Result += FString::Printf(TEXT("%s%d: %s (Cost: %.2f)\n"), 
            *TaskStatus, i + 1, Task ? *Task->ToString() : TEXT("NULL"), Task ? Task->GetCost() : 0.0f);
    }
    
    return Result;
}

void FHTNPlan::Clear()
{
    Tasks.Empty();
    TotalCost = 0.0f;
    CurrentTaskIndex = 0;
    bIsExecuting = false;
    bIsComplete = false;
    bFailed = false;
    StartTime = 0.0f;
    EndTime = 0.0f;
}

bool FHTNPlan::IsEmpty() const
{
    return Tasks.Num() == 0;
}

int32 FHTNPlan::GetLength() const
{
    return Tasks.Num();
}

UHTNPrimitiveTask* FHTNPlan::GetTask(int32 Index) const
{
    if (Tasks.IsValidIndex(Index))
    {
        return Tasks[Index];
    }
    return nullptr;
}

UHTNPrimitiveTask* FHTNPlan::GetCurrentTask() const
{
    if (bIsExecuting && !bIsComplete && !bFailed && Tasks.IsValidIndex(CurrentTaskIndex))
    {
        return Tasks[CurrentTaskIndex];
    }
    return nullptr;
}

void FHTNPlan::AddTask(UHTNPrimitiveTask* Task)
{
    if (Task)
    {
        Tasks.Add(Task);
        TotalCost += Task->GetCost();
    }
}

bool FHTNPlan::IsValid() const
{
    if (Tasks.Num() == 0)
    {
        return false;
    }
    
    for (const UHTNPrimitiveTask* Task : Tasks)
    {
        if (!Task)
        {
            return false;
        }
    }
    
    return true;
}

FString FHTNPlan::ToJson() const
{
    TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
    
    // Add plan metadata
    JsonObject->SetNumberField(TEXT("TotalCost"), TotalCost);
    JsonObject->SetNumberField(TEXT("CurrentTaskIndex"), CurrentTaskIndex);
    JsonObject->SetBoolField(TEXT("IsExecuting"), bIsExecuting);
    JsonObject->SetBoolField(TEXT("IsComplete"), bIsComplete);
    JsonObject->SetBoolField(TEXT("Failed"), bFailed);
    JsonObject->SetNumberField(TEXT("StartTime"), StartTime);
    JsonObject->SetNumberField(TEXT("EndTime"), EndTime);
    
    // Add tasks array
    TArray<TSharedPtr<FJsonValue>> TasksArray;
    for (const UHTNPrimitiveTask* Task : Tasks)
    {
        TSharedPtr<FJsonObject> TaskObject = MakeShared<FJsonObject>();
        if (Task)
        {
            TaskObject->SetStringField(TEXT("Class"), Task->GetClass()->GetName());
            TaskObject->SetStringField(TEXT("Name"), Task->TaskName.ToString());
            TaskObject->SetStringField(TEXT("ID"), Task->GetTaskID().ToString());
            TaskObject->SetNumberField(TEXT("Cost"), Task->GetCost());
        }
        else
        {
            TaskObject->SetStringField(TEXT("Class"), TEXT("NULL"));
        }
        
        TasksArray.Add(MakeShared<FJsonValueObject>(TaskObject));
    }
    JsonObject->SetArrayField(TEXT("Tasks"), TasksArray);
    
    // Convert to string
    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    
    return OutputString;
}

bool FHTNPlan::FromJson(const FString& JsonString)
{
    TSharedPtr<FJsonObject> JsonObject;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
    if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("Failed to parse plan JSON: %s"), *JsonString);
        return false;
    }
    
    // Clear existing data
    Clear();
    
    // Read plan metadata
    TotalCost = JsonObject->GetNumberField(TEXT("TotalCost"));
    CurrentTaskIndex = JsonObject->GetIntegerField(TEXT("CurrentTaskIndex"));
    bIsExecuting = JsonObject->GetBoolField(TEXT("IsExecuting"));
    bIsComplete = JsonObject->GetBoolField(TEXT("IsComplete"));
    bFailed = JsonObject->GetBoolField(TEXT("Failed"));
    StartTime = JsonObject->GetNumberField(TEXT("StartTime"));
    EndTime = JsonObject->GetNumberField(TEXT("EndTime"));
    
    // Note: Deserializing actual task objects would require more context
    // such as a task registry to look up classes by name. This implementation
    // just sets up the metadata but leaves the Tasks array empty.
    // A more complete implementation would recreate the task objects.
    
    UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Plan deserialized from JSON, but task objects were not recreated."));
    
    return true;
}

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