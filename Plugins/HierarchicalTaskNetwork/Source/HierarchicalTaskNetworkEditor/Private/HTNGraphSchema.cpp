// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphSchema.h"
#include "HTNGraphConnectionDrawingPolicy.h"
#include "HTNGraphSchemaActions.h"
#include "ToolMenu.h"
#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "HTNGraphNode_Condition.h"
#include "HTNGraphNode_Effect.h"
#include "HTNGraphNode_CompoundTask.h"
#include "HTNGraphNode_PrimitiveTask.h"
#include "HTNGraphNode_Method.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "Tasks/HTNCompoundTask.h"
#include "Conditions/HTNCondition.h"
#include "Effects/HTNEffect.h"
#include "HTNMethod.h"
#include "HTNEditorLogging.h"
#include "HTNGraph.h"
#include "HTNPlanAsset.h"
#include "HTNPlanAssetEditor.h"

// Define pin types
const FName UHTNGraphSchema::PC_Task = TEXT("Task");
const FName UHTNGraphSchema::PC_Sequence = TEXT("Sequence");
const FName UHTNGraphSchema::PC_Condition = TEXT("Condition");
const FName UHTNGraphSchema::PC_Effect = TEXT("Effect");

UHTNGraphSchema::UHTNGraphSchema()
{
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
            
            // Node-specific actions
            if (const UHTNGraphNode_CompoundTask* CompoundTask = Cast<UHTNGraphNode_CompoundTask>(Context->Node))
            {
                // Add a command to add a method to this compound task
                Section.AddMenuEntry(
                    "AddMethodToCompoundTask",
                    FText::FromString("Add Method"),
                    FText::FromString("Add a new method to this compound task"),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateLambda([CompoundTaskPtr = const_cast<UHTNGraphNode_CompoundTask*>(CompoundTask)]()
                        {
                            if(CompoundTaskPtr)
                            {
                                CompoundTaskPtr->AddMethod();
                                CompoundTaskPtr->GetGraph()->NotifyGraphChanged();
                            }
                        })
                    )
                );
            }
            else if (const UHTNGraphNode_PrimitiveTask* PrimitiveTask = Cast<UHTNGraphNode_PrimitiveTask>(Context->Node))
            {
                // Add commands for primitive task nodes
                Section.AddMenuEntry(
                    "AddConditionToPrimitiveTask",
                    FText::FromString("Add Condition"),
                    FText::FromString("Add a new condition to this primitive task"),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateLambda([PrimitiveTaskPtr = const_cast<UHTNGraphNode_PrimitiveTask*>(PrimitiveTask)]()
                        {
                            if(PrimitiveTaskPtr)
                            {
                                // Placeholder - Would be implemented in the PrimitiveTask class
                                // PrimitiveTaskPtr->AddCondition();
                                PrimitiveTaskPtr->GetGraph()->NotifyGraphChanged();
                            }
                        })
                    )
                );
                
                Section.AddMenuEntry(
                    "AddEffectToPrimitiveTask",
                    FText::FromString("Add Effect"),
                    FText::FromString("Add a new effect to this primitive task"),
                    FSlateIcon(),
                    FUIAction(
                        FExecuteAction::CreateLambda([PrimitiveTaskPtr = const_cast<UHTNGraphNode_PrimitiveTask*>(PrimitiveTask)]()
                        {
                            if(PrimitiveTaskPtr)
                            {
                                // Placeholder - Would be implemented in the PrimitiveTask class
                                // PrimitiveTaskPtr->AddEffect();
                                PrimitiveTaskPtr->GetGraph()->NotifyGraphChanged();
                            }
                        })
                    )
                );
            }
        }
    }

    // Add graph-specific commands
    {
        FToolMenuSection& Section = Menu->AddSection("HTNGraphSchemaGraphActions", FText::FromString("Graph Actions"));
        {
            Section.AddMenuEntry(FGraphEditorCommands::Get().CreateComment);
            
            // Add validate graph command
                            Section.AddMenuEntry(
                "ValidateHTNGraph",
                FText::FromString("Validate Graph"),
                FText::FromString("Check the HTN graph for errors"),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda([this, Context]()
                    {
                        if (Context && Context->Graph)
                        {
                            ValidateGraphFromContextMenu(Context->Graph);
                        }
                    })
                )
            );
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

void UHTNGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const
{
    // Get all classes that derive from UHTNGraphNode
    TArray<UClass*> NodeClasses;
    
    // Add comment node action
    const FText AddCommentString = FText::FromString("Add Comment");
    TSharedPtr<FHTNGraphSchemaAction_NewComment> NewComment(new FHTNGraphSchemaAction_NewComment(
        FText::FromString("Comments"), 
        AddCommentString,
        FText::FromString("Add a comment node"),
        0));
    ContextMenuBuilder.AddAction(NewComment);

    // === Task Nodes Category ===
    FText TasksCategory = FText::FromString("Tasks");
    
    // Primitive Task action
    TSharedPtr<FHTNGraphSchemaAction_NewNode> NewPrimitiveTask(new FHTNGraphSchemaAction_NewNode(
        TasksCategory,
        FText::FromString("Add Primitive Task"),
        FText::FromString("Add a new primitive task to the graph"),
        0,
        UHTNGraphNode_PrimitiveTask::StaticClass(),
        UHTNPrimitiveTask::StaticClass()));
    ContextMenuBuilder.AddAction(NewPrimitiveTask);
    
    // Compound Task action
    TSharedPtr<FHTNGraphSchemaAction_NewNode> NewCompoundTask(new FHTNGraphSchemaAction_NewNode(
        TasksCategory,
        FText::FromString("Add Compound Task"),
        FText::FromString("Add a new compound task to the graph"),
        0,
        UHTNGraphNode_CompoundTask::StaticClass(),
        UHTNCompoundTask::StaticClass()));
    ContextMenuBuilder.AddAction(NewCompoundTask);
    
    // === Method Nodes Category ===
    FText MethodsCategory = FText::FromString("Methods");
    
    // Method action
    TSharedPtr<FHTNGraphSchemaAction_NewNode> NewMethod(new FHTNGraphSchemaAction_NewNode(
        MethodsCategory,
        FText::FromString("Add Method"),
        FText::FromString("Add a new method to the graph"),
        0,
        UHTNGraphNode_Method::StaticClass()));
    ContextMenuBuilder.AddAction(NewMethod);
    
    // === Conditions and Effects Category ===
    FText ConditionsEffectsCategory = FText::FromString("Conditions & Effects");
    
    // Condition action - using specialized action class
    TSharedPtr<FHTNGraphSchemaAction_NewCondition> NewCondition(new FHTNGraphSchemaAction_NewCondition(
        ConditionsEffectsCategory,
        FText::FromString("Add Condition"),
        FText::FromString("Add a new condition to the graph"),
        0,
        UHTNGraphNode_Condition::StaticClass(),
        UHTNCondition::StaticClass()));
    ContextMenuBuilder.AddAction(NewCondition);
    
    // Effect action - using specialized action class
    TSharedPtr<FHTNGraphSchemaAction_NewEffect> NewEffect(new FHTNGraphSchemaAction_NewEffect(
        ConditionsEffectsCategory,
        FText::FromString("Add Effect"),
        FText::FromString("Add a new effect to the graph"),
        0,
        UHTNGraphNode_Effect::StaticClass(),
        UHTNEffect::StaticClass()));
    ContextMenuBuilder.AddAction(NewEffect);
    
    // === Advanced Nodes (customize based on your needs) ===
    // You can add more specialized node types here as needed
}

// Utility function to get the editor for a given graph
FHTNPlanAssetEditor* UHTNGraphSchema::GetEditorForGraph(const UEdGraph* Graph)
{
    if (!Graph)
    {
        return nullptr;
    }
    
    // Try to find the editor that owns this graph
    const UHTNGraph* HTNGraph = Cast<UHTNGraph>(Graph);
    if (!HTNGraph || !HTNGraph->HTNPlanAsset)
    {
        return nullptr;
    }
    
	// Get the HTN plan asset
	UHTNPlanAsset* PlanAsset = HTNGraph->HTNPlanAsset;
    
	// Use the asset editor subsystem to find editors for this asset
	UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
	if (!AssetEditorSubsystem)
	{
		return nullptr;
	}
    
	// Find editor instances for this asset
	TArray<IAssetEditorInstance*> Editors = AssetEditorSubsystem->FindEditorsForAsset(PlanAsset);
    
	// Look for HTN plan editor
	for (IAssetEditorInstance* Editor : Editors)
	{
		// Try to cast to our editor type
		if (FHTNPlanAssetEditor* HTNEditor = static_cast<FHTNPlanAssetEditor*>(Editor))
		{
			return HTNEditor;
		}
	}
    
	return nullptr;
}

// Connect these functions to context menu actions in HTNGraphSchema.cpp
// Helper for context menu action to validate graph
void UHTNGraphSchema::ValidateGraphFromContextMenu(const UEdGraph* Graph) const
{
    FHTNPlanAssetEditor* Editor = GetEditorForGraph(Graph);
    if (Editor)
    {
        Editor->ValidateHTNPlan();
    }
    else
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Warning, TEXT("Could not find editor for graph"));
    }
}
