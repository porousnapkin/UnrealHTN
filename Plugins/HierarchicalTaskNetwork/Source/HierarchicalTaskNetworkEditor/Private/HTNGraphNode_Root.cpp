// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode_Root.h"
#include "HTNGraphSchema.h"
#include "HTNGraph.h"
#include "EdGraph/EdGraphPin.h"

UHTNGraphNode_Root::UHTNGraphNode_Root()
{
	NodeColor = FLinearColor(0.1f, 0.2f, 0.7f); // Dark blue for root node
	PlanAsset = nullptr;
}

FText UHTNGraphNode_Root::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	// If we have a plan asset, use its name
	if (PlanAsset)
	{
		return FText::Format(FText::FromString(TEXT("HTN Plan: {0}")), FText::FromString(PlanAsset->GetName()));
	}
    
	return FText::FromString(TEXT("HTN Root"));
}

FText UHTNGraphNode_Root::GetTooltipText() const
{
	// Add plan description to tooltip if available
	if (PlanAsset && !PlanAsset->Description.IsEmpty())
	{
		return FText::FromString(FString::Printf(TEXT("The starting point of this HTN plan.\n\nDescription: %s"), 
			*PlanAsset->Description));
	}
    
	return FText::FromString(TEXT("The starting point of this HTN plan."));
}

FLinearColor UHTNGraphNode_Root::GetNodeTitleColor() const
{
	return FLinearColor(1.0f, 1.0f, 1.0f); // White for contrast on dark blue
}

FLinearColor UHTNGraphNode_Root::GetBackgroundColor() const
{
	return NodeColor;
}

UHTNPlanAsset* UHTNGraphNode_Root::GetPlanAsset() const
{
	// First try to get it from our direct reference
	if (PlanAsset)
	{
		return PlanAsset;
	}
    
	// If not set directly, try to get it from the graph
	UHTNGraph* HTNGraph = Cast<UHTNGraph>(GetGraph());
	if (HTNGraph)
	{
		return HTNGraph->HTNPlanAsset;
	}
    
	return nullptr;
}

void UHTNGraphNode_Root::SetPlanAsset(UHTNPlanAsset* InPlanAsset)
{
	PlanAsset = InPlanAsset;
}

void UHTNGraphNode_Root::CreateOutputPins()
{
	// Create the primary output pin for connecting to the first task
	UEdGraphPin* OutputPin = CreatePin(EGPD_Output, UHTNGraphSchema::PC_Task, TEXT("Plan Start"));
}