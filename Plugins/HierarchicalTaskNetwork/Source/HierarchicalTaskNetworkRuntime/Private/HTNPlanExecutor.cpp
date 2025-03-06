// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNPlanExecutor.h"

#include "HTNExecutionContext.h"
#include "HTNLogging.h"

UHTNPlanExecutor::UHTNPlanExecutor()
    : ExecutionMode(EHTNPlanExecutorMode::Sequential)
    , CurrentWorldState(nullptr)
    , OwnerActor(nullptr)
    , MaxTaskExecutionTime(0.0f)
    , bAbortOnTaskFailure(true)
    , bIsExecuting(false)
    , bIsPaused(false)
    , PlanStartTime(0.0f)
{
}

UHTNPlanExecutor::~UHTNPlanExecutor()
{
    // Clean up any resources if needed
    if (bIsExecuting)
    {
        AbortPlan(false);
    }
}

void UHTNPlanExecutor::Tick(float DeltaTime)
{
    if (!bIsExecuting || bIsPaused || !CurrentWorldState)
    {
        return;
    }

    // Get current time for timeout checking
    float CurrentTime = FPlatformTime::Seconds();

    // Check for task timeouts
    CheckTaskTimeouts(CurrentTime);

    // If in sequential mode, only tick the current task
    if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
    {
        // Get the current task
        UHTNPrimitiveTask* CurrentTask = GetCurrentTask();
        if (!CurrentTask)
        {
            // No current task, try to execute the next one
            ExecuteNextTask();
            return;
        }

        // Tick the current task
        if (CurrentTask && CurrentTask->GetStatus() == EHTNTaskStatus::InProgress)
        {
            EHTNTaskStatus NewStatus = CurrentTask->TickTask(ExecutionContext, DeltaTime);
            
            // If the task status changed during the tick, handle it
            if (NewStatus != EHTNTaskStatus::InProgress)
            {
                OnTaskCompleted(CurrentTask, NewStatus);
                
                // Try to execute the next task if the plan hasn't ended
                if (bIsExecuting && !bIsPaused)
                {
                    ExecuteNextTask();
                }
            }
        }
    }
    // For parallel or dependency-based execution, tick all executing tasks
    else
    {
        TArray<UHTNPrimitiveTask*> TasksToRemove;
        
        // Tick all executing tasks
        for (UHTNPrimitiveTask* Task : ExecutingTasks)
        {
            if (Task && Task->GetStatus() == EHTNTaskStatus::InProgress)
            {
                EHTNTaskStatus NewStatus = Task->TickTask(ExecutionContext, DeltaTime);
                
                // If the task status changed during the tick, handle it
                if (NewStatus != EHTNTaskStatus::InProgress)
                {
                    OnTaskCompleted(Task, NewStatus);
                    TasksToRemove.Add(Task);
                }
            }
            else
            {
                // Task is not in progress anymore, remove it
                TasksToRemove.Add(Task);
            }
        }
        
        // Remove completed tasks
        for (UHTNPrimitiveTask* Task : TasksToRemove)
        {
            ExecutingTasks.Remove(Task);
        }
        
        // If we have no executing tasks, try to start new ones
        if (ExecutingTasks.Num() == 0)
        {
            // For parallel mode, start all available tasks
            if (ExecutionMode == EHTNPlanExecutorMode::Parallel)
            {
                bool bTasksStarted = false;
                
                // Try to start all remaining tasks that are ready
                TArray<UHTNPrimitiveTask*> RemainingTasks = CurrentPlan.GetRemainingTasks();
                for (UHTNPrimitiveTask* Task : RemainingTasks)
                {
                    if (Task && Task->IsApplicable(CurrentWorldState))
                    {
                        // Start this task
                        if (Task->Execute(ExecutionContext))
                        {
                            ExecutingTasks.Add(Task);
                            TaskStartTimes.Add(Task, CurrentTime);
                            OnTaskStarted.Broadcast(CurrentPlan, Task);
                            bTasksStarted = true;
                            
                            // If the task completed immediately, handle it
                            if (Task->IsComplete())
                            {
                                OnTaskCompleted(Task, Task->GetStatus());
                                ExecutingTasks.Remove(Task);
                            }
                        }
                    }
                }
                
                // If no tasks could be started, check if the plan is complete
                if (!bTasksStarted)
                {
                    CheckPlanCompletion();
                }
            }
            // For dependency-based mode, start tasks whose dependencies are satisfied
            else if (ExecutionMode == EHTNPlanExecutorMode::DependencyBased)
            {
                bool bTasksStarted = false;
                
                // Get all remaining tasks
                TArray<UHTNPrimitiveTask*> RemainingTasks = CurrentPlan.GetRemainingTasks();
                
                // Try to start tasks whose dependencies are satisfied
                for (int32 i = 0; i < RemainingTasks.Num(); ++i)
                {
                    UHTNPrimitiveTask* Task = RemainingTasks[i];
                    int32 TaskIndex = CurrentPlan.Tasks.Find(Task);
                    
                    if (Task && 
                        TaskIndex != INDEX_NONE && 
                        CurrentPlan.AreTaskDependenciesSatisfied(TaskIndex) &&
                        Task->IsApplicable(CurrentWorldState))
                    {
                        // Start this task
                        if (Task->Execute(ExecutionContext))
                        {
                            ExecutingTasks.Add(Task);
                            TaskStartTimes.Add(Task, CurrentTime);
                            OnTaskStarted.Broadcast(CurrentPlan, Task);
                            bTasksStarted = true;
                            
                            // If the task completed immediately, handle it
                            if (Task->IsComplete())
                            {
                                OnTaskCompleted(Task, Task->GetStatus());
                                ExecutingTasks.Remove(Task);
                            }
                        }
                    }
                }
                
                // If no tasks could be started, check if the plan is complete
                if (!bTasksStarted)
                {
                    CheckPlanCompletion();
                }
            }
        }
    }
}

bool UHTNPlanExecutor::IsTickable() const
{
    return bIsExecuting && !bIsPaused;
}

TStatId UHTNPlanExecutor::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UHTNPlanExecutor, STATGROUP_Tickables);
}

bool UHTNPlanExecutor::StartPlan(const FHTNPlan& InPlan, UHTNExecutionContext* inExecutionContext, AActor* InOwner)
{
    ExecutionContext = inExecutionContext;
    
    // Check if a plan is already executing
    if (bIsExecuting)
    {
        LogExecution(TEXT("Cannot start plan - another plan is already executing"), ELogVerbosity::Warning);
        return false;
    }
    
    // Validate the plan and world state
    if (!InPlan.IsValid())
    {
        LogExecution(TEXT("Cannot start plan - plan is invalid"), ELogVerbosity::Warning);
        return false;
    }
    
    if (!ExecutionContext)
    {
        LogExecution(TEXT("Cannot start plan - world state is null"), ELogVerbosity::Warning);
        return false;
    }
    
    // Set up the execution
    CurrentPlan = InPlan;
    ExecutionContext = ExecutionContext;
    CurrentWorldState = ExecutionContext->GetWorldState();
    OwnerActor = InOwner;
    bIsExecuting = true;
    bIsPaused = false;
    PlanStartTime = FPlatformTime::Seconds();
    
    // Update plan status
    CurrentPlan.Status = EHTNPlanStatus::Executing;
    CurrentPlan.StartTime = PlanStartTime;
    CurrentPlan.bIsExecuting = true;
    CurrentPlan.bIsComplete = false;
    CurrentPlan.bFailed = false;
    CurrentPlan.bIsPaused = false;
    CurrentPlan.CurrentTaskIndex = 0;
    
    LogExecution(FString::Printf(TEXT("Starting plan execution with %d tasks"), CurrentPlan.Tasks.Num()));
    
    // Broadcast plan started event
    OnPlanStarted.Broadcast(CurrentPlan);
    
    // Start executing tasks based on the execution mode
    if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
    {
        // Start with the first task
        return ExecuteNextTask();
    }
    else if (ExecutionMode == EHTNPlanExecutorMode::Parallel)
    {
        // Start all applicable tasks
        bool bTasksStarted = false;
        float CurrentTime = FPlatformTime::Seconds();
        
        for (UHTNPrimitiveTask* Task : CurrentPlan.Tasks)
        {
            if (Task && Task->IsApplicable(CurrentWorldState))
            {
                // Start this task
                if (Task->Execute(ExecutionContext))
                {
                    ExecutingTasks.Add(Task);
                    TaskStartTimes.Add(Task, CurrentTime);
                    OnTaskStarted.Broadcast(CurrentPlan, Task);
                    bTasksStarted = true;
                    
                    // If the task completed immediately, handle it
                    if (Task->IsComplete())
                    {
                        OnTaskCompleted(Task, Task->GetStatus());
                        ExecutingTasks.Remove(Task);
                    }
                }
            }
        }
        
        // If no tasks could be started, the plan fails
        if (!bTasksStarted)
        {
            LogExecution(TEXT("Failed to start any tasks in the plan"), ELogVerbosity::Warning);
            AbortPlan(true);
            return false;
        }
        
        return true;
    }
    else if (ExecutionMode == EHTNPlanExecutorMode::DependencyBased)
    {
        // Start tasks with no dependencies
        bool bTasksStarted = false;
        float CurrentTime = FPlatformTime::Seconds();
        
        for (int32 i = 0; i < CurrentPlan.Tasks.Num(); ++i)
        {
            UHTNPrimitiveTask* Task = CurrentPlan.Tasks[i];
            
            if (Task && 
                CurrentPlan.AreTaskDependenciesSatisfied(i) &&
                Task->IsApplicable(CurrentWorldState))
            {
                // Start this task
                if (Task->Execute(ExecutionContext))
                {
                    ExecutingTasks.Add(Task);
                    TaskStartTimes.Add(Task, CurrentTime);
                    OnTaskStarted.Broadcast(CurrentPlan, Task);
                    bTasksStarted = true;
                    
                    // If the task completed immediately, handle it
                    if (Task->IsComplete())
                    {
                        OnTaskCompleted(Task, Task->GetStatus());
                        ExecutingTasks.Remove(Task);
                    }
                }
            }
        }
        
        // If no tasks could be started, the plan fails
        if (!bTasksStarted)
        {
            LogExecution(TEXT("Failed to start any tasks in the plan"), ELogVerbosity::Warning);
            AbortPlan(true);
            return false;
        }
        
        return true;
    }
    
    // Unrecognized execution mode
    LogExecution(FString::Printf(TEXT("Unrecognized execution mode: %d"), 
                 static_cast<int32>(ExecutionMode)), ELogVerbosity::Warning);
    return false;
}

bool UHTNPlanExecutor::PausePlan()
{
    if (!bIsExecuting || bIsPaused)
    {
        return false;
    }
    
    bIsPaused = true;
    CurrentPlan.bIsPaused = true;
    CurrentPlan.Status = EHTNPlanStatus::Paused;
    
    LogExecution(TEXT("Plan execution paused"));
    OnPlanPaused.Broadcast(CurrentPlan);
    
    return true;
}

bool UHTNPlanExecutor::ResumePlan()
{
    if (!bIsExecuting || !bIsPaused)
    {
        return false;
    }
    
    bIsPaused = false;
    CurrentPlan.bIsPaused = false;
    CurrentPlan.Status = EHTNPlanStatus::Executing;
    
    LogExecution(TEXT("Plan execution resumed"));
    OnPlanResumed.Broadcast(CurrentPlan);
    
    return true;
}

bool UHTNPlanExecutor::AbortPlan(bool bFailTasks)
{
    if (!bIsExecuting)
    {
        return false;
    }
    
    LogExecution(TEXT("Aborting plan execution"));
    
    // Abort all executing tasks
    for (UHTNPrimitiveTask* Task : ExecutingTasks)
    {
        if (Task && Task->GetStatus() == EHTNTaskStatus::InProgress)
        {
            Task->AbortTask(ExecutionContext);
            
            if (bFailTasks)
            {
                OnTaskFailed.Broadcast(CurrentPlan, Task);
            }
        }
    }
    
    // Update plan status
    CurrentPlan.bIsExecuting = false;
    CurrentPlan.bIsComplete = false;
    CurrentPlan.bFailed = bFailTasks;
    CurrentPlan.bIsPaused = false;
    CurrentPlan.Status = EHTNPlanStatus::Aborted;
    CurrentPlan.EndTime = FPlatformTime::Seconds();
    
    // Broadcast plan aborted event
    OnPlanAborted.Broadcast(CurrentPlan);
    
    // Clean up the execution state
    CleanupPlan();
    
    return true;
}

bool UHTNPlanExecutor::IsExecutingPlan() const
{
    return bIsExecuting;
}

const FHTNPlan& UHTNPlanExecutor::GetCurrentPlan() const
{
    return CurrentPlan;
}

UHTNWorldState* UHTNPlanExecutor::GetWorldState() const
{
    return CurrentWorldState;
}

AActor* UHTNPlanExecutor::GetOwner() const
{
    return OwnerActor;
}

void UHTNPlanExecutor::SetExecutionMode(EHTNPlanExecutorMode InExecutionMode)
{
    ExecutionMode = InExecutionMode;
}

void UHTNPlanExecutor::SetMaxTaskExecutionTime(float InMaxTaskExecutionTime)
{
    MaxTaskExecutionTime = InMaxTaskExecutionTime;
}

void UHTNPlanExecutor::SetAbortOnTaskFailure(bool bInAbortOnTaskFailure)
{
    bAbortOnTaskFailure = bInAbortOnTaskFailure;
}

UHTNPrimitiveTask* UHTNPlanExecutor::GetCurrentTask() const
{
    if (!bIsExecuting || !CurrentPlan.IsValid() || 
        !CurrentPlan.Tasks.IsValidIndex(CurrentPlan.CurrentTaskIndex))
    {
        return nullptr;
    }
    
    return CurrentPlan.Tasks[CurrentPlan.CurrentTaskIndex];
}

bool UHTNPlanExecutor::IsTaskExecuting(UHTNPrimitiveTask* Task) const
{
    if (!Task || !bIsExecuting)
    {
        return false;
    }
    
    if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
    {
        UHTNPrimitiveTask* CurrentTask = GetCurrentTask();
        return CurrentTask == Task && CurrentTask->GetStatus() == EHTNTaskStatus::InProgress;
    }
    else
    {
        return ExecutingTasks.Contains(Task) && Task->GetStatus() == EHTNTaskStatus::InProgress;
    }
}

EHTNTaskStatus UHTNPlanExecutor::GetTaskStatus(UHTNPrimitiveTask* Task) const
{
    if (!Task)
    {
        return EHTNTaskStatus::Invalid;
    }
    
    return Task->GetStatus();
}

bool UHTNPlanExecutor::ExecuteNextTask()
{
    if (!bIsExecuting || bIsPaused || !CurrentWorldState)
    {
        return false;
    }
    
    // For sequential execution, find the next task to execute
    if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
    {
        // Check if there are any tasks left to execute
        if (CurrentPlan.CurrentTaskIndex >= CurrentPlan.Tasks.Num())
        {
            // No more tasks, plan is complete
            LogExecution(TEXT("No more tasks to execute, plan completed successfully"));
            
            // Update plan status
            CurrentPlan.bIsExecuting = false;
            CurrentPlan.bIsComplete = true;
            CurrentPlan.bFailed = false;
            CurrentPlan.Status = EHTNPlanStatus::Completed;
            CurrentPlan.EndTime = FPlatformTime::Seconds();
            
            // Broadcast plan completed event
            OnPlanCompleted.Broadcast(CurrentPlan);
            
            // Clean up the execution state
            CleanupPlan();
            
            return false;
        }
        
        // Get the next task to execute
        UHTNPrimitiveTask* NextTask = CurrentPlan.Tasks[CurrentPlan.CurrentTaskIndex];
        if (!NextTask)
        {
            // Invalid task, skip it
            LogExecution(FString::Printf(TEXT("Skipping null task at index %d"), CurrentPlan.CurrentTaskIndex), ELogVerbosity::Warning);
            ++CurrentPlan.CurrentTaskIndex;
            return ExecuteNextTask();
        }
        
        // Check if the task is applicable
        if (!NextTask->IsApplicable(CurrentWorldState))
        {
            // Task is not applicable, fail it
            LogExecution(FString::Printf(TEXT("Task %s is not applicable, failing it"), *NextTask->ToString()), ELogVerbosity::Warning);
            
            // Mark the task as failed
            OnTaskCompleted(NextTask, EHTNTaskStatus::Failed);
            
            // Move to the next task if we're still executing
            if (bIsExecuting && !bIsPaused)
            {
                return ExecuteNextTask();
            }
            
            return false;
        }
        
        // Execute the task
        LogExecution(FString::Printf(TEXT("Executing task %s"), *NextTask->ToString()));
        
        float CurrentTime = FPlatformTime::Seconds();
        TaskStartTimes.Add(NextTask, CurrentTime);
        
        if (NextTask->Execute(ExecutionContext))
        {
            // Task execution started successfully
            OnTaskStarted.Broadcast(CurrentPlan, NextTask);
            
            // If the task completed immediately, handle it
            if (NextTask->IsComplete())
            {
                OnTaskCompleted(NextTask, NextTask->GetStatus());
                
                // Move to the next task if we're still executing
                if (bIsExecuting && !bIsPaused)
                {
                    return ExecuteNextTask();
                }
            }
            
            return true;
        }
        else
        {
            // Task execution failed to start
            LogExecution(FString::Printf(TEXT("Failed to start execution of task %s"), *NextTask->ToString()), ELogVerbosity::Warning);
            
            // Mark the task as failed
            OnTaskCompleted(NextTask, EHTNTaskStatus::Failed);
            
            // Move to the next task if we're still executing
            if (bIsExecuting && !bIsPaused)
            {
                return ExecuteNextTask();
            }
            
            return false;
        }
    }
    
    // For parallel or dependency-based execution, we handle task execution in the Tick function
    return false;
}

FString UHTNPlanExecutor::ToString() const
{
    FString Result = FString::Printf(TEXT("HTN Plan Executor - %s\n"), 
        bIsExecuting ? (bIsPaused ? TEXT("Paused") : TEXT("Running")) : TEXT("Idle"));
    
    if (bIsExecuting)
    {
        Result += FString::Printf(TEXT("Execution Time: %.2f seconds\n"), 
            FPlatformTime::Seconds() - PlanStartTime);
        Result += FString::Printf(TEXT("Mode: %s\n"), 
            *StaticEnum<EHTNPlanExecutorMode>()->GetNameStringByValue(static_cast<int64>(ExecutionMode)));
        
        Result += TEXT("\n");
        Result += CurrentPlan.ToString();
    }
    
    return Result;
}

void UHTNPlanExecutor::OnTaskCompleted(UHTNPrimitiveTask* Task, EHTNTaskStatus Status)
{
    if (!bIsExecuting || !Task)
    {
        return;
    }
    
    LogExecution(FString::Printf(TEXT("Task %s completed with status: %s"), 
        *Task->ToString(), 
        *StaticEnum<EHTNTaskStatus>()->GetNameStringByValue(static_cast<int64>(Status))));
    
    // Remove task from execution tracking
    TaskStartTimes.Remove(Task);

    Task->EndTask(ExecutionContext, Status);
    
    if (Status == EHTNTaskStatus::Succeeded)
    {
        // Broadcast task succeeded event
        OnTaskSucceeded.Broadcast(CurrentPlan, Task);
        
        // If in sequential mode, move to the next task
        if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
        {
            ++CurrentPlan.CurrentTaskIndex;
        }
    }
    else if (Status == EHTNTaskStatus::Failed)
    {
        // Broadcast task failed event
        OnTaskFailed.Broadcast(CurrentPlan, Task);
        
        // Check if we should abort the plan on task failure
        if (bAbortOnTaskFailure)
        {
            LogExecution(FString::Printf(TEXT("Aborting plan due to task failure: %s"), *Task->ToString()), ELogVerbosity::Warning);
            AbortPlan(true);
        }
        else if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
        {
            // Just move to the next task in sequential mode
            ++CurrentPlan.CurrentTaskIndex;
        }
    }
    
    // Check if the plan has completed
    CheckPlanCompletion();
}

void UHTNPlanExecutor::CheckTaskTimeouts(float CurrentTime)
{
    if (MaxTaskExecutionTime <= 0.0f)
    {
        return; // No timeout
    }
    
    TArray<UHTNPrimitiveTask*> TasksToTimeout;
    
    // Check each executing task for timeout
    for (const auto& Pair : TaskStartTimes)
    {
        UHTNPrimitiveTask* Task = Pair.Key;
        float StartTime = Pair.Value;
        
        if (Task && CurrentTime - StartTime > MaxTaskExecutionTime)
        {
            TasksToTimeout.Add(Task);
        }
    }
    
    // Handle timed out tasks
    for (UHTNPrimitiveTask* Task : TasksToTimeout)
    {
        LogExecution(FString::Printf(TEXT("Task %s timed out after %.2f seconds"), 
            *Task->ToString(), MaxTaskExecutionTime), ELogVerbosity::Warning);
        
        // Abort the task
        Task->AbortTask(ExecutionContext);
        
        // Broadcast task timeout event
        OnTaskTimeout.Broadcast(CurrentPlan, Task);
        
        // Remove from executing tasks
        ExecutingTasks.Remove(Task);
        TaskStartTimes.Remove(Task);
        
        // Mark as failed and handle completion
        OnTaskCompleted(Task, EHTNTaskStatus::Failed);
    }
}

bool UHTNPlanExecutor::CheckPlanCompletion()
{
    if (!bIsExecuting)
    {
        return false;
    }
    
    bool bAllTasksCompleted = false;
    
    if (ExecutionMode == EHTNPlanExecutorMode::Sequential)
    {
        // Sequential mode - check if we've reached the end of the task list
        bAllTasksCompleted = CurrentPlan.CurrentTaskIndex >= CurrentPlan.Tasks.Num();
    }
    else
    {
        // Parallel or dependency-based mode - check if all tasks are completed
        bool bHasRemainingTasks = false;
        
        for (UHTNPrimitiveTask* Task : CurrentPlan.Tasks)
        {
            if (Task && !Task->IsComplete())
            {
                bHasRemainingTasks = true;
                break;
            }
        }
        
        bAllTasksCompleted = !bHasRemainingTasks && ExecutingTasks.Num() == 0;
    }
    
    if (bAllTasksCompleted)
    {
        // Check if any tasks failed
        bool bAnyTaskFailed = false;
        for (UHTNPrimitiveTask* Task : CurrentPlan.Tasks)
        {
            if (Task && Task->GetStatus() == EHTNTaskStatus::Failed)
            {
                bAnyTaskFailed = true;
                break;
            }
        }
        
        // Update plan status
        CurrentPlan.bIsExecuting = false;
        CurrentPlan.bIsComplete = !bAnyTaskFailed;
        CurrentPlan.bFailed = bAnyTaskFailed;
        CurrentPlan.Status = bAnyTaskFailed ? EHTNPlanStatus::Failed : EHTNPlanStatus::Completed;
        CurrentPlan.EndTime = FPlatformTime::Seconds();
        
        if (bAnyTaskFailed)
        {
            LogExecution(TEXT("Plan execution failed - some tasks failed"));
            OnPlanFailed.Broadcast(CurrentPlan);
        }
        else
        {
            LogExecution(TEXT("Plan execution completed successfully"));
            OnPlanCompleted.Broadcast(CurrentPlan);
        }
        
        // Clean up the execution state
        CleanupPlan();
        
        return true;
    }
    
    return false;
}

void UHTNPlanExecutor::CleanupPlan()
{
    bIsExecuting = false;
    bIsPaused = false;
    ExecutingTasks.Empty();
    TaskStartTimes.Empty();
}

void UHTNPlanExecutor::ApplyTaskEffects(UHTNPrimitiveTask* Task)
{
    if (!Task || !CurrentWorldState)
    {
        return;
    }
    
    LogExecution(FString::Printf(TEXT("Applying effects of task %s"), *Task->ToString()));
    Task->ApplyEffects(ExecutionContext);
}

void UHTNPlanExecutor::LogExecution(const FString& Message, ELogVerbosity::Type Verbosity)
{
    FString FullMessage = FString::Printf(TEXT("[HTNPlanExecutor] %s"), *Message);
    
    switch (Verbosity)
    {
        case ELogVerbosity::Error:
            UE_LOG(LogHTNPlannerPlugin, Error, TEXT("%s"), *FullMessage);
            break;
        case ELogVerbosity::Warning:
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("%s"), *FullMessage);
            break;
        case ELogVerbosity::Display:
            UE_LOG(LogHTNPlannerPlugin, Display, TEXT("%s"), *FullMessage);
            break;
        case ELogVerbosity::Log:
            UE_LOG(LogHTNPlannerPlugin, Log, TEXT("%s"), *FullMessage);
            break;
        default:
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("%s"), *FullMessage);
            break;
    }
    
    // Also trigger monitor event for warnings and errors
    if (Verbosity <= ELogVerbosity::Warning)
    {
        TriggerMonitorEvent(Verbosity == ELogVerbosity::Error ? TEXT("Error") : TEXT("Warning"), Message);
    }
}

void UHTNPlanExecutor::TriggerMonitorEvent(FName MonitorType, const FString& Message)
{
    OnMonitorEvent.Broadcast(CurrentPlan, MonitorType, Message);
}