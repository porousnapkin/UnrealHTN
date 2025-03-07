// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphData.h"

#include "HTNEditorLogging.h"
#include "HTNGraph.h"
#include "HTNGraphNode_Root.h"
#include "HTNGraphNode_Task.h"
#include "HTNGraphNode_Method.h"
#include "HTNGraphNode_CompoundTask.h"
#include "HTNPlanAsset.h"

UHTNGraphData::UHTNGraphData()
    : PlanAsset(nullptr)
    , Graph(nullptr)
{
}

void UHTNGraphData::Initialize(UHTNPlanAsset* InPlanAsset)
{
    PlanAsset = InPlanAsset;
    
    // Create a new graph if needed
    if (!Graph && PlanAsset)
    {
        CreateNewGraph();
    }
}

bool UHTNGraphData::UpdateGraphFromAsset()
{
    if (!PlanAsset)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot update graph: No plan asset set"));
        return false;
    }
    
    // Create a new graph if needed
    if (!Graph)
    {
        if (!CreateNewGraph())
        {
            return false;
        }
    }
    
    // Clear the existing graph
    Graph->ClearGraph();
    
    // Create a root node
    UHTNGraphNode_Root* RootNode = FindOrCreateRootNode();
    if (!RootNode)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Failed to create root node"));
        return false;
    }
    
    // TODO: Populate the graph based on the plan
    // This is a placeholder for future implementation when we have
    // the ability to convert runtime plans back to graph representations
    
    return true;
}

bool UHTNGraphData::CompileGraphToAsset()
{
    if (!PlanAsset || !Graph)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot compile graph: Missing plan asset or graph"));
        return false;
    }
    
    // Validate the graph first
    TArray<FString> ValidationErrors;
    if (!ValidateGraph(ValidationErrors))
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Graph validation failed during compilation"));
        for (const FString& Error : ValidationErrors)
        {
            UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("  %s"), *Error);
        }
        return false;
    }
    
    // TODO: Implement graph compilation to FHTNPlan
    // This will convert the visual graph into a runtime plan object
    
    // For now, we'll create an empty plan
    FHTNPlan CompiledPlan;
    
    // Set the plan data in the asset
    PlanAsset->Plan = CompiledPlan;
    
    // Update the modified time
    PlanAsset->UpdateModifiedTime();
    
    return true;
}

bool UHTNGraphData::ValidateGraph(TArray<FString>& OutErrors) const
{
    if (!Graph)
    {
        OutErrors.Add(TEXT("No graph exists"));
        return false;
    }
    
    bool bIsValid = true;
    
    // Check for a root node
    UHTNGraphNode_Root* RootNode = nullptr;
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        if (UHTNGraphNode_Root* TestRootNode = Cast<UHTNGraphNode_Root>(Node))
        {
            if (RootNode)
            {
                OutErrors.Add(TEXT("Multiple root nodes found"));
                bIsValid = false;
            }
            else
            {
                RootNode = TestRootNode;
            }
        }
    }
    
    if (!RootNode)
    {
        OutErrors.Add(TEXT("No root node found"));
        bIsValid = false;
    }
    
    // Validate each node in the graph
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        // Check for null pins
        for (UEdGraphPin* Pin : Node->Pins)
        {
            if (!Pin)
            {
                OutErrors.Add(FString::Printf(TEXT("Node '%s' has a null pin"), *Node->GetNodeTitle(ENodeTitleType::FullTitle).ToString()));
                bIsValid = false;
                break;
            }
        }
        
        // Validate specific node types
        if (UHTNGraphNode* HTNNode = Cast<UHTNGraphNode>(Node))
        {
            // Check if the node has a valid task (for task nodes)
            if (UHTNGraphNode_Task* TaskNode = Cast<UHTNGraphNode_Task>(HTNNode))
            {
                if (!TaskNode->GetHTNTask())
                {
                    OutErrors.Add(FString::Printf(TEXT("Task node '%s' has no associated task"), 
                        *TaskNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString()));
                    bIsValid = false;
                }
            }
            
            // Method nodes should have valid methods
            if (UHTNGraphNode_Method* MethodNode = Cast<UHTNGraphNode_Method>(HTNNode))
            {
                if (!MethodNode->GetMethod())
                {
                    OutErrors.Add(FString::Printf(TEXT("Method node '%s' has no associated method"), 
                        *MethodNode->GetNodeTitle(ENodeTitleType::FullTitle).ToString()));
                    bIsValid = false;
                }
            }
        }
    }
    
    return bIsValid;
}

bool UHTNGraphData::CreateNewGraph()
{
    if (!PlanAsset)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot create graph: No plan asset set"));
        return false;
    }
    
    // Create a new graph
    Graph = NewObject<UHTNGraph>(this, UHTNGraph::StaticClass(), NAME_None, RF_Transactional);
    if (!Graph)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Failed to create HTN graph"));
        return false;
    }
    
    // Initialize the graph
    Graph->Initialize(PlanAsset);
    
    // Create a root node
    FindOrCreateRootNode();
    
    return true;
}

UHTNGraphNode_Root* UHTNGraphData::FindOrCreateRootNode()
{
    if (!Graph)
    {
        return nullptr;
    }
    
    // Check if a root node already exists
    for (UEdGraphNode* Node : Graph->Nodes)
    {
        if (UHTNGraphNode_Root* RootNode = Cast<UHTNGraphNode_Root>(Node))
        {
            return RootNode;
        }
    }
    
    // Create a new root node
    UHTNGraphNode_Root* RootNode = NewObject<UHTNGraphNode_Root>(Graph, UHTNGraphNode_Root::StaticClass(), NAME_None, RF_Transactional);
    if (!RootNode)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Failed to create root node"));
        return nullptr;
    }
    
    // Set up the root node
    RootNode->SetPlanAsset(PlanAsset);
    Graph->AddNode(RootNode, /*bUserAction=*/false, /*bSelectNewNode=*/false);
    
    // Position the root node
    RootNode->NodePosX = 0;
    RootNode->NodePosY = 0;
    
    // Create the pins
    RootNode->AllocateDefaultPins();
    
    return RootNode;
}