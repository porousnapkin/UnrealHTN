// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNPrimitiveTask.h"

#include "HTNWorldStateStruct.h"

UHTNPrimitiveTask::UHTNPrimitiveTask()
    : Super()
    , Status(EHTNTaskStatus::Invalid)
    , ExecutionStartTime(0.0f)
    , MaxExecutionTime(0.0f)
    , bIsExecuting(false)
{
}

void UHTNPrimitiveTask::PostInitProperties()
{
    Super::PostInitProperties();

    // Initialize any properties specific to primitive tasks
}

bool UHTNPrimitiveTask::IsApplicable(const UHTNWorldState* WorldState) const
{
    // Check all preconditions
    for (const UHTNCondition* Condition : Preconditions)
    {
        if (IsValid(Condition) && !Condition->CheckCondition(WorldState))
        {
            // If any precondition fails, the task is not applicable
            return false;
        }
    }

    // If there are no preconditions or all preconditions pass, the task is applicable
    return true;
}

UHTNWorldState* UHTNPrimitiveTask::GetExpectedEffects(const UHTNWorldState* WorldState) const
{
    // Create a copy of the world state to represent the expected effects
    UHTNWorldState* OutEffects = WorldState->Clone();

    // Apply all effects
    for (const UHTNEffect* Effect : Effects)
    {
        if (Effect)
        {
            Effect->ApplyEffect(OutEffects);
        }
    }

    return OutEffects;
}

bool UHTNPrimitiveTask::Decompose(const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks)
{
    // Primitive tasks don't decompose further - they just return themselves
    OutTasks.Add(this);
    return true;
}

bool UHTNPrimitiveTask::Execute(UHTNWorldState* WorldState)
{
    // Can't execute if already executing
    if (bIsExecuting)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Task is already executing: %s"), *ToString());
        return false;
    }

    // Check if the task is applicable in the current world state
    if (!IsApplicable(WorldState))
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Task is not applicable in the current world state: %s"), *ToString());
        SetStatus(EHTNTaskStatus::Failed);
        return false;
    }

    // Start execution
    bIsExecuting = true;
    ExecutionStartTime = FPlatformTime::Seconds();
    SetStatus(EHTNTaskStatus::InProgress);

    // Broadcast task started event
    BroadcastTaskEvent(Status);

    // Execute the task and get its initial status
    EHTNTaskStatus InitialStatus = ExecuteTask(WorldState);
    
    // If the task completed immediately, update the status
    if (InitialStatus != EHTNTaskStatus::InProgress)
    {
        SetStatus(InitialStatus);
        
        // End the task if it completed immediately
        EndTask(WorldState, InitialStatus);
        bIsExecuting = false;
    }

    return true;
}

bool UHTNPrimitiveTask::IsComplete() const
{
    // The task is complete if it's not in progress
    return Status != EHTNTaskStatus::InProgress;
}

EHTNTaskStatus UHTNPrimitiveTask::GetStatus() const
{
    return Status;
}

EHTNTaskStatus UHTNPrimitiveTask::ExecuteTask_Implementation(UHTNWorldState* WorldState)
{
    // Base implementation does nothing and succeeds immediately
    UE_LOG(LogHTNTask, Verbose, TEXT("ExecuteTask not implemented for primitive task: %s - using default success behavior"), *ToString());
    return EHTNTaskStatus::Succeeded;
}

EHTNTaskStatus UHTNPrimitiveTask::TickTask_Implementation(UHTNWorldState* WorldState, float DeltaTime)
{
    // Base implementation just maintains the current status
    // For tasks that execute over time, this should be overridden to update the task's state
    
    // Check for execution timeout if one is set
    if (MaxExecutionTime > 0.0f)
    {
        float CurrentTime = FPlatformTime::Seconds();
        float ElapsedTime = CurrentTime - ExecutionStartTime;
        
        if (ElapsedTime > MaxExecutionTime)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Task execution timed out: %s (%.2fs > %.2fs)"), 
                *ToString(), ElapsedTime, MaxExecutionTime);
            return EHTNTaskStatus::Failed;
        }
    }
    
    return Status;
}

void UHTNPrimitiveTask::EndTask_Implementation(UHTNWorldState* WorldState, EHTNTaskStatus FinalStatus)
{
    // Base implementation does nothing
    // Derived classes should override this to perform cleanup
    
    // If the task succeeded, apply its effects to the world state
    if (FinalStatus == EHTNTaskStatus::Succeeded)
    {
        ApplyEffects(WorldState);
    }
}

void UHTNPrimitiveTask::AbortTask(UHTNWorldState* WorldState)
{
    // Only abort if the task is executing
    if (bIsExecuting)
    {
        UE_LOG(LogHTNTask, Verbose, TEXT("Aborting task: %s"), *ToString());
        
        // Set status to failed
        SetStatus(EHTNTaskStatus::Failed);
        
        // End the task
        EndTask(WorldState, EHTNTaskStatus::Failed);
        
        // Update execution state
        bIsExecuting = false;
    }
}

void UHTNPrimitiveTask::ApplyEffects(UHTNWorldState* WorldState) const
{
    // Apply all effects to the world state
    for (const UHTNEffect* Effect : Effects)
    {
        if (Effect)
        {
            Effect->ApplyEffect(WorldState);
        }
    }
}

void UHTNPrimitiveTask::SetStatus(EHTNTaskStatus NewStatus)
{
    // Only update if the status has changed
    if (Status != NewStatus)
    {
        EHTNTaskStatus OldStatus = Status;
        Status = NewStatus;
        
        // Broadcast the appropriate event based on the new status
        BroadcastTaskEvent(NewStatus);
        
        UE_LOG(LogHTNTask, Verbose, TEXT("Task status changed: %s -> %s for task %s"),
            *StaticEnum<EHTNTaskStatus>()->GetNameStringByValue((int64)OldStatus),
            *StaticEnum<EHTNTaskStatus>()->GetNameStringByValue((int64)NewStatus),
            *ToString());
    }
}

bool UHTNPrimitiveTask::ValidateTask_Implementation() const
{
    // First validate the base task
    if (!Super::ValidateTask_Implementation())
    {
        return false;
    }
    
    // Validate preconditions and effects
    for (const UHTNCondition* Condition : Preconditions)
    {
        if (!Condition)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Null precondition in task: %s"), *ToString());
            return false;
        }
    }
    
    for (const UHTNEffect* Effect : Effects)
    {
        if (!Effect)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Null effect in task: %s"), *ToString());
            return false;
        }
    }
    
    return true;
}

void UHTNPrimitiveTask::BroadcastTaskEvent(EHTNTaskStatus NewStatus)
{
    switch (NewStatus)
    {
    case EHTNTaskStatus::InProgress:
        OnTaskStarted.Broadcast(this);
        break;
    case EHTNTaskStatus::Succeeded:
        OnTaskSucceeded.Broadcast(this);
        break;
    case EHTNTaskStatus::Failed:
        OnTaskFailed.Broadcast(this);
        break;
    default:
        // No broadcast for other statuses
        break;
    }
}