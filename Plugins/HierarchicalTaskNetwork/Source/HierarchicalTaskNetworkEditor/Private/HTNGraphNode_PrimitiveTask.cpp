// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_PrimitiveTask.h"
#include "HTNGraphSchema.h"
#include "Tasks/HTNTaskFactory.h"
#include "Conditions/HTNCondition.h"
#include "Effects/HTNEffect.h"

UHTNGraphNode_PrimitiveTask::UHTNGraphNode_PrimitiveTask()
{
    // Initialize with green color for primitive tasks
    NodeColor = FLinearColor(0.2f, 0.7f, 0.2f);
}

FText UHTNGraphNode_PrimitiveTask::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    // Use the primitive task name if available
    UHTNPrimitiveTask* PrimitiveTask = GetPrimitiveTask();
    if (PrimitiveTask)
    {
        if (TitleType == ENodeTitleType::FullTitle)
        {
            return FText::Format(FText::FromString(TEXT("{0} (Primitive)")), FText::FromName(PrimitiveTask->TaskName));
        }
        else
        {
            return FText::FromName(PrimitiveTask->TaskName);
        }
    }
    
    return FText::FromString(TEXT("Primitive Task"));
}

FLinearColor UHTNGraphNode_PrimitiveTask::GetNodeTitleColor() const
{
    // Bright green for primitive tasks
    return FLinearColor(0.2f, 0.8f, 0.2f);
}

UHTNTask* UHTNGraphNode_PrimitiveTask::RecreateTask()
{
    // Create a new primitive task if needed
    if (!GetPrimitiveTask())
    {
        UHTNPrimitiveTask* NewTask = UHTNTaskFactory::Get()->CreatePrimitiveTask(UHTNPrimitiveTask::StaticClass(), this);
        SetPrimitiveTask(NewTask);
    }
    
    return GetPrimitiveTask();
}

void UHTNGraphNode_PrimitiveTask::UpdateFromTask(UHTNTask* InTask)
{
    // Only update if the input task is a primitive task
    UHTNPrimitiveTask* PrimitiveTask = Cast<UHTNPrimitiveTask>(InTask);
    if (PrimitiveTask)
    {
        // Store the task reference
        SetPrimitiveTask(PrimitiveTask);
        
        // Reconstruct the node with updated pins
        ReconstructNode();
    }
}

void UHTNGraphNode_PrimitiveTask::CreateOutputPins()
{
    // Call the parent class implementation
    Super::CreateOutputPins();
    
    // Create pins for preconditions and effects
    CreatePreconditionPins();
    CreateEffectPins();
}

UHTNPrimitiveTask* UHTNGraphNode_PrimitiveTask::GetPrimitiveTask() const
{
    return Cast<UHTNPrimitiveTask>(Task);
}

void UHTNGraphNode_PrimitiveTask::SetPrimitiveTask(UHTNPrimitiveTask* InPrimitiveTask)
{
    // Update the base task reference
    SetHTNTask(InPrimitiveTask);
}

void UHTNGraphNode_PrimitiveTask::CreatePreconditionPins()
{
    UHTNPrimitiveTask* PrimitiveTask = GetPrimitiveTask();
    if (!PrimitiveTask)
    {
        return;
    }
    
    // Create a pin for each precondition
    for (int32 Index = 0; Index < PrimitiveTask->Preconditions.Num(); ++Index)
    {
        UHTNCondition* Condition = PrimitiveTask->Preconditions[Index];
        if (Condition)
        {
            FString PinName = FString::Printf(TEXT("Precondition_%d"), Index);
            FString PinTooltip = Condition->GetDescription();
            UEdGraphPin* ConditionPin = CreatePin(EGPD_Input, UHTNGraphSchema::PC_Condition, *PinName);
            ConditionPin->PinToolTip = PinTooltip;
        }
    }
}

void UHTNGraphNode_PrimitiveTask::CreateEffectPins()
{
    UHTNPrimitiveTask* PrimitiveTask = GetPrimitiveTask();
    if (!PrimitiveTask)
    {
        return;
    }
    
    // Create a pin for each effect
    for (int32 Index = 0; Index < PrimitiveTask->Effects.Num(); ++Index)
    {
        UHTNEffect* Effect = PrimitiveTask->Effects[Index];
        if (Effect)
        {
            FString PinName = FString::Printf(TEXT("Effect_%d"), Index);
            FString PinTooltip = Effect->GetDescription();
            UEdGraphPin* EffectPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Effect, *PinName);
            EffectPin->PinToolTip = PinTooltip;
        }
    }
}