// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphSchema.h"
#include "HTNGraphConnectionDrawingPolicy.h"
#include "ToolMenu.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HTNGraphSchemaActions.h"

// Define pin types
const FName UHTNGraphSchema::PC_Task = TEXT("Task");
const FName UHTNGraphSchema::PC_Sequence = TEXT("Sequence");
const FName UHTNGraphSchema::PC_Condition = TEXT("Condition");
const FName UHTNGraphSchema::PC_Effect = TEXT("Effect");

UHTNGraphSchema::UHTNGraphSchema()
{
}

void UHTNGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
	// Add comment node action
	const FText AddCommentString = FText::FromString("Add Comment");
	TSharedPtr<FHTNGraphSchemaAction_NewComment> NewComment(new FHTNGraphSchemaAction_NewComment(
		FText::FromString("Comments"), 
		AddCommentString,
		FText::FromString("Add a comment node"),
		0));
	ContextMenuBuilder.AddAction(NewComment);

	// In the future, add HTN-specific nodes here
	// This will include Task nodes, Method nodes, Root nodes, etc.
}

void UHTNGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const
{
	// Add standard editing commands
	if (Context->Node)
	{
		FToolMenuSection& Section = Menu->AddSection("HTNGraphSchemaNodeActions", FText::FromString("Node Actions"));
		{
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
		}
	}

	// Add graph-specific commands
	{
		FToolMenuSection& Section = Menu->AddSection("HTNGraphSchemaGraphActions", FText::FromString("Graph Actions"));
		{
			Section.AddMenuEntry(FGraphEditorCommands::Get().CreateComment);
		}
	}
}

const FPinConnectionResponse UHTNGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const
{
	// Make sure the pins are valid
	if (!A || !B)
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Invalid pins"));
	}
	
	// Make sure the pins are not on the same node
	if (A->GetOwningNode() == B->GetOwningNode())
	{
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Cannot connect pins on the same node"));
	}
	
	// Basic compatibility check - can add more rules here
	if (A->PinType.PinCategory == PC_Task && B->PinType.PinCategory == PC_Task)
	{
		if (A->Direction == EGPD_Output && B->Direction == EGPD_Input)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
		}
		else if (A->Direction == EGPD_Input && B->Direction == EGPD_Output)
		{
			return FPinConnectionResponse(CONNECT_RESPONSE_MAKE, TEXT(""));
		}
	}
	
	return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Incompatible pins"));
}

bool UHTNGraphSchema::TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const
{
	// Check if connection is allowed
	const FPinConnectionResponse Response = CanCreateConnection(A, B);
	const bool bModified = UEdGraphSchema::TryCreateConnection(A, B);
	
	return bModified;
}

FLinearColor UHTNGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const
{
	if (PinType.PinCategory == PC_Task)
	{
		return FLinearColor::Blue;
	}
	else if (PinType.PinCategory == PC_Sequence)
	{
		return FLinearColor::Yellow;
	}
	else if (PinType.PinCategory == PC_Condition)
	{
		return FLinearColor::Green;
	}
	else if (PinType.PinCategory == PC_Effect)
	{
		return FLinearColor::Red;
	}
	
	return FLinearColor::White;
}

FConnectionDrawingPolicy* UHTNGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const
{
	// Return a custom drawing policy for HTN connections
	return new FHTNGraphConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements, InGraphObj);
}

void UHTNGraphSchema::BreakNodeLinks(UEdGraphNode& TargetNode) const
{
	UEdGraphSchema::BreakNodeLinks(TargetNode);
}

void UHTNGraphSchema::BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const
{
	UEdGraphSchema::BreakPinLinks(TargetPin, bSendsNodeNotification);
}

void UHTNGraphSchema::BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const
{
	UEdGraphSchema::BreakSinglePinLink(SourcePin, TargetPin);
}

FText UHTNGraphSchema::GetPinDisplayName(const UEdGraphPin* Pin) const
{
	return FText::FromString(Pin->PinName.ToString());
}