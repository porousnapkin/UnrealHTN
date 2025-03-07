// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_CompoundTask.h"
#include "HTNGraphSchema.h"
#include "Tasks/HTNTaskFactory.h"
#include "HTNMethod.h"

UHTNGraphNode_CompoundTask::UHTNGraphNode_CompoundTask()
{
    // Initialize with red color for compound tasks
    NodeColor = FLinearColor(0.7f, 0.2f, 0.2f);
}

FText UHTNGraphNode_CompoundTask::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // Use the compound task name if available
    UHTNCompoundTask* CompoundTask = GetCompoundTask();
    if (CompoundTask)
    {
        if (TitleType == ENodeTitleType::FullTitle)
        {
            if (CompoundTask->Methods.Num() > 0)
            {
                return FText::Format(FText::FromString(TEXT("{0} (Compound: {1} methods)")), 
                    FText::FromName(CompoundTask->TaskName), 
                    FText::AsNumber(CompoundTask->Methods.Num()));
            }
            else
            {
                return FText::Format(FText::FromString(TEXT("{0} (Compound)")), FText::FromName(CompoundTask->TaskName));
            }
        }
        else
        {
            return FText::FromName(CompoundTask->TaskName);
        }
    }
    
    return FText::FromString(TEXT("Compound Task"));
}

FLinearColor UHTNGraphNode_CompoundTask::GetNodeTitleColor() const
{
    // Bright red for compound tasks
    return FLinearColor(0.8f, 0.2f, 0.2f);
}

UHTNTask* UHTNGraphNode_CompoundTask::RecreateTask()
{
    // Create a new compound task if needed
    if (!GetCompoundTask())
    {
        UHTNCompoundTask* NewTask = UHTNTaskFactory::Get()->CreateCompoundTask(UHTNCompoundTask::StaticClass(), this);
        SetCompoundTask(NewTask);
    }
    
    return GetCompoundTask();
}

void UHTNGraphNode_CompoundTask::UpdateFromTask(UHTNTask* InTask)
{
    // Only update if the input task is a compound task
    UHTNCompoundTask* CompoundTask = Cast<UHTNCompoundTask>(InTask);
    if (CompoundTask)
    {
        // Store the task reference
        SetCompoundTask(CompoundTask);
        
        // Reconstruct the node with updated pins
        ReconstructNode();
    }
}

void UHTNGraphNode_CompoundTask::CreateOutputPins()
{
    // Call the parent class implementation
    Super::CreateOutputPins();
    
    // Create pins for methods
    CreateMethodPins();
}

UHTNCompoundTask* UHTNGraphNode_CompoundTask::GetCompoundTask() const
{
    return Cast<UHTNCompoundTask>(Task);
}

void UHTNGraphNode_CompoundTask::SetCompoundTask(UHTNCompoundTask* InCompoundTask)
{
    // Update the base task reference
    SetHTNTask(InCompoundTask);
}

TArray<UHTNMethod*> UHTNGraphNode_CompoundTask::GetMethods() const
{
    UHTNCompoundTask* CompoundTask = GetCompoundTask();
    if (CompoundTask)
    {
        return CompoundTask->Methods;
    }
    
    return TArray<UHTNMethod*>();
}

UHTNMethod* UHTNGraphNode_CompoundTask::AddMethod()
{
    UHTNCompoundTask* CompoundTask = GetCompoundTask();
    if (!CompoundTask)
    {
        // Create the compound task if it doesn't exist
        CompoundTask = Cast<UHTNCompoundTask>(RecreateTask());
        if (!CompoundTask)
        {
            return nullptr;
        }
    }
    
    // Create a new method
    UHTNMethod* NewMethod = NewObject<UHTNMethod>(CompoundTask, UHTNMethod::StaticClass(), NAME_None, RF_Transactional);
    
    // Generate a default name
    int32 MethodCount = CompoundTask->Methods.Num();
    NewMethod->MethodName = FName(*FString::Printf(TEXT("Method_%d"), MethodCount));
    NewMethod->Priority = 1.0f; // Default priority
    
    // Add the method to the compound task
    CompoundTask->Methods.Add(NewMethod);
    
    // Reconstruct the node to update pins
    ReconstructNode();
    
    return NewMethod;
}

bool UHTNGraphNode_CompoundTask::RemoveMethod(UHTNMethod* Method)
{
    UHTNCompoundTask* CompoundTask = GetCompoundTask();
    if (!CompoundTask || !Method)
    {
        return false;
    }
    
    // Try to remove the method
    int32 MethodIndex = CompoundTask->Methods.Find(Method);
    if (MethodIndex != INDEX_NONE)
    {
        // Mark the method for garbage collection
        Method->MarkAsGarbage();
        
        // Remove from the array
        CompoundTask->Methods.RemoveAt(MethodIndex);
        
        // Reconstruct the node to update pins
        ReconstructNode();
        
        return true;
    }
    
    return false;
}

void UHTNGraphNode_CompoundTask::CreateMethodPins()
{
    UHTNCompoundTask* CompoundTask = GetCompoundTask();
    if (!CompoundTask)
    {
        return;
    }
    
    // Create a pin for each method
    for (int32 Index = 0; Index < CompoundTask->Methods.Num(); ++Index)
    {
        UHTNMethod* Method = CompoundTask->Methods[Index];
        if (Method)
        {
            FString PinName = FString::Printf(TEXT("Method_%d"), Index);
            FString PinTooltip = Method->GetDescription();
            UEdGraphPin* MethodPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Sequence, *PinName);
            MethodPin->PinToolTip = PinTooltip;
        }
    }
}