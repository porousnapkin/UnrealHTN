// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode.h"
#include "Tasks/HTNTask.h"
#include "HTNGraphNode_Task.generated.h"

/**
 * Graph node representation of an HTN Task.
 * This node visually represents a task in the HTN graph editor.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_Task : public UHTNGraphNode
{
	GENERATED_BODY()

public:
	UHTNGraphNode_Task();

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	// UHTNGraphNode interface
	virtual UHTNTask* GetHTNTask() const override;
	virtual void SetHTNTask(UHTNTask* InTask);
	virtual UHTNTask* RecreateTask() override;
	virtual void UpdateFromTask(UHTNTask* InTask) override;
	virtual FLinearColor GetBackgroundColor() const override;
	// End of UHTNGraphNode interface

	/** Create input pins specific to this node */
	virtual void CreateInputPins() override;

	/** Create output pins specific to this node */
	virtual void CreateOutputPins() override;

protected:
	/** The task reference represented by this node */
	UPROPERTY(Instanced)
	UHTNTask* Task;
};