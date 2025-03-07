// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode_Task.h"
#include "Tasks/HTNCompoundTask.h"
#include "HTNGraphNode_CompoundTask.generated.h"

/**
 * Graph node representation of an HTN Compound Task.
 * Compound tasks require decomposition through methods before they can be executed.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_CompoundTask : public UHTNGraphNode_Task
{
	GENERATED_BODY()

public:
	UHTNGraphNode_CompoundTask();

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	// End of UEdGraphNode interface

	// UHTNGraphNode_Task interface
	virtual UHTNTask* RecreateTask() override;
	virtual void UpdateFromTask(UHTNTask* InTask) override;
	virtual void CreateOutputPins() override;
	// End of UHTNGraphNode_Task interface

	/** Get the associated compound task */
	UHTNCompoundTask* GetCompoundTask() const;

	/** Set the compound task for this node */
	void SetCompoundTask(UHTNCompoundTask* InCompoundTask);

	/** Get the associated methods for this compound task */
	TArray<class UHTNMethod*> GetMethods() const;

	/** Add a new method to this compound task */
	UHTNMethod* AddMethod();

	/** Remove a method from this compound task */
	bool RemoveMethod(UHTNMethod* Method);

protected:
	/** Create output pins specific to this compound task */
	void CreateMethodPins();
};