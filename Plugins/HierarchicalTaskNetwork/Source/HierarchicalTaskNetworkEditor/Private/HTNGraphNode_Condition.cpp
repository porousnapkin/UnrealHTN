// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_Condition.h"
#include "HTNEditorLogging.h"
#include "HTNGraphSchema.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphPin.h"

UHTNGraphNode_Condition::UHTNGraphNode_Condition()
{
	Condition = nullptr;
	NodeTitle = FText::FromString("Condition");
}

void UHTNGraphNode_Condition::AllocateDefaultPins()
{
	// Create pins
	CreateInputPins();
	CreateOutputPins();
}

FText UHTNGraphNode_Condition::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!NodeTitle.IsEmpty())
	{
		return NodeTitle;
	}

	if (Condition)
	{
		return FText::FromString(Condition->GetDescription());
	}
	
	return FText::FromString("Condition");
}

FLinearColor UHTNGraphNode_Condition::GetNodeTitleColor() const
{
	// Use the condition's debug color if available, otherwise default to yellow (typical for conditions)
	if (Condition)
	{
		return Condition->DebugColor;
	}
	
	return FLinearColor::Yellow;
}

FText UHTNGraphNode_Condition::GetTooltipText() const
{
	if (Condition)
	{
		// Use the condition's description for the tooltip
		return FText::FromString(Condition->GetDescription());
	}
	
	return FText::FromString("A condition node represents a check that must be satisfied for a task to be applicable");
}

void UHTNGraphNode_Condition::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	
	// You could add custom logic here when pin connections change
}

FSlateIcon UHTNGraphNode_Condition::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	
	// Use a standard icon from the editor style
	static FSlateIcon Icon("EditorStyle", "ClassIcon.ConditionNode");
	return Icon;
}

bool UHTNGraphNode_Condition::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	// Check if this node can be created under the HTN graph schema
	return Schema->IsA(UHTNGraphSchema::StaticClass());
}

void UHTNGraphNode_Condition::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// When properties change, reconstruct the node to reflect changes
	ReconstructNode();
}

UHTNCondition* UHTNGraphNode_Condition::GetCondition() const
{
	return Condition;
}

void UHTNGraphNode_Condition::SetCondition(UHTNCondition* InCondition)
{
	// Store the new condition
	Condition = InCondition;
	
	// Update the node based on the new condition
	ReconstructNode();
}

void UHTNGraphNode_Condition::UpdateFromCondition(UHTNCondition* InCondition)
{
	if (InCondition)
	{
		// Store the condition reference
		SetCondition(InCondition);
		
		// Reconstruct the node with updated pins
		ReconstructNode();
	}
}

void UHTNGraphNode_Condition::CreateInputPins()
{
	// Create an input pin for the condition to connect to a task
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, UHTNGraphSchema::PC_Condition, TEXT("In"));
	InputPin->bHidden = false;
}

void UHTNGraphNode_Condition::CreateOutputPins()
{
	// Conditions typically don't have outputs in HTN, but you could add an output
	// pin if needed for your specific HTN implementation
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Condition, TEXT("Out"));
	OutputPin->bHidden = false;
}

void UHTNGraphNode_Condition::ReconstructNode()
{
	// Clear existing pins
	Pins.Empty();
	
	// Reallocate pins
	AllocateDefaultPins();
	
	// Notify the graph that this node has changed
	GetGraph()->NotifyGraphChanged();
}
