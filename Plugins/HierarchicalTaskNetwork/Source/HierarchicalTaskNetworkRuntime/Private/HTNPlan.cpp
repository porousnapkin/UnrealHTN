#include "HTNPlan.h"

#include "HTNLogging.h"
#include "HTNPrimitiveTask.h"

FHTNPlan::FHTNPlan()
    : TotalCost(0.0f)
    , CurrentTaskIndex(0)
    , bIsExecuting(false)
    , bIsComplete(false)
    , bFailed(false)
    , bIsPaused(false)
    , StartTime(0.0f)
    , EndTime(0.0f)
    , Status(EHTNPlanStatus::NotStarted)
{
}

FHTNPlan::FHTNPlan(const TArray<UHTNPrimitiveTask*>& InTasks, float InTotalCost)
    : Tasks(InTasks)
    , TotalCost(InTotalCost)
    , CurrentTaskIndex(0)
    , bIsExecuting(false)
    , bIsComplete(false)
    , bFailed(false)
    , bIsPaused(false)
    , StartTime(0.0f)
    , EndTime(0.0f)
    , Status(EHTNPlanStatus::NotStarted)
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
    , bIsPaused(Other.bIsPaused)
    , StartTime(Other.StartTime)
    , EndTime(Other.EndTime)
    , Status(Other.Status)
    , TaskParameters(Other.TaskParameters)
    , TaskResults(Other.TaskResults)
    , TaskDependencies(Other.TaskDependencies)
{
}

FHTNPlan::FHTNPlan(FHTNPlan&& Other) noexcept
    : Tasks(MoveTemp(Other.Tasks))
    , TotalCost(Other.TotalCost)
    , CurrentTaskIndex(Other.CurrentTaskIndex)
    , bIsExecuting(Other.bIsExecuting)
    , bIsComplete(Other.bIsComplete)
    , bFailed(Other.bFailed)
    , bIsPaused(Other.bIsPaused)
    , StartTime(Other.StartTime)
    , EndTime(Other.EndTime)
    , Status(Other.Status)
    , TaskParameters(MoveTemp(Other.TaskParameters))
    , TaskResults(MoveTemp(Other.TaskResults))
    , TaskDependencies(MoveTemp(Other.TaskDependencies))
{
    // Reset the moved-from object to a valid state
    Other.TotalCost = 0.0f;
    Other.CurrentTaskIndex = 0;
    Other.bIsExecuting = false;
    Other.bIsComplete = false;
    Other.bFailed = false;
    Other.bIsPaused = false;
    Other.StartTime = 0.0f;
    Other.EndTime = 0.0f;
    Other.Status = EHTNPlanStatus::NotStarted;
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
        bIsPaused = Other.bIsPaused;
        StartTime = Other.StartTime;
        EndTime = Other.EndTime;
        Status = Other.Status;
        TaskParameters = Other.TaskParameters;
        TaskResults = Other.TaskResults;
        TaskDependencies = Other.TaskDependencies;
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
        bIsPaused = Other.bIsPaused;
        StartTime = Other.StartTime;
        EndTime = Other.EndTime;
        Status = Other.Status;
        TaskParameters = MoveTemp(Other.TaskParameters);
        TaskResults = MoveTemp(Other.TaskResults);
        TaskDependencies = MoveTemp(Other.TaskDependencies);
        
        // Reset the moved-from object to a valid state
        Other.TotalCost = 0.0f;
        Other.CurrentTaskIndex = 0;
        Other.bIsExecuting = false;
        Other.bIsComplete = false;
        Other.bFailed = false;
        Other.bIsPaused = false;
        Other.StartTime = 0.0f;
        Other.EndTime = 0.0f;
        Other.Status = EHTNPlanStatus::NotStarted;
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

void FHTNPlan::Clear()
{
    Tasks.Empty();
    TotalCost = 0.0f;
    CurrentTaskIndex = 0;
    bIsExecuting = false;
    bIsComplete = false;
    bFailed = false;
    bIsPaused = false;
    StartTime = 0.0f;
    EndTime = 0.0f;
    Status = EHTNPlanStatus::NotStarted;
    TaskParameters.Empty();
    TaskResults.Empty();
    TaskDependencies.Empty();
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

TArray<UHTNPrimitiveTask*> FHTNPlan::GetRemainingTasks() const
{
    TArray<UHTNPrimitiveTask*> RemainingTasks;
    
    if (CurrentTaskIndex < Tasks.Num())
    {
        for (int32 i = CurrentTaskIndex; i < Tasks.Num(); ++i)
        {
            RemainingTasks.Add(Tasks[i]);
        }
    }
    
    return RemainingTasks;
}

TArray<UHTNPrimitiveTask*> FHTNPlan::GetExecutedTasks() const
{
    TArray<UHTNPrimitiveTask*> ExecutedTasks;
    
    for (int32 i = 0; i < FMath::Min(CurrentTaskIndex, Tasks.Num()); ++i)
    {
        ExecutedTasks.Add(Tasks[i]);
    }
    
    return ExecutedTasks;
}

FString FHTNPlan::ToString() const
{
    FString Result = FString::Printf(TEXT("HTN Plan (Cost: %.2f, Tasks: %d)\n"), TotalCost, Tasks.Num());
    
    // Add execution info if relevant
    FString StatusStr;
    switch (Status)
    {
        case EHTNPlanStatus::NotStarted:
            StatusStr = TEXT("Not Started");
            break;
        case EHTNPlanStatus::Executing:
            StatusStr = FString::Printf(TEXT("Executing (Task %d/%d)"), CurrentTaskIndex + 1, Tasks.Num());
            break;
        case EHTNPlanStatus::Paused:
            StatusStr = FString::Printf(TEXT("Paused (Task %d/%d)"), CurrentTaskIndex + 1, Tasks.Num());
            break;
        case EHTNPlanStatus::Completed:
            StatusStr = TEXT("Completed");
            break;
        case EHTNPlanStatus::Failed:
            StatusStr = TEXT("Failed");
            break;
        case EHTNPlanStatus::Aborted:
            StatusStr = TEXT("Aborted");
            break;
        default:
            StatusStr = TEXT("Unknown");
            break;
    }
    
    Result += FString::Printf(TEXT("Status: %s\n"), *StatusStr);
    
    if (bIsExecuting && !bIsComplete && !bFailed)
    {
        float ElapsedTime = FPlatformTime::Seconds() - StartTime;
        Result += FString::Printf(TEXT("Execution Time: %.2f seconds\n"), ElapsedTime);
    }
    else if (bIsComplete || bFailed)
    {
        Result += FString::Printf(TEXT("Total Execution Time: %.2f seconds\n"), EndTime - StartTime);
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
            if (bIsPaused)
            {
                TaskStatus = TEXT("[⏸] ");
            }
            else
            {
                TaskStatus = TEXT("[►] ");
            }
        }
        else
        {
            TaskStatus = TEXT("[ ] ");
        }
        
        Result += FString::Printf(TEXT("%s%d: %s (Cost: %.2f)\n"), 
            *TaskStatus, i + 1, Task ? *Task->ToString() : TEXT("NULL"), Task ? Task->GetCost() : 0.0f);
        
        // Add task parameters if any
        for (const auto& Pair : TaskParameters)
        {
            FString KeyStr = Pair.Key.ToString();
            FString TaskIndexPrefix = FString::Printf(TEXT("Task_%d_"), i);
            if (KeyStr.StartsWith(TaskIndexPrefix))
            {
                FString ParamName = KeyStr.RightChop(TaskIndexPrefix.Len());
                Result += FString::Printf(TEXT("   - Param %s: %s\n"), *ParamName, *Pair.Value.ToString());
            }
        }
        
        // Add task results if any
        for (const auto& Pair : TaskResults)
        {
            FString KeyStr = Pair.Key.ToString();
            FString TaskIndexPrefix = FString::Printf(TEXT("Task_%d_"), i);
            if (KeyStr.StartsWith(TaskIndexPrefix))
            {
                FString ResultName = KeyStr.RightChop(TaskIndexPrefix.Len());
                Result += FString::Printf(TEXT("   - Result %s: %s\n"), *ResultName, *Pair.Value.ToString());
            }
        }
    }
    
    // Add dependencies if any
    if (TaskDependencies.Num() > 0)
    {
        Result += TEXT("\nTask Dependencies:\n");
        for (const auto& Pair : TaskDependencies)
        {
            if (Pair.Value.Num() > 0)
            {
                FString DependsOn;
                for (int32 DepIndex : Pair.Value)
                {
                    if (!DependsOn.IsEmpty())
                    {
                        DependsOn += TEXT(", ");
                    }
                    DependsOn += FString::FromInt(DepIndex + 1);
                }
                Result += FString::Printf(TEXT("Task %d depends on: %s\n"), Pair.Key + 1, *DependsOn);
            }
        }
    }
    
    return Result;
}

bool FHTNPlan::SetTaskParameter(int32 TaskIndex, FName ParamName, const FHTNProperty& ParamValue)
{
    if (!Tasks.IsValidIndex(TaskIndex))
    {
        return false;
    }
    
    FName QualifiedName = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex, *ParamName.ToString()));
    TaskParameters.Add(QualifiedName, ParamValue);
    
    return true;
}

bool FHTNPlan::GetTaskParameter(int32 TaskIndex, FName ParamName, FHTNProperty& OutValue) const
{
    if (!Tasks.IsValidIndex(TaskIndex))
    {
        return false;
    }
    
    FName QualifiedName = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex, *ParamName.ToString()));
    const FHTNProperty* FoundValue = TaskParameters.Find(QualifiedName);
    
    if (FoundValue)
    {
        OutValue = *FoundValue;
        return true;
    }
    
    return false;
}

bool FHTNPlan::SetTaskResult(int32 TaskIndex, FName ResultName, const FHTNProperty& ResultValue)
{
    if (!Tasks.IsValidIndex(TaskIndex))
    {
        return false;
    }
    
    FName QualifiedName = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex, *ResultName.ToString()));
    TaskResults.Add(QualifiedName, ResultValue);
    
    return true;
}

bool FHTNPlan::GetTaskResult(int32 TaskIndex, FName ResultName, FHTNProperty& OutValue) const
{
    if (!Tasks.IsValidIndex(TaskIndex))
    {
        return false;
    }
    
    FName QualifiedName = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex, *ResultName.ToString()));
    const FHTNProperty* FoundValue = TaskResults.Find(QualifiedName);
    
    if (FoundValue)
    {
        OutValue = *FoundValue;
        return true;
    }
    
    return false;
}

bool FHTNPlan::AddTaskDependency(int32 TaskIndex, int32 DependsOnTaskIndex)
{
    if (!Tasks.IsValidIndex(TaskIndex) || !Tasks.IsValidIndex(DependsOnTaskIndex))
    {
        return false;
    }
    
    if (TaskIndex == DependsOnTaskIndex)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Cannot add self-dependency for task %d"), TaskIndex);
        return false;
    }
    
    // Check for circular dependencies
    TArray<int32> VisitedIndices;
    VisitedIndices.Add(TaskIndex);
    
    TArray<int32> Queue;
    Queue.Add(DependsOnTaskIndex);
    
    while (Queue.Num() > 0)
    {
        int32 CurrentIndex = Queue[0];
        Queue.RemoveAt(0);
        
        if (VisitedIndices.Contains(CurrentIndex))
        {
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Adding dependency would create a circular reference"));
            return false;
        }
        
        VisitedIndices.Add(CurrentIndex);
        
        TArray<int32>* Dependencies = TaskDependencies.Find(CurrentIndex);
        if (Dependencies)
        {
            Queue.Append(*Dependencies);
        }
    }
    
    // Add the dependency
    TArray<int32>& Dependencies = TaskDependencies.FindOrAdd(TaskIndex);
    if (!Dependencies.Contains(DependsOnTaskIndex))
    {
        Dependencies.Add(DependsOnTaskIndex);
        return true;
    }
    
    return false;
}

bool FHTNPlan::AreTaskDependenciesSatisfied(int32 TaskIndex) const
{
    if (!Tasks.IsValidIndex(TaskIndex))
    {
        return false;
    }
    
    const TArray<int32>* Dependencies = TaskDependencies.Find(TaskIndex);
    if (!Dependencies || Dependencies->Num() == 0)
    {
        // No dependencies to satisfy
        return true;
    }
    
    // Check if all dependency tasks have been completed
    for (int32 DepIndex : *Dependencies)
    {
        if (!Tasks.IsValidIndex(DepIndex) || DepIndex >= CurrentTaskIndex)
        {
            // Dependency task doesn't exist or hasn't been completed
            return false;
        }
    }
    
    return true;
}

TArray<int32> FHTNPlan::FindTasksByName(FName TaskName) const
{
    TArray<int32> FoundIndices;
    
    for (int32 i = 0; i < Tasks.Num(); ++i)
    {
        if (Tasks[i] && Tasks[i]->TaskName == TaskName)
        {
            FoundIndices.Add(i);
        }
    }
    
    return FoundIndices;
}

TArray<int32> FHTNPlan::FindTasksByClass(TSubclassOf<UHTNPrimitiveTask> TaskClass) const
{
    TArray<int32> FoundIndices;
    
    if (!TaskClass)
    {
        return FoundIndices;
    }
    
    for (int32 i = 0; i < Tasks.Num(); ++i)
    {
        if (Tasks[i] && Tasks[i]->IsA(TaskClass))
        {
            FoundIndices.Add(i);
        }
    }
    
    return FoundIndices;
}

bool FHTNPlan::MergeWith(const FHTNPlan& OtherPlan)
{
    if (!OtherPlan.IsValid())
    {
        return false;
    }
    
    // Store original tasks count for index adjustments
    int32 OriginalTaskCount = Tasks.Num();
    
    // Add tasks from other plan
    for (UHTNPrimitiveTask* Task : OtherPlan.Tasks)
    {
        AddTask(Task);
    }
    
    // Merge task parameters
    for (const auto& Pair : OtherPlan.TaskParameters)
    {
        // Adjust task indices in parameter names
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                FString ParamName = KeyStr.RightChop(UnderscorePos + 1);
                
                // Adjust index to account for existing tasks
                int32 NewTaskIndex = TaskIndex + OriginalTaskCount;
                
                // Create new parameter name
                FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), NewTaskIndex, *ParamName));
                TaskParameters.Add(NewKey, Pair.Value);
            }
        }
    }
    
    // Merge task results
    for (const auto& Pair : OtherPlan.TaskResults)
    {
        // Adjust task indices in result names
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                FString ResultName = KeyStr.RightChop(UnderscorePos + 1);
                
                // Adjust index to account for existing tasks
                int32 NewTaskIndex = TaskIndex + OriginalTaskCount;
                
                // Create new result name
                FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), NewTaskIndex, *ResultName));
                TaskResults.Add(NewKey, Pair.Value);
            }
        }
    }
    
    // Merge task dependencies
    for (const auto& Pair : OtherPlan.TaskDependencies)
    {
        int32 AdjustedTaskIndex = Pair.Key + OriginalTaskCount;
        
        TArray<int32> AdjustedDependencies;
        for (int32 DepIndex : Pair.Value)
        {
            AdjustedDependencies.Add(DepIndex + OriginalTaskCount);
        }
        
        TaskDependencies.Add(AdjustedTaskIndex, AdjustedDependencies);
    }
    
    return true;
}

FHTNPlan FHTNPlan::ExtractSubplan(int32 StartIndex, int32 EndIndex) const
{
    FHTNPlan Subplan;
    
    // Validate indices
    if (StartIndex < 0 || EndIndex >= Tasks.Num() || StartIndex > EndIndex)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Invalid indices for ExtractSubplan: %d to %d (plan size: %d)"), 
            StartIndex, EndIndex, Tasks.Num());
        return Subplan;
    }
    
    // Copy tasks
    for (int32 i = StartIndex; i <= EndIndex; ++i)
    {
        Subplan.AddTask(Tasks[i]);
    }
    
    // Copy relevant task parameters
    for (const auto& Pair : TaskParameters)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                
                if (TaskIndex >= StartIndex && TaskIndex <= EndIndex)
                {
                    // Adjust index for the subplan
                    FString ParamName = KeyStr.RightChop(UnderscorePos + 1);
                    FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex - StartIndex, *ParamName));
                    Subplan.TaskParameters.Add(NewKey, Pair.Value);
                }
            }
        }
    }
    
    // Copy relevant task results
    for (const auto& Pair : TaskResults)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                
                if (TaskIndex >= StartIndex && TaskIndex <= EndIndex)
                {
                    // Adjust index for the subplan
                    FString ResultName = KeyStr.RightChop(UnderscorePos + 1);
                    FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex - StartIndex, *ResultName));
                    Subplan.TaskResults.Add(NewKey, Pair.Value);
                }
            }
        }
    }
    
    // Copy and adjust relevant dependencies
    for (const auto& Pair : TaskDependencies)
    {
        if (Pair.Key >= StartIndex && Pair.Key <= EndIndex)
        {
            TArray<int32> AdjustedDependencies;
            
            for (int32 DepIndex : Pair.Value)
            {
                if (DepIndex >= StartIndex && DepIndex <= EndIndex)
                {
                    // Only include dependencies that are also in the subplan
                    AdjustedDependencies.Add(DepIndex - StartIndex);
                }
            }
            
            if (AdjustedDependencies.Num() > 0)
            {
                Subplan.TaskDependencies.Add(Pair.Key - StartIndex, AdjustedDependencies);
            }
        }
    }
    
    return Subplan;
}

bool FHTNPlan::ReplaceSection(int32 StartIndex, int32 EndIndex, const FHTNPlan& ReplacementPlan)
{
    // Validate indices
    if (StartIndex < 0 || EndIndex >= Tasks.Num() || StartIndex > EndIndex)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Invalid indices for ReplaceSection: %d to %d (plan size: %d)"), 
            StartIndex, EndIndex, Tasks.Num());
        return false;
    }
    
    if (!ReplacementPlan.IsValid())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Invalid replacement plan for ReplaceSection"));
        return false;
    }
    
    // Calculate the section size difference
    int32 OldSectionSize = EndIndex - StartIndex + 1;
    int32 NewSectionSize = ReplacementPlan.Tasks.Num();
    int32 SizeDifference = NewSectionSize - OldSectionSize;
    
    // Adjust total cost
    float OldSectionCost = 0.0f;
    for (int32 i = StartIndex; i <= EndIndex; ++i)
    {
        OldSectionCost += Tasks[i]->GetCost();
    }
    TotalCost = TotalCost - OldSectionCost + ReplacementPlan.TotalCost;
    
    // Replace tasks
    if (SizeDifference == 0)
    {
        // Simple replacement if the sizes match
        for (int32 i = 0; i < NewSectionSize; ++i)
        {
            Tasks[StartIndex + i] = ReplacementPlan.Tasks[i];
        }
    }
    else
    {
        // Remove old section
        Tasks.RemoveAt(StartIndex, OldSectionSize);
        
        // Insert new section
        for (int32 i = 0; i < NewSectionSize; ++i)
        {
            Tasks.Insert(ReplacementPlan.Tasks[i], StartIndex + i);
        }
    }
    
    // Remove parameters and results for the replaced section
    TArray<FName> KeysToRemove;
    
    for (const auto& Pair : TaskParameters)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                
                if (TaskIndex >= StartIndex && TaskIndex <= EndIndex)
                {
                    KeysToRemove.Add(Pair.Key);
                }
            }
        }
    }
    
    for (const FName& Key : KeysToRemove)
    {
        TaskParameters.Remove(Key);
    }
    
    KeysToRemove.Empty();
    
    for (const auto& Pair : TaskResults)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                
                if (TaskIndex >= StartIndex && TaskIndex <= EndIndex)
                {
                    KeysToRemove.Add(Pair.Key);
                }
            }
        }
    }
    
    for (const FName& Key : KeysToRemove)
    {
        TaskResults.Remove(Key);
    }
    
    // Add parameters and results from the replacement plan
    for (const auto& Pair : ReplacementPlan.TaskParameters)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                FString ParamName = KeyStr.RightChop(UnderscorePos + 1);
                
                // Adjust index for the main plan
                FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), TaskIndex + StartIndex, *ParamName));
                TaskParameters.Add(NewKey, Pair.Value);
            }
        }
    }

    // Restore task results and task parameters for non-modified tasks after replacement index
    for (const auto& Pair : TaskResults)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                
                if (TaskIndex > EndIndex)
                {
                    // Adjust index for tasks after the replacement section
                    int32 NewTaskIndex = TaskIndex + SizeDifference;
                    FString ResultName = KeyStr.RightChop(UnderscorePos + 1);
                    FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), NewTaskIndex, *ResultName));
                    TaskResults.Add(NewKey, Pair.Value);
                }
            }
        }
    }
    
    for (const auto& Pair : TaskParameters)
    {
        FString KeyStr = Pair.Key.ToString();
        if (KeyStr.StartsWith(TEXT("Task_")))
        {
            int32 UnderscorePos = KeyStr.Find(TEXT("_"), ESearchCase::CaseSensitive, ESearchDir::FromStart, 5);
            if (UnderscorePos != INDEX_NONE)
            {
                FString IndexStr = KeyStr.Mid(5, UnderscorePos - 5);
                int32 TaskIndex = FCString::Atoi(*IndexStr);
                
                if (TaskIndex > EndIndex)
                {
                    // Adjust index for tasks after the replacement section
                    int32 NewTaskIndex = TaskIndex + SizeDifference;
                    FString ParamName = KeyStr.RightChop(UnderscorePos + 1);
                    FName NewKey = FName(*FString::Printf(TEXT("Task_%d_%s"), NewTaskIndex, *ParamName));
                    TaskParameters.Add(NewKey, Pair.Value);
                }
            }
        }
    }
    
    // Update task dependencies
    TMap<int32, TArray<int32>> NewDependencies;
    
    // Add dependencies for tasks before the replaced section
    for (const auto& Pair : TaskDependencies)
    {
        if (Pair.Key < StartIndex)
        {
            TArray<int32> AdjustedDependencies;
            for (int32 DepIndex : Pair.Value)
            {
                if (DepIndex < StartIndex)
                {
                    // Dependency is before replaced section, no adjustment needed
                    AdjustedDependencies.Add(DepIndex);
                }
                else if (DepIndex > EndIndex)
                {
                    // Dependency is after replaced section, adjust index
                    AdjustedDependencies.Add(DepIndex + SizeDifference);
                }
                // Dependencies within the replaced section are lost
            }
            
            if (AdjustedDependencies.Num() > 0)
            {
                NewDependencies.Add(Pair.Key, AdjustedDependencies);
            }
        }
        else if (Pair.Key > EndIndex)
        {
            // Task is after replaced section, adjust its index
            int32 NewTaskIndex = Pair.Key + SizeDifference;
            
            TArray<int32> AdjustedDependencies;
            for (int32 DepIndex : Pair.Value)
            {
                if (DepIndex < StartIndex)
                {
                    // Dependency is before replaced section, no adjustment needed
                    AdjustedDependencies.Add(DepIndex);
                }
                else if (DepIndex > EndIndex)
                {
                    // Dependency is after replaced section, adjust index
                    AdjustedDependencies.Add(DepIndex + SizeDifference);
                }
                // Dependencies within the replaced section are lost
            }
            
            if (AdjustedDependencies.Num() > 0)
            {
                NewDependencies.Add(NewTaskIndex, AdjustedDependencies);
            }
        }
        // Dependencies for tasks within the replaced section are lost
    }
    
    // Add dependencies from the replacement plan
    for (const auto& Pair : ReplacementPlan.TaskDependencies)
    {
        int32 AdjustedTaskIndex = Pair.Key + StartIndex;
        
        TArray<int32> AdjustedDependencies;
        for (int32 DepIndex : Pair.Value)
        {
            AdjustedDependencies.Add(DepIndex + StartIndex);
        }
        
        if (AdjustedDependencies.Num() > 0)
        {
            NewDependencies.Add(AdjustedTaskIndex, AdjustedDependencies);
        }
    }
    
    // Replace the dependencies map
    TaskDependencies = NewDependencies;
    
    return true;
}

bool FHTNPlan::ToBinary(TArray<uint8>& OutData)
{
    // Create a memory writer to serialize the data
    FMemoryWriter MemWriter(OutData);
    
    // Write a version number for backward compatibility
    int32 SerializationVersion = 1;
    MemWriter << SerializationVersion;
    
    // Write basic plan data
    float TotalCostCopy = TotalCost;
    MemWriter << TotalCostCopy;

    int32 CurrentTaskIndexCopy = CurrentTaskIndex;
    MemWriter << CurrentTaskIndexCopy;

    // Convert bit fields to bool for serialization
    bool IsExecutingCopy = bIsExecuting;
    MemWriter << IsExecutingCopy;

    bool IsCompleteCopy = bIsComplete;
    MemWriter << IsCompleteCopy;

    bool FailedCopy = bFailed;
    MemWriter << FailedCopy;

    bool IsPausedCopy = bIsPaused;
    MemWriter << IsPausedCopy;

    float StartTimeCopy = StartTime;
    MemWriter << StartTimeCopy;

    float EndTimeCopy = EndTime;
    MemWriter << EndTimeCopy;
    
    // Write the status as an integer
    int32 StatusInt = static_cast<int32>(Status);
    MemWriter << StatusInt;
    
    // Write task references
    // Note: This only writes GUIDs, not the actual tasks
    // The tasks will need to be resolved separately
    int32 TaskCount = Tasks.Num();
    MemWriter << TaskCount;
    
    for (int32 i = 0; i < Tasks.Num(); ++i)
    {
        const UHTNPrimitiveTask* Task = Tasks[i];
        if (Task)
        {
            bool bValidTask = true;
            MemWriter << bValidTask;
            
            // Write the task GUID and class name for later reconstruction
            FGuid TaskID = Task->GetTaskID();
            FString ClassName = Task->GetClass()->GetPathName();
            FName TaskName = Task->TaskName;
            float Cost = Task->GetCost();
            
            MemWriter << TaskID;
            MemWriter << ClassName;
            MemWriter << TaskName;
            MemWriter << Cost;
        }
        else
        {
            bool bValidTask = false;
            MemWriter << bValidTask;
        }
    }
    
    // Write task parameters
    int32 ParamCount = TaskParameters.Num();
    MemWriter << ParamCount;
    
    for (auto& Pair : TaskParameters)
    {
        MemWriter << Pair.Key;
        
        // Serialize the property type
        int32 PropType = static_cast<int32>(Pair.Value.GetType());
        MemWriter << PropType;
        
        // Serialize the property value based on its type
        switch (Pair.Value.GetType())
        {
        case EHTNPropertyType::Boolean:
            {
                bool BoolValue = Pair.Value.GetBoolValue();
                MemWriter << BoolValue;
                break;
            }
        case EHTNPropertyType::Integer:
            {
                int32 IntValue = Pair.Value.GetIntValue();
                MemWriter << IntValue;
                break;
            }
        case EHTNPropertyType::Float:
            {
                float FloatValue = Pair.Value.GetFloatValue();
                MemWriter << FloatValue;
                break;
            }
        case EHTNPropertyType::String:
            {
                FString StringValue = Pair.Value.GetStringValue();
                MemWriter << StringValue;
                break;
            }
        case EHTNPropertyType::Name:
            {
                FName NameValue = Pair.Value.GetNameValue();
                MemWriter << NameValue;
                break;
            }
        case EHTNPropertyType::Vector:
            {
                FVector VectorValue = Pair.Value.GetVectorValue();
                MemWriter << VectorValue;
                break;
            }
        case EHTNPropertyType::Object:
            {
                // For objects, just write a null reference as they can't be reliably serialized
                bool bHasObject = false;
                MemWriter << bHasObject;
                break;
            }
            default:
            {
                // Invalid type, just write a marker
                bool bInvalidType = true;
                MemWriter << bInvalidType;
                break;
            }
        }
    }
    
    // Write task results (same pattern as parameters)
    int32 ResultCount = TaskResults.Num();
    MemWriter << ResultCount;
    
    for (auto& Pair : TaskResults)
    {
        MemWriter << Pair.Key;
        
        // Serialize the property type
        int32 PropType = static_cast<int32>(Pair.Value.GetType());
        MemWriter << PropType;
        
        // Serialize the property value based on its type
        switch (Pair.Value.GetType())
        {
        case EHTNPropertyType::Boolean:
            {
                bool BoolValue = Pair.Value.GetBoolValue();
                MemWriter << BoolValue;
                break;
            }
        case EHTNPropertyType::Integer:
            {
                int32 IntValue = Pair.Value.GetIntValue();
                MemWriter << IntValue;
                break;
            }
        case EHTNPropertyType::Float:
            {
                float FloatValue = Pair.Value.GetFloatValue();
                MemWriter << FloatValue;
                break;
            }
        case EHTNPropertyType::String:
            {
                FString StringValue = Pair.Value.GetStringValue();
                MemWriter << StringValue;
                break;
            }
        case EHTNPropertyType::Name:
            {
                FName NameValue = Pair.Value.GetNameValue();
                MemWriter << NameValue;
                break;
            }
        case EHTNPropertyType::Vector:
            {
                FVector VectorValue = Pair.Value.GetVectorValue();
                MemWriter << VectorValue;
                break;
            }
        case EHTNPropertyType::Object:
            {
                // For objects, just write a null reference as they can't be reliably serialized
                bool bHasObject = false;
                MemWriter << bHasObject;
                break;
            }
        default:
            {
                // Invalid type, just write a marker
                bool bInvalidType = true;
                MemWriter << bInvalidType;
                break;
            }
        }
    }
    
    // Write task dependencies
    int32 DependencyMapSize = TaskDependencies.Num();
    MemWriter << DependencyMapSize;
    
    for (auto& Pair : TaskDependencies)
    {
        MemWriter << Pair.Key;
        
        int32 DependencyCount = Pair.Value.Num();
        MemWriter << DependencyCount;
        
        for (int32 DepIndex : Pair.Value)
        {
            MemWriter << DepIndex;
        }
    }
    
    return true;
}

bool FHTNPlan::FromBinary(const TArray<uint8>& InData)
{
    if (InData.Num() == 0)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("Cannot deserialize plan from empty data"));
        return false;
    }
    
    // Create a memory reader to deserialize the data
    FMemoryReader MemReader(InData);
    
    // Read and verify version number
    int32 SerializationVersion;
    MemReader << SerializationVersion;
    
    if (SerializationVersion != 1)
    {
        UE_LOG(LogHTNPlannerPlugin, Error, TEXT("Unsupported serialization version: %d"), SerializationVersion);
        return false;
    }
    
    // Clear existing data
    Clear();
    
    // Read basic plan data
    float TotalCostCopy = TotalCost;
    MemReader << TotalCostCopy;

    int32 CurrentTaskIndexCopy = CurrentTaskIndex;
    MemReader << CurrentTaskIndexCopy;

    // Convert bit fields to bool for serialization
    bool IsExecutingCopy = bIsExecuting;
    MemReader << IsExecutingCopy;

    bool IsCompleteCopy = bIsComplete;
    MemReader << IsCompleteCopy;

    bool FailedCopy = bFailed;
    MemReader << FailedCopy;

    bool IsPausedCopy = bIsPaused;
    MemReader << IsPausedCopy;

    float StartTimeCopy = StartTime;
    MemReader << StartTimeCopy;

    float EndTimeCopy = EndTime;
    MemReader << EndTimeCopy;
    
    // Read the status as an integer
    int32 StatusInt;
    MemReader << StatusInt;
    Status = static_cast<EHTNPlanStatus>(StatusInt);
    
    // Read task references
    int32 TaskCount;
    MemReader << TaskCount;
    
    // This is a placeholder for the tasks, as we can't fully reconstruct them without a task registry
    // In a complete implementation, task reconstruction would require the task registry to find tasks by GUID
    Tasks.Empty(TaskCount);
    
    TMap<FGuid, int32> TaskIndexMap; // Maps task GUIDs to their indices in the plan
    
    for (int32 i = 0; i < TaskCount; ++i)
    {
        bool bValidTask;
        MemReader << bValidTask;
        
        if (bValidTask)
        {
            FGuid TaskID;
            FString ClassName;
            FName TaskName;
            float Cost;
            
            MemReader << TaskID;
            MemReader << ClassName;
            MemReader << TaskName;
            MemReader << Cost;
            
            // In a real implementation, we would look up the task by GUID or create a new one of the proper class
            // For now, we'll just add a null placeholder to maintain the structure
            Tasks.Add(nullptr);
            
            // Store the mapping of GUID to index for potential future use
            TaskIndexMap.Add(TaskID, i);
            
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Loaded task reference [%s] with GUID %s, but actual task objects cannot be reconstructed without a task registry"),
                *TaskName.ToString(), *TaskID.ToString());
        }
        else
        {
            // Add null placeholder for invalid task
            Tasks.Add(nullptr);
        }
    }
    
    // Read task parameters
    int32 ParamCount;
    MemReader << ParamCount;
    
    for (int32 i = 0; i < ParamCount; ++i)
    {
        FName Key;
        MemReader << Key;
        
        // Read the property type
        int32 PropType;
        MemReader << PropType;
        
        EHTNPropertyType Type = static_cast<EHTNPropertyType>(PropType);
        FHTNProperty Value;
        
        // Deserialize the property value based on its type
        switch (Type)
        {
            case EHTNPropertyType::Boolean:
            {
                bool BoolValue;
                MemReader << BoolValue;
                Value = FHTNProperty(BoolValue);
                break;
            }
            case EHTNPropertyType::Integer:
            {
                int32 IntValue;
                MemReader << IntValue;
                Value = FHTNProperty(IntValue);
                break;
            }
            case EHTNPropertyType::Float:
            {
                float FloatValue;
                MemReader << FloatValue;
                Value = FHTNProperty(FloatValue);
                break;
            }
            case EHTNPropertyType::String:
            {
                FString StringValue;
                MemReader << StringValue;
                Value = FHTNProperty(StringValue);
                break;
            }
            case EHTNPropertyType::Name:
            {
                FName NameValue;
                MemReader << NameValue;
                Value = FHTNProperty(NameValue);
                break;
            }
            case EHTNPropertyType::Vector:
            {
                FVector VectorValue;
                MemReader << VectorValue;
                Value = FHTNProperty(VectorValue);
                break;
            }
            case EHTNPropertyType::Object:
            {
                bool bHasObject;
                MemReader << bHasObject;
                // We can't deserialize object references, so just create a null property
                Value = FHTNProperty(static_cast<UObject*>(nullptr));
                break;
            }
            default:
            {
                bool bInvalidType;
                MemReader << bInvalidType;
                // Just create an invalid property
                Value = FHTNProperty::Invalid();
                break;
            }
        }
        
        // Add the parameter to the map
        TaskParameters.Add(Key, Value);
    }
    
    // Read task results
    int32 ResultCount;
    MemReader << ResultCount;
    
    for (int32 i = 0; i < ResultCount; ++i)
    {
        FName Key;
        MemReader << Key;
        
        // Read the property type
        int32 PropType;
        MemReader << PropType;
        
        EHTNPropertyType Type = static_cast<EHTNPropertyType>(PropType);
        FHTNProperty Value;
        
        // Deserialize the property value based on its type
        switch (Type)
        {
            case EHTNPropertyType::Boolean:
            {
                bool BoolValue;
                MemReader << BoolValue;
                Value = FHTNProperty(BoolValue);
                break;
            }
            case EHTNPropertyType::Integer:
            {
                int32 IntValue;
                MemReader << IntValue;
                Value = FHTNProperty(IntValue);
                break;
            }
            case EHTNPropertyType::Float:
            {
                float FloatValue;
                MemReader << FloatValue;
                Value = FHTNProperty(FloatValue);
                break;
            }
            case EHTNPropertyType::String:
            {
                FString StringValue;
                MemReader << StringValue;
                Value = FHTNProperty(StringValue);
                break;
            }
            case EHTNPropertyType::Name:
            {
                FName NameValue;
                MemReader << NameValue;
                Value = FHTNProperty(NameValue);
                break;
            }
            case EHTNPropertyType::Vector:
            {
                FVector VectorValue;
                MemReader << VectorValue;
                Value = FHTNProperty(VectorValue);
                break;
            }
            case EHTNPropertyType::Object:
            {
                bool bHasObject;
                MemReader << bHasObject;
                // We can't deserialize object references, so just create a null property
                Value = FHTNProperty(static_cast<UObject*>(nullptr));
                break;
            }
            default:
            {
                bool bInvalidType;
                MemReader << bInvalidType;
                // Just create an invalid property
                Value = FHTNProperty::Invalid();
                break;
            }
        }
        
        // Add the result to the map
        TaskResults.Add(Key, Value);
    }
    
    // Read task dependencies
    int32 DependencyMapSize;
    MemReader << DependencyMapSize;
    
    for (int32 i = 0; i < DependencyMapSize; ++i)
    {
        int32 TaskIndex;
        MemReader << TaskIndex;
        
        int32 DependencyCount;
        MemReader << DependencyCount;
        
        TArray<int32> Dependencies;
        Dependencies.Reserve(DependencyCount);
        
        for (int32 j = 0; j < DependencyCount; ++j)
        {
            int32 DepIndex;
            MemReader << DepIndex;
            Dependencies.Add(DepIndex);
        }
        
        // Add the dependencies to the map
        TaskDependencies.Add(TaskIndex, Dependencies);
    }
    
    return true;
}

FString FHTNPlan::ToGraphViz() const
{
    FString Result = TEXT("digraph HTNPlan {\n");
    Result += TEXT("  rankdir=LR;\n");
    Result += TEXT("  node [shape=box, style=filled, fontname=\"Arial\"];\n");
    Result += TEXT("  edge [fontname=\"Arial\"];\n\n");
    
    // Add basic plan information as a special node
    Result += FString::Printf(TEXT("  plan_info [shape=note, fillcolor=lightblue, label=\"HTN Plan\\nTotal Cost: %.2f\\nTasks: %d\"];\n\n"),
        TotalCost, Tasks.Num());
    
    // Add nodes for each task
    for (int32 i = 0; i < Tasks.Num(); ++i)
    {
        const UHTNPrimitiveTask* Task = Tasks[i];
        
        FString FillColor;
        FString ExecutionStatus;
        
        if (i < CurrentTaskIndex)
        {
            // Completed task
            FillColor = TEXT("lightgreen");
            ExecutionStatus = TEXT("(Completed)");
        }
        else if (i == CurrentTaskIndex && bIsExecuting)
        {
            // Current task
            FillColor = TEXT("gold");
            ExecutionStatus = TEXT("(Current)");
        }
        else
        {
            // Future task
            FillColor = TEXT("white");
            ExecutionStatus = TEXT("");
        }
        
        // Create label with task details
        FString TaskLabel = FString::Printf(TEXT("Task %d\\n%s\\nCost: %.2f %s"),
            i + 1,
            Task ? *Task->TaskName.ToString().Replace(TEXT("\""), TEXT("\\\"")) : TEXT("NULL"),
            Task ? Task->GetCost() : 0.0f,
            *ExecutionStatus);
        
        // Add parameters if any
        for (const auto& Pair : TaskParameters)
        {
            FString KeyStr = Pair.Key.ToString();
            FString TaskIndexPrefix = FString::Printf(TEXT("Task_%d_"), i);
            if (KeyStr.StartsWith(TaskIndexPrefix))
            {
                FString ParamName = KeyStr.RightChop(TaskIndexPrefix.Len());
                TaskLabel += FString::Printf(TEXT("\\nParam %s: %s"),
                    *ParamName.Replace(TEXT("\""), TEXT("\\\"")),
                    *Pair.Value.ToString().Replace(TEXT("\""), TEXT("\\\"")));
            }
        }
        
        // Add results if any
        for (const auto& Pair : TaskResults)
        {
            FString KeyStr = Pair.Key.ToString();
            FString TaskIndexPrefix = FString::Printf(TEXT("Task_%d_"), i);
            if (KeyStr.StartsWith(TaskIndexPrefix))
            {
                FString ResultName = KeyStr.RightChop(TaskIndexPrefix.Len());
                TaskLabel += FString::Printf(TEXT("\\nResult %s: %s"),
                    *ResultName.Replace(TEXT("\""), TEXT("\\\"")),
                    *Pair.Value.ToString().Replace(TEXT("\""), TEXT("\\\"")));
            }
        }
        
        // Add the node
        Result += FString::Printf(TEXT("  task_%d [fillcolor=%s, label=\"%s\"];\n"),
            i, *FillColor, *TaskLabel);
    }
    
    // Add sequential edges
    for (int32 i = 0; i < Tasks.Num() - 1; ++i)
    {
        Result += FString::Printf(TEXT("  task_%d -> task_%d [style=bold, label=\"Sequence\"];\n"),
            i, i + 1);
    }
    
    // Add dependency edges
    for (const auto& Pair : TaskDependencies)
    {
        int32 TaskIndex = Pair.Key;
        
        for (int32 DepIndex : Pair.Value)
        {
            // Only add valid dependency edges
            if (Tasks.IsValidIndex(TaskIndex) && Tasks.IsValidIndex(DepIndex))
            {
                Result += FString::Printf(TEXT("  task_%d -> task_%d [style=dashed, color=red, label=\"Depends On\"];\n"),
                    TaskIndex, DepIndex);
            }
        }
    }
    
    // Close the graph
    Result += TEXT("}\n");
    
    return Result;
}

FString FHTNPlan::CreateExecutionPreview() const
{
    FString Result = FString::Printf(TEXT("HTN Plan Execution Preview (Total Cost: %.2f, Tasks: %d)\n\n"), 
        TotalCost, Tasks.Num());
    
    // Show current execution status
    FString StatusStr;
    switch (Status)
    {
        case EHTNPlanStatus::NotStarted:
            StatusStr = TEXT("Not Started");
            break;
        case EHTNPlanStatus::Executing:
            StatusStr = FString::Printf(TEXT("Executing (Task %d/%d)"), CurrentTaskIndex + 1, Tasks.Num());
            break;
        case EHTNPlanStatus::Paused:
            StatusStr = FString::Printf(TEXT("Paused (Task %d/%d)"), CurrentTaskIndex + 1, Tasks.Num());
            break;
        case EHTNPlanStatus::Completed:
            StatusStr = TEXT("Completed");
            break;
        case EHTNPlanStatus::Failed:
            StatusStr = TEXT("Failed");
            break;
        case EHTNPlanStatus::Aborted:
            StatusStr = TEXT("Aborted");
            break;
        default:
            StatusStr = TEXT("Unknown");
            break;
    }
    
    Result += FString::Printf(TEXT("Status: %s\n"), *StatusStr);
    
    if (bIsExecuting && !bIsComplete && !bFailed)
    {
        float ElapsedTime = FPlatformTime::Seconds() - StartTime;
        Result += FString::Printf(TEXT("Execution Time: %.2f seconds\n"), ElapsedTime);
    }
    else if (bIsComplete || bFailed)
    {
        Result += FString::Printf(TEXT("Total Execution Time: %.2f seconds\n"), EndTime - StartTime);
    }
    
    Result += TEXT("\nStep-by-Step Execution:\n");
    
    // Check for dependencies to determine task ordering
    TArray<int32> ExecutionOrder;
    TSet<int32> AddedTasks;
    
    // Simple topological sort based on dependencies
    while (AddedTasks.Num() < Tasks.Num())
    {
        bool bAddedTask = false;
        
        for (int32 i = 0; i < Tasks.Num(); ++i)
        {
            if (AddedTasks.Contains(i))
            {
                continue; // Already added
            }
            
            // Check if all dependencies are satisfied
            bool bDependenciesSatisfied = true;
            
            if (const TArray<int32>* Dependencies = TaskDependencies.Find(i))
            {
                for (int32 DepIndex : *Dependencies)
                {
                    if (!AddedTasks.Contains(DepIndex))
                    {
                        bDependenciesSatisfied = false;
                        break;
                    }
                }
            }
            
            if (bDependenciesSatisfied)
            {
                ExecutionOrder.Add(i);
                AddedTasks.Add(i);
                bAddedTask = true;
            }
        }
        
        // If we couldn't add any task, there might be a cycle in dependencies
        if (!bAddedTask)
        {
            Result += TEXT("\nWarning: Possible cyclic dependencies detected.\n");
            
            // Add remaining tasks in sequential order
            for (int32 i = 0; i < Tasks.Num(); ++i)
            {
                if (!AddedTasks.Contains(i))
                {
                    ExecutionOrder.Add(i);
                    AddedTasks.Add(i);
                }
            }
            break;
        }
    }
    
    // Now display tasks in execution order
    for (int32 Step = 0; Step < ExecutionOrder.Num(); ++Step)
    {
        int32 TaskIndex = ExecutionOrder[Step];
        const UHTNPrimitiveTask* Task = Tasks[TaskIndex];
        
        FString StepStatus;
        
        if (TaskIndex < CurrentTaskIndex)
        {
            StepStatus = TEXT("✓ Completed");
        }
        else if (TaskIndex == CurrentTaskIndex && bIsExecuting)
        {
            if (bIsPaused)
            {
                StepStatus = TEXT("⏸ Paused");
            }
            else
            {
                StepStatus = TEXT("► Executing");
            }
        }
        else
        {
            StepStatus = TEXT("⋯ Pending");
        }
        
        // Add the task entry with its status
        Result += FString::Printf(TEXT("\nStep %d. [%s] Task %d: %s (Cost: %.2f)\n"),
            Step + 1,
            *StepStatus,
            TaskIndex + 1, 
            Task ? *Task->TaskName.ToString() : TEXT("NULL"),
            Task ? Task->GetCost() : 0.0f);
        
        // List task parameters if any
        bool bHasParams = false;
        for (const auto& Pair : TaskParameters)
        {
            FString KeyStr = Pair.Key.ToString();
            FString TaskIndexPrefix = FString::Printf(TEXT("Task_%d_"), TaskIndex);
            if (KeyStr.StartsWith(TaskIndexPrefix))
            {
                if (!bHasParams)
                {
                    Result += TEXT("  Parameters:\n");
                    bHasParams = true;
                }
                FString ParamName = KeyStr.RightChop(TaskIndexPrefix.Len());
                Result += FString::Printf(TEXT("    - %s: %s\n"), *ParamName, *Pair.Value.ToString());
            }
        }
        
        // List task results if any (for completed tasks)
        if (TaskIndex < CurrentTaskIndex)
        {
            bool bHasResults = false;
            for (const auto& Pair : TaskResults)
            {
                FString KeyStr = Pair.Key.ToString();
                FString TaskIndexPrefix = FString::Printf(TEXT("Task_%d_"), TaskIndex);
                if (KeyStr.StartsWith(TaskIndexPrefix))
                {
                    if (!bHasResults)
                    {
                        Result += TEXT("  Results:\n");
                        bHasResults = true;
                    }
                    FString ResultName = KeyStr.RightChop(TaskIndexPrefix.Len());
                    Result += FString::Printf(TEXT("    - %s: %s\n"), *ResultName, *Pair.Value.ToString());
                }
            }
        }
        
        // List dependencies if any
        if (const TArray<int32>* Dependencies = TaskDependencies.Find(TaskIndex))
        {
            if (Dependencies->Num() > 0)
            {
                Result += TEXT("  Dependencies:\n");
                for (int32 DepIndex : *Dependencies)
                {
                    const UHTNPrimitiveTask* DepTask = Tasks.IsValidIndex(DepIndex) ? Tasks[DepIndex] : nullptr;
                    Result += FString::Printf(TEXT("    - Task %d: %s\n"), 
                        DepIndex + 1, 
                        DepTask ? *DepTask->TaskName.ToString() : TEXT("NULL"));
                }
            }
        }
    }
    
    return Result;
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

void UHTNPlanLibrary::ClearPlan(FHTNPlan& Plan)
{
    Plan.Clear();
}

bool UHTNPlanLibrary::IsEmptyPlan(const FHTNPlan& Plan)
{
    return Plan.IsEmpty();
}

int32 UHTNPlanLibrary::GetPlanLength(const FHTNPlan& Plan)
{
    return Plan.GetLength();
}

UHTNPrimitiveTask* UHTNPlanLibrary::GetTaskFromPlan(const FHTNPlan& Plan, int32 Index)
{
    return Plan.GetTask(Index);
}

UHTNPrimitiveTask* UHTNPlanLibrary::GetCurrentTask(const FHTNPlan& Plan)
{
    return Plan.GetCurrentTask();
}

void UHTNPlanLibrary::AddTaskToPlan(FHTNPlan& Plan, UHTNPrimitiveTask* Task)
{
    Plan.AddTask(Task);
}

bool UHTNPlanLibrary::IsValidPlan(const FHTNPlan& Plan)
{
    return Plan.IsValid();
}

TArray<UHTNPrimitiveTask*> UHTNPlanLibrary::GetRemainingTasks(const FHTNPlan& Plan)
{
    return Plan.GetRemainingTasks();
}

TArray<UHTNPrimitiveTask*> UHTNPlanLibrary::GetExecutedTasks(const FHTNPlan& Plan)
{
    return Plan.GetExecutedTasks();
}

bool UHTNPlanLibrary::MergePlans(FHTNPlan& TargetPlan, const FHTNPlan& SourcePlan)
{
    return TargetPlan.MergeWith(SourcePlan);
}

FHTNPlan UHTNPlanLibrary::ExtractSubplan(const FHTNPlan& Plan, int32 StartIndex, int32 EndIndex)
{
    return Plan.ExtractSubplan(StartIndex, EndIndex);
}

bool UHTNPlanLibrary::ReplacePlanSection(FHTNPlan& Plan, int32 StartIndex, int32 EndIndex, const FHTNPlan& ReplacementPlan)
{
    return Plan.ReplaceSection(StartIndex, EndIndex, ReplacementPlan);
}

TArray<int32> UHTNPlanLibrary::FindTasksByName(const FHTNPlan& Plan, FName TaskName)
{
    return Plan.FindTasksByName(TaskName);
}

TArray<int32> UHTNPlanLibrary::FindTasksByClass(const FHTNPlan& Plan, TSubclassOf<UHTNPrimitiveTask> TaskClass)
{
    return Plan.FindTasksByClass(TaskClass);
}

bool UHTNPlanLibrary::SetTaskParameter(FHTNPlan& Plan, int32 TaskIndex, FName ParamName, const FHTNProperty& ParamValue)
{
    return Plan.SetTaskParameter(TaskIndex, ParamName, ParamValue);
}

bool UHTNPlanLibrary::GetTaskParameter(const FHTNPlan& Plan, int32 TaskIndex, FName ParamName, FHTNProperty& OutValue)
{
    return Plan.GetTaskParameter(TaskIndex, ParamName, OutValue);
}

bool UHTNPlanLibrary::SetTaskResult(FHTNPlan& Plan, int32 TaskIndex, FName ResultName, const FHTNProperty& ResultValue)
{
    return Plan.SetTaskResult(TaskIndex, ResultName, ResultValue);
}

bool UHTNPlanLibrary::GetTaskResult(const FHTNPlan& Plan, int32 TaskIndex, FName ResultName, FHTNProperty& OutValue)
{
    return Plan.GetTaskResult(TaskIndex, ResultName, OutValue);
}

bool UHTNPlanLibrary::AddTaskDependency(FHTNPlan& Plan, int32 TaskIndex, int32 DependsOnTaskIndex)
{
    return Plan.AddTaskDependency(TaskIndex, DependsOnTaskIndex);
}

bool UHTNPlanLibrary::AreTaskDependenciesSatisfied(const FHTNPlan& Plan, int32 TaskIndex)
{
    return Plan.AreTaskDependenciesSatisfied(TaskIndex);
}

FString UHTNPlanLibrary::GenerateGraphViz(const FHTNPlan& Plan)
{
    return Plan.ToGraphViz();
}

FString UHTNPlanLibrary::CreateExecutionPreview(const FHTNPlan& Plan)
{
    return Plan.CreateExecutionPreview();
}

FString UHTNPlanLibrary::PlanToJson(const FHTNPlan& Plan)
{
    return Plan.ToJson();
}

bool UHTNPlanLibrary::PlanFromJson(const FString& JsonString, FHTNPlan& OutPlan)
{
    return OutPlan.FromJson(JsonString);
}

FString UHTNPlanLibrary::PlanToString(const FHTNPlan& Plan)
{
    return Plan.ToString();
}

FString UHTNPlanLibrary::GetPlanStatusString(const FHTNPlan& Plan)
{
    switch (Plan.Status)
    {
        case EHTNPlanStatus::NotStarted:
            return TEXT("Not Started");
        case EHTNPlanStatus::Executing:
            return TEXT("Executing");
        case EHTNPlanStatus::Paused:
            return TEXT("Paused");
        case EHTNPlanStatus::Completed:
            return TEXT("Completed");
        case EHTNPlanStatus::Failed:
            return TEXT("Failed");
        case EHTNPlanStatus::Aborted:
            return TEXT("Aborted");
        default:
            return TEXT("Unknown Status");
    }
}

float UHTNPlanLibrary::GetPlanTotalCost(const FHTNPlan& Plan)
{
    return Plan.TotalCost;
}

bool UHTNPlanLibrary::IsPlanExecuting(const FHTNPlan& Plan)
{
    return Plan.bIsExecuting;
}

bool UHTNPlanLibrary::IsPlanComplete(const FHTNPlan& Plan)
{
    return Plan.bIsComplete;
}

bool UHTNPlanLibrary::HasPlanFailed(const FHTNPlan& Plan)
{
    return Plan.bFailed;
}