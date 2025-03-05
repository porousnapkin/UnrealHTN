// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tasks/HTNCompoundTask.h"
#include "Tasks/HTNPrimitiveTask.h"

UHTNCompoundTask::UHTNCompoundTask()
    : Super()
    , MaxDecompositionDepth(10)
    , CurrentDecompositionDepth(0)
{
}

void UHTNCompoundTask::PostInitProperties()
{
    Super::PostInitProperties();

    // Additional initialization specific to compound tasks
}

void UHTNCompoundTask::BeginDestroy()
{
    // Cleanup any resources specific to compound tasks

    Super::BeginDestroy();
}

bool UHTNCompoundTask::Decompose(const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks)
{
    // Reset the current decomposition depth
    CurrentDecompositionDepth = 0;

    // Select the best applicable method
    UHTNMethod* BestMethod = SelectBestMethod(WorldState);
    if (!BestMethod)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Failed to find applicable method for compound task: %s"), *ToString());
        return false;
    }

    // Apply the selected method
    TArray<UHTNTask*> Subtasks;
    if (!ApplyMethod(BestMethod, WorldState, Subtasks))
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Failed to apply method for compound task: %s"), *ToString());
        return false;
    }

    // Decompose all subtasks recursively
    for (UHTNTask* Subtask : Subtasks)
    {
        if (!DecomposeTaskRecursively(Subtask, WorldState, OutTasks, 1))
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Failed to decompose subtask for compound task: %s"), *ToString());
            return false;
        }
    }

    return true;
}

bool UHTNCompoundTask::IsApplicable(const UHTNWorldState* WorldState) const
{
    // A compound task is applicable if at least one of its methods is applicable
    TArray<UHTNMethod*> ApplicableMethods;
    return GetAvailableMethods(WorldState, ApplicableMethods) && ApplicableMethods.Num() > 0;
}

bool UHTNCompoundTask::GetAvailableMethods(const UHTNWorldState* WorldState, TArray<UHTNMethod*>& OutMethods) const
{
    // Check each method for applicability
    for (UHTNMethod* Method : Methods)
    {
        if (Method && Method->IsApplicable(WorldState))
        {
            OutMethods.Add(Method);
        }
    }

    // Sort methods by priority (highest first)
    OutMethods.Sort([](const UHTNMethod& A, const UHTNMethod& B) {
        return A.Priority > B.Priority;
    });

    return OutMethods.Num() > 0;
}

bool UHTNCompoundTask::ApplyMethod(UHTNMethod* Method, const UHTNWorldState* WorldState, TArray<UHTNTask*>& OutTasks) const
{
    if (!Method)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Null method provided to apply for compound task: %s"), *ToString());
        return false;
    }

    // Check if the method is applicable in the current world state
    if (!Method->IsApplicable(WorldState))
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Method %s is not applicable for compound task: %s"), *Method->GetDescription(), *ToString());
        return false;
    }

    // Get the subtasks from the method
    const TArray<UHTNTask*>& Subtasks = Method->GetSubtasks();
    if (Subtasks.Num() == 0)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Method %s has no subtasks for compound task: %s"), *Method->GetDescription(), *ToString());
        return false;
    }

    // Add all subtasks to the output
    OutTasks.Append(Subtasks);

    UE_LOG(LogHTNTask, Verbose, TEXT("Applied method %s for compound task: %s with %d subtasks"), 
        *Method->GetDescription(), *ToString(), Subtasks.Num());

    return true;
}

bool UHTNCompoundTask::ValidateTask_Implementation() const
{
    // First validate the base task
    if (!Super::ValidateTask_Implementation())
    {
        return false;
    }

    // Check if the compound task has at least one method
    if (Methods.Num() == 0)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Compound task %s has no methods"), *ToString());
        return false;
    }

    // Validate all methods
    for (const UHTNMethod* Method : Methods)
    {
        if (!Method)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Compound task %s has null method"), *ToString());
            return false;
        }

        if (!Method->ValidateMethod())
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Compound task %s has invalid method: %s"), *ToString(), *Method->GetDescription());
            return false;
        }
    }

    return true;
}

FString UHTNCompoundTask::GetDecompositionTreeString(int32 Indent) const
{
    FString IndentStr = FString::ChrN(Indent * 2, ' ');
    FString Result = FString::Printf(TEXT("%s%s\n"), *IndentStr, *ToString());

    for (const UHTNMethod* Method : Methods)
    {
        if (Method)
        {
            Result += FString::Printf(TEXT("%s  Method: %s\n"), *IndentStr, *Method->GetDescription());
            
            for (const UHTNTask* Subtask : Method->GetSubtasks())
            {
                if (Subtask)
                {
                    if (const UHTNCompoundTask* CompoundSubtask = Cast<UHTNCompoundTask>(Subtask))
                    {
                        Result += CompoundSubtask->GetDecompositionTreeString(Indent + 2);
                    }
                    else
                    {
                        Result += FString::Printf(TEXT("%s    %s\n"), *IndentStr, *Subtask->ToString());
                    }
                }
            }
        }
    }

    return Result;
}

bool UHTNCompoundTask::DecomposeTaskRecursively(UHTNTask* Task, const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks, int32 CurrentDepth) const
{
    if (!Task)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Trying to decompose null task in compound task: %s"), *ToString());
        return false;
    }

    // Check for maximum recursion depth
    if (CurrentDepth > MaxDecompositionDepth)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Maximum decomposition depth reached (%d) in compound task: %s"), MaxDecompositionDepth, *ToString());
        return false;
    }

    // Check if the task is applicable in the current world state
    if (!Task->IsApplicable(WorldState))
    {
        UE_LOG(LogHTNTask, Verbose, TEXT("Task is not applicable in current world state: %s"), *Task->ToString());
        return false;
    }

    // Handle primitive tasks (leaf nodes in the decomposition tree)
    if (UHTNPrimitiveTask* PrimitiveTask = Cast<UHTNPrimitiveTask>(Task))
    {
        OutTasks.Add(PrimitiveTask);
        return true;
    }
    // Handle compound tasks (internal nodes in the decomposition tree)
    else if (UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(Task))
    {
        // Select the best applicable method
        UHTNMethod* BestMethod = nullptr;
        TArray<UHTNMethod*> ApplicableMethods;
        if (!CompoundTask->GetAvailableMethods(WorldState, ApplicableMethods) || ApplicableMethods.Num() == 0)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("No applicable methods for compound task: %s"), *CompoundTask->ToString());
            return false;
        }

        BestMethod = ApplicableMethods[0];

        // Apply the selected method
        TArray<UHTNTask*> Subtasks;
        if (!CompoundTask->ApplyMethod(BestMethod, WorldState, Subtasks))
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Failed to apply method for compound task: %s"), *CompoundTask->ToString());
            return false;
        }

        // Decompose all subtasks recursively
        for (UHTNTask* Subtask : Subtasks)
        {
            if (!DecomposeTaskRecursively(Subtask, WorldState, OutTasks, CurrentDepth + 1))
            {
                UE_LOG(LogHTNTask, Warning, TEXT("Failed to decompose subtask for compound task: %s"), *CompoundTask->ToString());
                return false;
            }
        }

        return true;
    }
    else
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Unknown task type encountered during decomposition: %s"), *Task->GetClass()->GetName());
        return false;
    }
}

UHTNMethod* UHTNCompoundTask::SelectBestMethod(const UHTNWorldState* WorldState) const
{
    TArray<UHTNMethod*> ApplicableMethods;
    if (GetAvailableMethods(WorldState, ApplicableMethods) && ApplicableMethods.Num() > 0)
    {
        // The methods have already been sorted by priority in GetAvailableMethods
        return ApplicableMethods[0];
    }
    return nullptr;
}