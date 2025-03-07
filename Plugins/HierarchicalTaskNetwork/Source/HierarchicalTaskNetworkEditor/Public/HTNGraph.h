// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "HTNGraph.generated.h"

/**
 * Graph representing an HTN (Hierarchical Task Network) plan.
 * This is the main graph class used for the visual representation of HTN plans.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraph : public UEdGraph
{
	GENERATED_BODY()

public:
	UHTNGraph();

	/** The HTN Plan Asset that owns this graph */
	UPROPERTY()
	class UHTNPlanAsset* HTNPlanAsset;
	
	/** Initialize the graph with the specified plan asset */
	void Initialize(class UHTNPlanAsset* InHTNPlanAsset);
	
	/** Clear the graph's contents */
	void ClearGraph();
};