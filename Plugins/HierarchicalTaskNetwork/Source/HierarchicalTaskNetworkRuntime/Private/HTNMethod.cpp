// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNMethod.h"
#include "HTNTask.h"
#include "HTNTaskInterface.h"

UHTNMethod::UHTNMethod()
    : Priority(1.0f)
{
    // Generate a default name if none exists
    if (MethodName.IsNone())
    {
        MethodName = FName(*FString::Printf(TEXT("Method_%s"), *FGuid::NewGuid().ToString()));
    }
}

bool UHTNMethod::IsApplicable_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState) const
{
    // Check all conditions
    for (const UHTNCondition* Condition : Conditions)
    {
        if (Condition && !Condition->CheckCondition(WorldState))
        {
            // If any condition fails, this method is not applicable
            return false;
        }
    }
    
    // If there are no conditions or all conditions pass, the method is applicable
    return true;
}

FString UHTNMethod::GetDescription_Implementation() const
{
    // Use the custom description if provided, otherwise use the method name
    return !Description.IsEmpty() ? Description : MethodName.ToString();
}

bool UHTNMethod::ValidateMethod_Implementation() const
{
    // Check if the method has a valid name
    if (MethodName.IsNone())
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Method has no name"));
        return false;
    }
    
    // Check if all conditions are valid
    for (const UHTNCondition* Condition : Conditions)
    {
        if (!Condition)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Method %s has null condition"), *MethodName.ToString());
            return false;
        }
    }
    
    // Check if the method has at least one subtask
    if (Subtasks.Num() == 0)
    {
        UE_LOG(LogHTNTask, Warning, TEXT("Method %s has no subtasks"), *MethodName.ToString());
        return false;
    }
    
    // Check if all subtasks are valid
    for (const UHTNTask* Subtask : Subtasks)
    {
        if (!Subtask)
        {
            UE_LOG(LogHTNTask, Warning, TEXT("Method %s has null subtask"), *MethodName.ToString());
            return false;
        }
    }
    
    return true;
}