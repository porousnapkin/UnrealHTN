// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode_Task.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "HTNGraphNode_PrimitiveTask.generated.h"

/**
 * Graph node representation of an HTN Primitive Task.
 * Primitive tasks are the leaf nodes in the HTN hierarchy and represent
 * directly executable actions.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_PrimitiveTask : public UHTNGraphNode_Task
{
	GENERATED_BODY()

public:
	UHTNGraphNode_PrimitiveTask();

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface

	// UHTNGraphNode_Task interface
	virtual UHTNTask* RecreateTask() override;
	virtual void UpdateFromTask(UHTNTask* InTask) override;
	virtual void CreateOutputPins() override;
	// End of UHTNGraphNode_Task interface

	/** Get the associated primitive task */
	UHTNPrimitiveTask* GetPrimitiveTask() const;

	/** Set the primitive task for this node */
	void SetPrimitiveTask(UHTNPrimitiveTask* InPrimitiveTask);

protected:
	/** Create the precondition and effect pins for primitive tasks */
	void CreatePreconditionPins();
	void CreateEffectPins();
};