// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNGraphNode.h"
#include "Effects/HTNEffect.h"
#include "HTNGraphNode_Effect.generated.h"

/**
 * Graph node representing an effect in an HTN plan.
 * Effects represent changes to the world state that occur when tasks are executed.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphNode_Effect : public UHTNGraphNode
{
	GENERATED_BODY()

public:
	UHTNGraphNode_Effect();

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

	/** Get the associated effect */
	UFUNCTION(BlueprintCallable, Category = "HTN|Effect")
	UHTNEffect* GetEffect() const;

	/** Set the associated effect */
	UFUNCTION(BlueprintCallable, Category = "HTN|Effect")
	void SetEffect(UHTNEffect* InEffect);

	/** Update the node based on changes to the effect */
	void UpdateFromEffect(UHTNEffect* InEffect);

	/** Create input pins for the node */
	virtual void CreateInputPins();

	/** Create output pins for the node */
	virtual void CreateOutputPins();

	/** Reconstruct this node based on effect changes */
	virtual void ReconstructNode();

protected:
	/** The effect represented by this node */
	UPROPERTY(EditAnywhere, Instanced, Category = "HTN")
	UHTNEffect* Effect;
};