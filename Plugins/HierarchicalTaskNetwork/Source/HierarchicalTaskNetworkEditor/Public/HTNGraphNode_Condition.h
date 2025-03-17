// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode.h"
#include "Conditions/HTNCondition.h"
#include "HTNGraphNode_Condition.generated.h"

/**
 * Graph node representing a condition in an HTN plan.
 * Conditions are used to check whether tasks are applicable in a given world state.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_Condition : public UHTNGraphNode
{
	GENERATED_BODY()

public:
	UHTNGraphNode_Condition();

	//~ Begin UEdGraphNode Interface
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetTooltipText() const override;
	virtual void PinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual FSlateIcon GetIconAndTint(FLinearColor& OutColor) const override;
	//~ End UEdGraphNode Interface

	//~ Begin UHTNGraphNode Interface
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	//~ End UHTNGraphNode Interface

	/** Get the associated condition */
	UFUNCTION(BlueprintCallable, Category = "HTN|Condition")
	UHTNCondition* GetCondition() const;

	/** Set the associated condition */
	UFUNCTION(BlueprintCallable, Category = "HTN|Condition")
	void SetCondition(UHTNCondition* InCondition);

	/** Update the node based on changes to the condition */
	void UpdateFromCondition(UHTNCondition* InCondition);

	/** Create input pins for the node */
	virtual void CreateInputPins();

	/** Create output pins for the node */
	virtual void CreateOutputPins();

	/** Reconstruct this node based on condition changes */
	virtual void ReconstructNode();
	

protected:
	/** The condition represented by this node */
	UPROPERTY(EditAnywhere, Instanced, Category = "HTN")
	UHTNCondition* Condition;
};
