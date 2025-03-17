// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_Effect.h"
#include "HTNEditorLogging.h"
#include "HTNGraphSchema.h"
#include "SGraphNode.h"
#include "EdGraph/EdGraphPin.h"

UHTNGraphNode_Effect::UHTNGraphNode_Effect()
{
	Effect = nullptr;
	NodeTitle = FText::FromString("Effect");
	NodeColor = FLinearColor(0.9f, 0.3f, 0.3f); // Reddish color for effects
}

void UHTNGraphNode_Effect::AllocateDefaultPins()
{
	// Create pins
	CreateInputPins();
	CreateOutputPins();
}

FText UHTNGraphNode_Effect::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	if (!NodeTitle.IsEmpty())
	{
		return NodeTitle;
	}

	if (Effect)
	{
		return FText::FromString(Effect->GetDescription());
	}
	
	return FText::FromString("Effect");
}

FLinearColor UHTNGraphNode_Effect::GetNodeTitleColor() const
{
	// Use the effect's debug color if available, otherwise default to reddish (typical for effects)
	if (Effect)
	{
		return Effect->DebugColor;
	}
	
	return FLinearColor(0.9f, 0.3f, 0.3f);
}

FText UHTNGraphNode_Effect::GetTooltipText() const
{
	if (Effect)
	{
		// Use the effect's description for the tooltip
		return FText::FromString(Effect->GetDescription());
	}
	
	return FText::FromString("An effect node represents a change to the world state after a task is executed");
}

void UHTNGraphNode_Effect::PinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::PinConnectionListChanged(Pin);
	
	// You could add custom logic here when pin connections change
}

FSlateIcon UHTNGraphNode_Effect::GetIconAndTint(FLinearColor& OutColor) const
{
	OutColor = GetNodeTitleColor();
	
	// Use a standard icon from the editor style
	static FSlateIcon Icon("EditorStyle", "ClassIcon.EffectNode");
	return Icon;
}

bool UHTNGraphNode_Effect::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const
{
	// Check if this node can be created under the HTN graph schema
	return Schema->IsA(UHTNGraphSchema::StaticClass());
}

void UHTNGraphNode_Effect::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	// When properties change, reconstruct the node to reflect changes
	ReconstructNode();
}

UHTNEffect* UHTNGraphNode_Effect::GetEffect() const
{
	return Effect;
}

void UHTNGraphNode_Effect::SetEffect(UHTNEffect* InEffect)
{
	// Store the new effect
	Effect = InEffect;
	
	// Update the node based on the new effect
	ReconstructNode();
}

void UHTNGraphNode_Effect::UpdateFromEffect(UHTNEffect* InEffect)
{
	if (InEffect)
	{
		// Store the effect reference
		SetEffect(InEffect);
		
		// Reconstruct the node with updated pins
		ReconstructNode();
	}
}

void UHTNGraphNode_Effect::CreateInputPins()
{
	// Create an input pin for the effect to connect to a task
	UEdGraphPin* InputPin = CreatePin(EGPD_Input, UHTNGraphSchema::PC_Effect, TEXT("In"));
	InputPin->bHidden = false;
}

void UHTNGraphNode_Effect::CreateOutputPins()
{
	// Effects typically don't need output pins in HTN, but in case we need them
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Effect, TEXT("Out"));
	OutputPin->bHidden = false;
}

void UHTNGraphNode_Effect::ReconstructNode()
{
	// Clear existing pins
	Pins.Empty();
	
	// Reallocate pins
	AllocateDefaultPins();
	
	// Notify the graph that this node has changed
	GetGraph()->NotifyGraphChanged();
}