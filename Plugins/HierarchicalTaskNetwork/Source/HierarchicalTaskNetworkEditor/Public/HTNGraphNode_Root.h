// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode.h"
#include "HTNPlanAsset.h"
#include "HTNGraphNode_Root.generated.h"

/**
 * Root node of an HTN graph, representing the starting point of plans.
 * This node has no inputs and provides a single output to connect to the first task.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_Root : public UHTNGraphNode
{
	GENERATED_BODY()

public:
	UHTNGraphNode_Root();

	// UEdGraphNode interface
	virtual bool CanUserDeleteNode() const override { return false; } // Root node can't be deleted
	virtual bool CanDuplicateNode() const override { return false; } // Root node can't be duplicated
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetTooltipText() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
    
	// UHTNGraphNode interface
	virtual FLinearColor GetBackgroundColor() const override;

	/** Get the HTN plan asset this root node belongs to */
	UHTNPlanAsset* GetPlanAsset() const;

	/** Set the HTN plan asset reference */
	void SetPlanAsset(UHTNPlanAsset* InPlanAsset);

protected:
	// UHTNGraphNode interface
	virtual void CreateOutputPins() override;

	/** Reference to the owning HTN plan asset */
	UPROPERTY()
	UHTNPlanAsset* PlanAsset;
};