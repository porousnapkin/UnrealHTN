// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"
#include "HTNGraphSchema.generated.h"

class FHTNPlanAssetEditor;
/**
 * Schema defining the rules for HTN graph connections and node interactions.
 * This class controls what connections are valid between nodes in an HTN graph.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphSchema : public UEdGraphSchema
{
	GENERATED_BODY()

public:
	UHTNGraphSchema();

	// UEdGraphSchema interface
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	virtual bool TryCreateConnection(UEdGraphPin* A, UEdGraphPin* B) const override;
	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, class FSlateWindowElementList& InDrawElements, class UEdGraph* InGraphObj) const override;
	virtual void BreakNodeLinks(UEdGraphNode& TargetNode) const override;
	virtual void BreakPinLinks(UEdGraphPin& TargetPin, bool bSendsNodeNotification) const override;
	virtual void BreakSinglePinLink(UEdGraphPin* SourcePin, UEdGraphPin* TargetPin) const override;
	virtual FText GetPinDisplayName(const UEdGraphPin* Pin) const override;
	
	// Pin types
	static const FName PC_Task;      // Task pin type 
	static const FName PC_Sequence;  // Sequence connection pin type
	static const FName PC_Condition; // Condition pin type
	static const FName PC_Effect;    // Effect pin type

	// Helper function to validate a graph from the context menu
	void ValidateGraphFromContextMenu(const UEdGraph* Graph) const;

	// Helper function to get the editor for a graph
	static FHTNPlanAssetEditor* GetEditorForGraph(const UEdGraph* Graph);
};