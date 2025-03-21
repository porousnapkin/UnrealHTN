// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EdGraph/EdGraphSchema.h"

/**
 * Base action for creating HTN Graph nodes.
 */
struct FHTNGraphSchemaAction : public FEdGraphSchemaAction
{
public:
    FHTNGraphSchemaAction() 
        : FEdGraphSchemaAction()
    {}

    FHTNGraphSchemaAction(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
        : FEdGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
    {}

    // FEdGraphSchemaAction interface
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override
    {
        // Default implementation returns nullptr
        return nullptr;
    }
};

/**
 * Action for creating comment nodes in HTN Graph.
 */
struct FHTNGraphSchemaAction_NewComment : public FHTNGraphSchemaAction
{
public:
    FHTNGraphSchemaAction_NewComment() 
        : FHTNGraphSchemaAction()
    {}

    FHTNGraphSchemaAction_NewComment(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping)
        : FHTNGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
    {}

    // FEdGraphSchemaAction interface
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

/**
 * Action for creating task nodes in HTN Graph.
 */
struct FHTNGraphSchemaAction_NewNode : public FHTNGraphSchemaAction
{
public:
    FHTNGraphSchemaAction_NewNode() 
        : FHTNGraphSchemaAction()
        , NodeClass(nullptr)
        , TaskClass(nullptr)
    {}

    FHTNGraphSchemaAction_NewNode(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping, TSubclassOf<UEdGraphNode> InNodeClass, TSubclassOf<class UObject> InTaskClass = nullptr)
        : FHTNGraphSchemaAction(InNodeCategory, InMenuDesc, InToolTip, InGrouping)
        , NodeClass(InNodeClass)
        , TaskClass(InTaskClass)
    {}

    // FEdGraphSchemaAction interface
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;

protected:
    /** The class of node to create */
    TSubclassOf<UEdGraphNode> NodeClass;
    
    /** The class of task/object to create for the node */
    TSubclassOf<UObject> TaskClass;
};

/**
 * Specialized action for creating condition nodes with type selection.
 */
struct FHTNGraphSchemaAction_NewCondition : public FHTNGraphSchemaAction_NewNode
{
public:
    FHTNGraphSchemaAction_NewCondition() 
        : FHTNGraphSchemaAction_NewNode()
    {}

    FHTNGraphSchemaAction_NewCondition(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping, TSubclassOf<UEdGraphNode> InNodeClass, TSubclassOf<class UObject> InTaskClass = nullptr)
        : FHTNGraphSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InNodeClass, InTaskClass)
    {}

    // Override to provide specialized condition node creation
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};

/**
 * Specialized action for creating effect nodes with type selection.
 */
struct FHTNGraphSchemaAction_NewEffect : public FHTNGraphSchemaAction_NewNode
{
public:
    FHTNGraphSchemaAction_NewEffect() 
        : FHTNGraphSchemaAction_NewNode()
    {}

    FHTNGraphSchemaAction_NewEffect(const FText& InNodeCategory, const FText& InMenuDesc, const FText& InToolTip, int32 InGrouping, TSubclassOf<UEdGraphNode> InNodeClass, TSubclassOf<class UObject> InTaskClass = nullptr)
        : FHTNGraphSchemaAction_NewNode(InNodeCategory, InMenuDesc, InToolTip, InGrouping, InNodeClass, InTaskClass)
    {}

    // Override to provide specialized effect node creation
    virtual UEdGraphNode* PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode = true) override;
};