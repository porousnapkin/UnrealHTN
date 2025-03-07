// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode.h"
#include "HTNMethod.h"
#include "HTNGraphNode_Method.generated.h"

/**
 * Graph node representation of an HTN Method.
 * This node visually represents a decomposition method in the HTN graph editor.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_Method : public UHTNGraphNode
{
	GENERATED_BODY()

public:
	UHTNGraphNode_Method();

	// UEdGraphNode interface
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	// End of UEdGraphNode interface

	// UHTNGraphNode interface
	virtual FLinearColor GetBackgroundColor() const override;
	// End of UHTNGraphNode interface

	/** Get the Method represented by this node */
	UHTNMethod* GetMethod() const { return Method; }

	/** Set the Method represented by this node */
	void SetMethod(UHTNMethod* InMethod);

	/** Recreate the Method if needed */
	UHTNMethod* RecreateMethod();

	/** Update this node from the provided Method */
	void UpdateFromMethod(UHTNMethod* InMethod);

protected:
	/** Create input pins specific to this node */
	virtual void CreateInputPins() override;

	/** Create output pins specific to this node */
	virtual void CreateOutputPins() override;

	/** The Method reference represented by this node */
	UPROPERTY(Instanced)
	UHTNMethod* Method;
};