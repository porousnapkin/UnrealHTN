// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HTNPlanAsset.h"
#include "HTNGraphData.generated.h"

/**
 * Editor-only class that stores graph data for HTN Plan Assets.
 * This class bridges the gap between the visual graph representation in the editor
 * and the runtime plan asset.
 */
UCLASS()
class HIERARCHICALTASKNETWORKEDITOR_API UHTNGraphData : public UObject
{
    GENERATED_BODY()

public:
    UHTNGraphData();

    /**
     * Initialize with a reference to a plan asset
     * @param InPlanAsset - The plan asset to associate with this graph data
     */
    void Initialize(UHTNPlanAsset* InPlanAsset);

    /**
     * Get the associated plan asset
     * @return The associated plan asset
     */
    UHTNPlanAsset* GetPlanAsset() const { return PlanAsset; }

    /**
     * Get the graph that represents this plan
     * @return The graph object
     */
    class UHTNGraph* GetGraph() const { return Graph; }

    /**
     * Create or update the graph based on the plan asset
     * @return True if successful
     */
    bool UpdateGraphFromAsset();

    /**
     * Compile the graph into the plan asset
     * @return True if compilation was successful
     */
    bool CompileGraphToAsset();

    /**
     * Validate the graph for errors
     * @param OutErrors - Array to hold any validation errors
     * @return True if the graph is valid (no errors)
     */
    bool ValidateGraph(TArray<FString>& OutErrors) const;

    /**
     * Track debug mode for editor.
     */
    bool bIsInDebugMode = false;

private:
    /** The associated plan asset */
    UPROPERTY()
    UHTNPlanAsset* PlanAsset;

    /** The graph representing the plan */
    UPROPERTY()
    class UHTNGraph* Graph;

    /**
     * Create a new graph for the plan asset
     * @return True if successful
     */
    bool CreateNewGraph();

    /**
     * Find or create a root node in the graph
     * @return The root node
     */
    class UHTNGraphNode_Root* FindOrCreateRootNode();
};