// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphNode.h"
#include "HTNGraphSchema.h"
#include "Tasks/HTNTask.h"
#include "GraphEditorSettings.h"
#include "UObject/NameTypes.h"
#include "Kismet2/BlueprintEditorUtils.h"

UHTNGraphNode::UHTNGraphNode()
{
	NodeColor = FLinearColor(0.5f, 0.5f, 0.5f);
	BorderColor = FLinearColor(0.2f, 0.2f, 0.2f);
}

void UHTNGraphNode::AllocateDefaultPins()
{
	// Create the base pins for the node
	CreateInputPins();
	CreateOutputPins();
}

void UHTNGraphNode::PrepareForCopying()
{
	// Make sure the task reference is properly marked for duplication
	if (UHTNTask* Task = GetHTNTask())
	{
		Task->Rename(nullptr, this, REN_DontCreateRedirectors | REN_DoNotDirty);
	}
}

FText UHTNGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	// Return the custom display name if set
	if (!NodeDisplayName.IsEmpty())
	{
		return NodeDisplayName;
	}
	
	// Get the name from the associated task
	UHTNTask* Task = GetHTNTask();
	if (Task && !Task->TaskName.IsNone())
	{
		return FText::FromName(Task->TaskName);
	}
	
	// Default to the class name if no better name is available
	return FText::FromString(GetClass()->GetName());
}

FLinearColor UHTNGraphNode::GetNodeTitleColor() const
{
	// Return the node's configured title color
	const UGraphEditorSettings* Settings = GetDefault<UGraphEditorSettings>();
	return Settings->FunctionCallNodeTitleColor;
;
}

FText UHTNGraphNode::GetTooltipText() const
{
	// Return the description from the underlying task if available
	UHTNTask* Task = GetHTNTask();
	if (Task && !Task->Description.IsEmpty())
	{
		return FText::FromString(Task->Description);
	}
	
	// Default tooltip
	return FText::FromString(TEXT("HTN Node"));
}

void UHTNGraphNode::AutowireNewNode(UEdGraphPin* FromPin)
{
	if (!FromPin)
	{
		return;
	}
	
	// Get the schema
	const UEdGraphSchema* Schema = GetSchema();
	if (!Schema)
	{
		return;
	}
	
	// Find a compatible pin on this node
	UEdGraphPin* CompatiblePin = nullptr;
	
	// If the source pin is an output, we need to find a compatible input pin
	if (FromPin->Direction == EGPD_Output)
	{
		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin->Direction == EGPD_Input && Schema->CanCreateConnection(FromPin, Pin).Response != CONNECT_RESPONSE_DISALLOW)
			{
				CompatiblePin = Pin;
				break;
			}
		}
	}
	// If the source pin is an input, we need to find a compatible output pin
	else if (FromPin->Direction == EGPD_Input)
	{
		for (UEdGraphPin* Pin : Pins)
		{
			if (Pin->Direction == EGPD_Output && Schema->CanCreateConnection(Pin, FromPin).Response != CONNECT_RESPONSE_DISALLOW)
			{
				CompatiblePin = Pin;
				break;
			}
		}
	}
	
	// Create the connection if we found a compatible pin
	if (CompatiblePin)
	{
		if (FromPin->Direction == EGPD_Output)
		{
			Schema->TryCreateConnection(FromPin, CompatiblePin);
		}
		else
		{
			Schema->TryCreateConnection(CompatiblePin, FromPin);
		}
	}
}

bool UHTNGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* InSchema) const
{
	// Only allow creation in HTN graph
	return InSchema->IsA<UHTNGraphSchema>();
}

FString UHTNGraphNode::GetDocumentationLink() const
{
	// Default documentation link - can be overridden by derived classes
	return TEXT("");
}

FString UHTNGraphNode::GetDocumentationExcerptName() const
{
	// Default excerpt name - can be overridden by derived classes
	return GetClass()->GetName();
}

void UHTNGraphNode::DestroyNode()
{
	// Clean up the task before the node is destroyed
	UHTNTask* Task = GetHTNTask();
	if (Task && Task->GetOuter() == this)
	{
		Task->MarkAsGarbage();
	}
	
	Super::DestroyNode();
}

FText UHTNGraphNode::GetDisplayName() const
{
	return GetNodeTitle(ENodeTitleType::FullTitle);
}

UHTNTask* UHTNGraphNode::RecreateTask()
{
	// Base implementation doesn't create a task - override in derived classes
	return nullptr;
}

void UHTNGraphNode::UpdateFromTask(UHTNTask* InTask)
{
	// Base implementation does nothing - override in derived classes
}

FLinearColor UHTNGraphNode::GetBackgroundColor() const
{
	return NodeColor;
}

FLinearColor UHTNGraphNode::GetBorderColor() const
{
	return BorderColor;
}