// Copyright Epic Games, Inc. All Rights Reserved.

#include "Conditions/HTNPropertyCondition.h"

#include "HTNWorldStateStruct.h"
#include "HTNLogging.h"

UHTNPropertyCondition::UHTNPropertyCondition()
    : Super()
    , PropertyKey(NAME_None)
    , CheckType(EHTNPropertyCheckType::Exists)
{
    DebugColor = FLinearColor(1.0f, 0.8f, 0.0f); // Light yellow
}

bool UHTNPropertyCondition::CheckCondition_Implementation(const UHTNWorldState* WorldState) const
{
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PropertyCondition: World state is null"));
        return false;
    }

    // Get the property if it exists
    FHTNProperty PropertyValue;
    bool bPropertyExists = WorldState->GetProperty(PropertyKey, PropertyValue);

    // Check based on the check type
    switch (CheckType)
    {
        case EHTNPropertyCheckType::Exists:
            return bPropertyExists;

        case EHTNPropertyCheckType::NotExists:
            return !bPropertyExists;

        case EHTNPropertyCheckType::IsTrue:
            if (bPropertyExists && PropertyValue.GetType() == EHTNPropertyType::Boolean)
            {
                return PropertyValue.GetBoolValue();
            }
            return false;

        case EHTNPropertyCheckType::IsFalse:
            if (bPropertyExists && PropertyValue.GetType() == EHTNPropertyType::Boolean)
            {
                return !PropertyValue.GetBoolValue();
            }
            return false;

        case EHTNPropertyCheckType::Equals:
            if (bPropertyExists)
            {
                return PropertyValue == CompareValue;
            }
            return false;

        case EHTNPropertyCheckType::NotEquals:
            if (bPropertyExists)
            {
                return PropertyValue != CompareValue;
            }
            return true; // Non-existent properties are not equal to anything

        default:
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PropertyCondition: Unknown check type %d"), 
                   static_cast<int32>(CheckType));
            return false;
    }
}

FString UHTNPropertyCondition::GetDescription_Implementation() const
{
    FString CheckDescription;
    
    switch (CheckType)
    {
        case EHTNPropertyCheckType::Exists:
            CheckDescription = FString::Printf(TEXT("%s exists"), *PropertyKey.ToString());
            break;
        
        case EHTNPropertyCheckType::NotExists:
            CheckDescription = FString::Printf(TEXT("%s does not exist"), *PropertyKey.ToString());
            break;
        
        case EHTNPropertyCheckType::IsTrue:
            CheckDescription = FString::Printf(TEXT("%s is true"), *PropertyKey.ToString());
            break;
        
        case EHTNPropertyCheckType::IsFalse:
            CheckDescription = FString::Printf(TEXT("%s is false"), *PropertyKey.ToString());
            break;
        
        case EHTNPropertyCheckType::Equals:
            CheckDescription = FString::Printf(TEXT("%s equals %s"), *PropertyKey.ToString(), *CompareValue.ToString());
            break;
        
        case EHTNPropertyCheckType::NotEquals:
            CheckDescription = FString::Printf(TEXT("%s does not equal %s"), *PropertyKey.ToString(), *CompareValue.ToString());
            break;
        
        default:
            CheckDescription = TEXT("Unknown check");
            break;
    }
    
    return FString::Printf(TEXT("Property Condition: %s"), *CheckDescription);
}

bool UHTNPropertyCondition::ValidateCondition_Implementation() const
{
    // Check if base condition is valid
    if (!Super::ValidateCondition_Implementation())
    {
        return false;
    }
    
    // Check if property key is valid
    if (PropertyKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PropertyCondition: Property key is none"));
        return false;
    }
    
    // For equality checks, validate the compare value
    if ((CheckType == EHTNPropertyCheckType::Equals || CheckType == EHTNPropertyCheckType::NotEquals) &&
        !CompareValue.IsValid())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("PropertyCondition: Compare value is invalid for equality check"));
        return false;
    }
    
    // For boolean checks, no additional validation needed
    
    return true;
}