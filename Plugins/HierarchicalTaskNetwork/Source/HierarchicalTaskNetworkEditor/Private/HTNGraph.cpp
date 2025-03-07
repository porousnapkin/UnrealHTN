// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraph.h"
#include "HTNPlanAsset.h"
#include "HTNGraphSchema.h"

UHTNGraph::UHTNGraph()
{
	// Set the default schema for this graph to our custom schema
	Schema = UHTNGraphSchema::StaticClass();
}

void UHTNGraph::Initialize(UHTNPlanAsset* InHTNPlanAsset)
{
	HTNPlanAsset = InHTNPlanAsset;
	
	// Ensure the schema is correctly set
	if (!Schema)
	{
		Schema = UHTNGraphSchema::StaticClass();
	}
	
	// Set the graph's outer to the plan asset for proper serialization
	SetFlags(RF_Transactional);
	Rename(nullptr, InHTNPlanAsset, REN_ForceNoResetLoaders);
}

void UHTNGraph::ClearGraph()
{
	// Remove all nodes and connections
	Nodes.Empty();
	
	// Notify any listeners that the graph has changed
	NotifyGraphChanged();
}