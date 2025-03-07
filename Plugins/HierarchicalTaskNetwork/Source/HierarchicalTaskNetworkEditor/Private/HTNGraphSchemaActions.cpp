// Fill out your copyright notice in the Description page of Project Settings.

#include "HTNGraphSchemaActions.h"

#include "EdGraphNode_Comment.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "HTNGraphNode.h"
#include "Tasks/HTNTask.h"
#include "Tasks/HTNTaskFactory.h"

UEdGraphNode* FHTNGraphSchemaAction_NewComment::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	// Create a comment node
	UEdGraphNode_Comment* CommentNode = NewObject<UEdGraphNode_Comment>(ParentGraph);
	
	// Set node location
	CommentNode->NodePosX = Location.X;
	CommentNode->NodePosY = Location.Y;
	
	// Set default properties
	CommentNode->NodeWidth = 400.0f;
	CommentNode->NodeHeight = 200.0f;
	CommentNode->NodeComment = TEXT("Comment");
	
	// Add node to graph
	ParentGraph->AddNode(CommentNode, true, bSelectNewNode);
	
	return CommentNode;
}

UEdGraphNode* FHTNGraphSchemaAction_NewNode::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
	// Check if the node class is valid
	if (!NodeClass)
	{
		return nullptr;
	}
	
	// Create the node
	UEdGraphNode* NewNode = NewObject<UEdGraphNode>(ParentGraph, NodeClass);
	
	// Create the task if we have a valid task class
	if (TaskClass && NewNode->IsA<UHTNGraphNode>())
	{
		UHTNGraphNode* HTNNode = Cast<UHTNGraphNode>(NewNode);
		
		// Create the task for this node
		UHTNTask* NewTask = UHTNTaskFactory::Get()->CreateTask(TaskClass, HTNNode, FName("Task"));
		
		// Update the node from the task
		if (NewTask)
		{
			HTNNode->UpdateFromTask(NewTask);
		}
	}
	
	// Set node location
	NewNode->NodePosX = Location.X;
	NewNode->NodePosY = Location.Y;
	
	// Add node to graph
	ParentGraph->AddNode(NewNode, true, bSelectNewNode);
	
	// Auto-wire if we have a source pin
	if (FromPin)
	{
		NewNode->AutowireNewNode(FromPin);
	}
	
	return NewNode;
}