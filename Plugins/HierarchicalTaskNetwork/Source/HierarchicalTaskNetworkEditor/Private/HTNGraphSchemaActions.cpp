#include "HTNGraphSchemaActions.h"
#include "Conditions/HTNPropertyCondition.h"
#include "Effects/HTNSetPropertyEffect.h"
#include "EdGraphNode_Comment.h"
#include "EdGraph/EdGraph.h"
#include "EdGraph/EdGraphNode.h"
#include "HTNGraphNode.h"
#include "HTNGraphNode_Method.h"
#include "HTNGraphNode_Condition.h"
#include "HTNGraphNode_Effect.h"
#include "HTNGraphNode_CompoundTask.h"
#include "HTNGraphNode_PrimitiveTask.h"
#include "Tasks/HTNTask.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "Tasks/HTNCompoundTask.h"
#include "Tasks/HTNTaskFactory.h"
#include "Conditions/HTNCondition.h"
#include "Effects/HTNEffect.h"
#include "HTNEditorLogging.h"

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
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Cannot create node: NodeClass is null"));
        return nullptr;
    }
    
    // Create the node
    UEdGraphNode* NewNode = NewObject<UEdGraphNode>(ParentGraph, NodeClass);
    if (!NewNode)
    {
        UE_LOG(LogHTNPlannerEditorPlugin, Error, TEXT("Failed to create node of class: %s"), *NodeClass->GetName());
        return nullptr;
    }
    
    // Set node location
    NewNode->NodePosX = Location.X;
    NewNode->NodePosY = Location.Y;
    
    // Create the task if we have a valid task class
    if (TaskClass && NewNode->IsA<UHTNGraphNode>())
    {
        UHTNGraphNode* HTNNode = Cast<UHTNGraphNode>(NewNode);
        
        // Create the appropriate task or object based on node type
        UObject* NewlyCreatedObject = nullptr;
        FName DefaultName = FName("NewTask");
        
        if (UHTNGraphNode_PrimitiveTask* PrimitiveNode = Cast<UHTNGraphNode_PrimitiveTask>(HTNNode))
        {
            // Create primitive task
            UHTNPrimitiveTask* NewTask = UHTNTaskFactory::Get()->CreatePrimitiveTask(
                TaskClass->IsChildOf(UHTNPrimitiveTask::StaticClass()) ? 
                    Cast<UClass>(TaskClass) : UHTNPrimitiveTask::StaticClass(), 
                PrimitiveNode, DefaultName);
            
            // Update the node from the task
            if (NewTask)
            {
                PrimitiveNode->SetPrimitiveTask(NewTask);
            }
        }
        else if (UHTNGraphNode_CompoundTask* CompoundNode = Cast<UHTNGraphNode_CompoundTask>(HTNNode))
        {
            // Create compound task
            UHTNCompoundTask* NewTask = UHTNTaskFactory::Get()->CreateCompoundTask(
                TaskClass->IsChildOf(UHTNCompoundTask::StaticClass()) ? 
                    Cast<UClass>(TaskClass) : UHTNCompoundTask::StaticClass(), 
                CompoundNode, DefaultName);
            
            // Update the node from the task
            if (NewTask)
            {
                CompoundNode->SetCompoundTask(NewTask);
            }
        }
        else if (UHTNGraphNode_Method* MethodNode = Cast<UHTNGraphNode_Method>(HTNNode))
        {
            // Create a default method
            UHTNMethod* NewMethod = MethodNode->RecreateMethod();
            if (NewMethod)
            {
                MethodNode->SetMethod(NewMethod);
            }
        }
        else if (UHTNGraphNode_Condition* ConditionNode = Cast<UHTNGraphNode_Condition>(HTNNode))
        {
            // Create a condition
            UHTNCondition* NewCondition = NewObject<UHTNCondition>(ConditionNode, 
                TaskClass->IsChildOf(UHTNCondition::StaticClass()) ? 
                    Cast<UClass>(TaskClass) : UHTNCondition::StaticClass(), 
                NAME_None, RF_Transactional);
            
            if (NewCondition)
            {
                ConditionNode->SetCondition(NewCondition);
            }
        }
        else if (UHTNGraphNode_Effect* EffectNode = Cast<UHTNGraphNode_Effect>(HTNNode))
        {
            // Create an effect
            UHTNEffect* NewEffect = NewObject<UHTNEffect>(EffectNode, 
                TaskClass->IsChildOf(UHTNEffect::StaticClass()) ? 
                    Cast<UClass>(TaskClass) : UHTNEffect::StaticClass(), 
                NAME_None, RF_Transactional);
            
            if (NewEffect)
            {
                EffectNode->SetEffect(NewEffect);
            }
        }
        else
        {
            // For generic HTN node, create the base task
            UHTNTask* NewTask = Cast<UHTNTask>(UHTNTaskFactory::Get()->CreateTask(
                TSubclassOf<UHTNTask>(), 
                HTNNode, DefaultName));
            
            // Update the node from the task
            if (NewTask)
            {
                HTNNode->UpdateFromTask(NewTask);
            }
        }
    }
    
    // Add node to graph
    ParentGraph->AddNode(NewNode, true, bSelectNewNode);
    
    // Allocate default pins for the new node
    NewNode->AllocateDefaultPins();
    
    // Auto-wire if we have a source pin
    if (FromPin)
    {
        NewNode->AutowireNewNode(FromPin);
    }
    
    return NewNode;
}

// Specialized node creation for selecting condition types
UEdGraphNode* FHTNGraphSchemaAction_NewCondition::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
    // First create the condition node
    UHTNGraphNode_Condition* ConditionNode = Cast<UHTNGraphNode_Condition>(
        FHTNGraphSchemaAction_NewNode::PerformAction(ParentGraph, FromPin, Location, bSelectNewNode));
    
    if (!ConditionNode)
    {
        return nullptr;
    }
    
    // Here we'd show a dialog to select the condition type
    // For now, we'll just create a property condition as default
    UHTNPropertyCondition* PropertyCondition = NewObject<UHTNPropertyCondition>(ConditionNode, UHTNPropertyCondition::StaticClass(), NAME_None, RF_Transactional);
    if (PropertyCondition)
    {
        // Set some default values
        PropertyCondition->PropertyKey = FName("NewProperty");
        PropertyCondition->CheckType = EHTNPropertyCheckType::Exists;
        
        // Assign the condition to the node
        ConditionNode->SetCondition(PropertyCondition);
        ConditionNode->ReconstructNode();
    }
    
    return ConditionNode;
}

// Specialized node creation for selecting effect types
UEdGraphNode* FHTNGraphSchemaAction_NewEffect::PerformAction(class UEdGraph* ParentGraph, UEdGraphPin* FromPin, const FVector2D Location, bool bSelectNewNode)
{
    // First create the effect node
    UHTNGraphNode_Effect* EffectNode = Cast<UHTNGraphNode_Effect>(
        FHTNGraphSchemaAction_NewNode::PerformAction(ParentGraph, FromPin, Location, bSelectNewNode));
    
    if (!EffectNode)
    {
        return nullptr;
    }
    
    // Here we'd show a dialog to select the effect type
    // For now, we'll just create a set property effect as default
    UHTNSetPropertyEffect* SetPropertyEffect = NewObject<UHTNSetPropertyEffect>(EffectNode, UHTNSetPropertyEffect::StaticClass(), NAME_None, RF_Transactional);
    if (SetPropertyEffect)
    {
        // Set some default values
        SetPropertyEffect->PropertyKey = FName("NewProperty");
        SetPropertyEffect->PropertyValue = FHTNProperty(true); // Default to boolean true
        
        // Assign the effect to the node
        EffectNode->SetEffect(SetPropertyEffect);
        EffectNode->ReconstructNode();
    }
    
    return EffectNode;
}