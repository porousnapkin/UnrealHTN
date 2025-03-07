// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_Method.h"
#include "HTNGraphSchema.h"

UHTNGraphNode_Method::UHTNGraphNode_Method()
{
    // Initialize with default colors for method nodes
    NodeColor = FLinearColor(0.9f, 0.6f, 0.1f); // Orange for methods
}

FText UHTNGraphNode_Method::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
    if (Method != nullptr)
    {
        // If we have a method, use its name
        return FText::FromName(Method->MethodName);
    }
    
    // Default title if no method is assigned
    return FText::FromString(TEXT("HTN Method"));
}

void UHTNGraphNode_Method::AllocateDefaultPins()
{
    // Create the basic pins for the node
    CreateInputPins();
    CreateOutputPins();
}

FLinearColor UHTNGraphNode_Method::GetNodeTitleColor() const
{
    // Customize title color based on method priority
    if (Method)
    {
        // Higher priority methods get brighter colors
        float Intensity = FMath::Clamp(Method->Priority * 0.1f, 0.5f, 1.0f);
        return FLinearColor(1.0f, 0.8f * Intensity, 0.3f * Intensity); // Yellowish for methods
    }
    
    return FLinearColor(1.0f, 0.8f, 0.3f); // Default method color
}

FText UHTNGraphNode_Method::GetTooltipText() const
{
    if (Method != nullptr)
    {
        if (!Method->Description.IsEmpty())
        {
            return FText::FromString(Method->Description);
        }
        
        return FText::FromString(FString::Printf(TEXT("Method: %s (Priority: %.1f)"), 
            *Method->MethodName.ToString(), Method->Priority));
    }
    
    return FText::FromString(TEXT("No method assigned"));
}

FLinearColor UHTNGraphNode_Method::GetBackgroundColor() const
{
    // Use a darker orange for the background
    return FLinearColor(0.4f, 0.25f, 0.05f);
}

void UHTNGraphNode_Method::SetMethod(UHTNMethod* InMethod)
{
    // Store the method reference
    Method = InMethod;
    
    // If the method has an owner, we need to reparent it to this node
    if (Method && Method->GetOuter() != this)
    {
        Method->Rename(nullptr, this, REN_DontCreateRedirectors);
    }
}

UHTNMethod* UHTNGraphNode_Method::RecreateMethod()
{
    // Recreate the method if needed
    if (!Method)
    {
        Method = NewObject<UHTNMethod>(this, UHTNMethod::StaticClass(), NAME_None, RF_Transactional);
        Method->MethodName = FName(*GetNodeTitle(ENodeTitleType::FullTitle).ToString());
        Method->Priority = 1.0f; // Default priority
    }
    
    return Method;
}

void UHTNGraphNode_Method::UpdateFromMethod(UHTNMethod* InMethod)
{
    if (InMethod)
    {
        // Update this node with the new method
        SetMethod(InMethod);
        
        // Recreate pins to match the method
        ReconstructNode();
    }
}

void UHTNGraphNode_Method::CreateInputPins()
{
    // Create the primary input pin for connecting from compound tasks
    UEdGraphPin* InputPin = CreatePin(EGPD_Input, UHTNGraphSchema::PC_Sequence, TEXT("In"));
}

void UHTNGraphNode_Method::CreateOutputPins()
{
    // Create the output pin for connecting to subtasks
    UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Sequence, TEXT("Out"));
    
    // You could add more pins here based on the method's properties
    // For example, pins for each condition or parameter
}