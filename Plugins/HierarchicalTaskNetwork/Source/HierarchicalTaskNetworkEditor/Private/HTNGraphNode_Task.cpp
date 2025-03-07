// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_Task.h"
#include "HTNGraphSchema.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "Tasks/HTNCompoundTask.h"
#include "Tasks/HTNTaskFactory.h"

UHTNGraphNode_Task::UHTNGraphNode_Task()
{
    // Initialize with default colors based on task type
    NodeColor = FLinearColor(0.3f, 0.6f, 0.9f); // Blue for generic tasks
}

FText UHTNGraphNode_Task::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if (Task != nullptr)
    {
        // If we have a task, use its name
        return FText::FromName(Task->TaskName);
    }
    
    // Default title if no task is assigned
    return FText::FromString(TEXT("HTN Task"));
}

void UHTNGraphNode_Task::AllocateDefaultPins()
{
    // Create the basic pins for the node
    CreateInputPins();
    CreateOutputPins();
}

FLinearColor UHTNGraphNode_Task::GetNodeTitleColor() const
{
    // Customize title color based on task type
    if (Task)
    {
        if (Task->IsA<UHTNPrimitiveTask>())
        {
            return FLinearColor(0.2f, 0.8f, 0.2f); // Green for primitive tasks
        }
        else if (Task->IsA<UHTNCompoundTask>())
        {
            return FLinearColor(0.8f, 0.2f, 0.2f); // Red for compound tasks
        }
    }
    
    return FLinearColor(0.8f, 0.8f, 0.8f); // Light gray for unknown/unassigned tasks
}

FText UHTNGraphNode_Task::GetTooltipText() const
{
    if (Task != nullptr)
    {
        if (!Task->Description.IsEmpty())
        {
            return FText::FromString(Task->Description);
        }
        
        return FText::FromString(FString::Printf(TEXT("Task: %s"), *Task->TaskName.ToString()));
    }
    
    return FText::FromString(TEXT("No task assigned"));
}

UHTNTask* UHTNGraphNode_Task::GetHTNTask() const
{
    return Task;
}

void UHTNGraphNode_Task::SetHTNTask(UHTNTask* InTask)
{
    // Store the task reference
    Task = InTask;
    
    // If the task has an owner, we need to reparent it to this node
    if (Task && Task->GetOuter() != this)
    {
        Task->Rename(nullptr, this, REN_DontCreateRedirectors);
    }
}

UHTNTask* UHTNGraphNode_Task::RecreateTask()
{
    // Recreate the task if needed
    if (!Task)
    {
        // By default, create a generic task
        UClass* TaskClass = UHTNTask::StaticClass();
        Task = NewObject<UHTNTask>(this, TaskClass, NAME_None, RF_Transactional);
        Task->TaskName = FName(*GetNodeTitle(ENodeTitleType::FullTitle).ToString());
    }
    
    return Task;
}

void UHTNGraphNode_Task::UpdateFromTask(UHTNTask* InTask)
{
    if (InTask)
    {
        // Update this node with the new task
        SetHTNTask(InTask);
        
        // Update visual properties based on the task
        if (UHTNPrimitiveTask* PrimitiveTask = Cast<UHTNPrimitiveTask>(InTask))
        {
            NodeColor = FLinearColor(0.2f, 0.7f, 0.2f); // Green for primitive tasks
        }
        else if (UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(InTask))
        {
            NodeColor = FLinearColor(0.7f, 0.2f, 0.2f); // Red for compound tasks
        }
        
        // Recreate pins to match the task type
        ReconstructNode();
    }
}

FLinearColor UHTNGraphNode_Task::GetBackgroundColor() const
{
    // Use different background colors for different task types
    if (Task)
    {
        if (Task->IsA<UHTNPrimitiveTask>())
        {
            return FLinearColor(0.1f, 0.4f, 0.1f); // Darker green for primitive tasks
        }
        else if (Task->IsA<UHTNCompoundTask>())
        {
            return FLinearColor(0.4f, 0.1f, 0.1f); // Darker red for compound tasks
        }
    }
    
    // Default color
    return NodeColor;
}

void UHTNGraphNode_Task::CreateInputPins()
{
    // Create the primary input pin for connecting from parent tasks or methods
    UEdGraphPin* InputPin = CreatePin(EGPD_Input, UHTNGraphSchema::PC_Task, TEXT("In"));
}

void UHTNGraphNode_Task::CreateOutputPins()
{
    // Create the primary output pin for connecting to child tasks
    UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Task, TEXT("Out"));
    
    // If this is a compound task, it might need additional pins
    if (Task && Task->IsA<UHTNCompoundTask>())
    {
        // Could add method-specific pins here
    }
}