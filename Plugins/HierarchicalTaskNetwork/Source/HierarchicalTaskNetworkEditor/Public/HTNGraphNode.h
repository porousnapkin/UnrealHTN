// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphNode.h"
#include "UObject/ObjectMacros.h"
#include "HTNGraphNode.generated.h"

/**
 * Base class for all HTN graph nodes.
 * This provides common functionality for all nodes in the HTN editor graph.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode : public UEdGraphNode
{
	GENERATED_BODY()

public:
	UHTNGraphNode();
	
	// UEdGraphNode interface
	virtual void AllocateDefaultPins() override;
	virtual void PrepareForCopying() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* InSchema) const override;
	virtual FString GetDocumentationLink() const override;
	virtual FString GetDocumentationExcerptName() const override;
	virtual void DestroyNode() override;
	virtual bool CanUserDeleteNode() const override { return true; }
	virtual bool CanDuplicateNode() const override { return true; }
	
	/** Gets the underlying HTN task represented by this node, if any */
	virtual class UHTNTask* GetHTNTask() const { return nullptr; }
	
	/** Gets the display name for this node */
	virtual FText GetDisplayName() const;
	
	/** 
	 * Recreates the underlying HTN task based on the state of this node.
	 * @return The newly created task.
	 */
	virtual class UHTNTask* RecreateTask();
	
	/** 
	 * Updates this node from an existing task.
	 * @param InTask The task to update from.
	 */
	virtual void UpdateFromTask(class UHTNTask* InTask);
	
	/** Get the display background color for this node */
	virtual FLinearColor GetBackgroundColor() const;
	
	/** Get the border color for this node */
	virtual FLinearColor GetBorderColor() const;
	
protected:
	/** Override to create input pins specific to the derived node type */
	virtual void CreateInputPins() {}
	
	/** Override to create output pins specific to the derived node type */
	virtual void CreateOutputPins() {}
	
	/** Custom display name for this node */
	UPROPERTY(EditAnywhere, Category="HTN")
	FText NodeDisplayName;
	
	/** Background color for this node */
	UPROPERTY(EditAnywhere, Category="Appearance")
	FLinearColor NodeColor;
	
	/** Border color for this node */
	UPROPERTY(EditAnywhere, Category="Appearance")
	FLinearColor BorderColor;
};