// Copyright Epic Games, Inc. All Rights Reserved.


#include "Conditions/HTNComparisonCondition.h"

#include "HTNWorldStateStruct.h"
#include "HTNLogging.h"

UHTNComparisonCondition::UHTNComparisonCondition()
    : Super()
    , LeftPropertyKey(NAME_None)
    , ComparisonType(EHTNComparisonType::GreaterThan)
    , bUseFixedRightValue(true)
    , RightPropertyKey(NAME_None)
    , FixedRightValue(0.0f)
    , ApproximateTolerance(0.001f)
{
    DebugColor = FLinearColor(0.1f, 0.7f, 0.9f); // Light blue
}

bool UHTNComparisonCondition::CheckCondition_Implementation(const UHTNWorldState* WorldState) const
{
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: World state is null"));
        return false;
    }

    // Get the left-hand property
    FHTNProperty LeftValue;
    if (!WorldState->GetProperty(LeftPropertyKey, LeftValue))
    {
        UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("ComparisonCondition: Left property %s not found"), *LeftPropertyKey.ToString());
        return false;
    }

    // Convert left value to float
    float LeftFloat = 0.0f;
    switch (LeftValue.GetType())
    {
        case EHTNPropertyType::Boolean:
            LeftFloat = LeftValue.GetBoolValue() ? 1.0f : 0.0f;
            break;
        case EHTNPropertyType::Integer:
            LeftFloat = static_cast<float>(LeftValue.GetIntValue());
            break;
        case EHTNPropertyType::Float:
            LeftFloat = LeftValue.GetFloatValue();
            break;
        default:
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: Left property %s is not numeric"), *LeftPropertyKey.ToString());
            return false;
    }

    // Get the right-hand value
    float RightFloat = 0.0f;
    if (bUseFixedRightValue)
    {
        RightFloat = FixedRightValue;
    }
    else
    {
        // Get the right property
        FHTNProperty RightValue;
        if (!WorldState->GetProperty(RightPropertyKey, RightValue))
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("ComparisonCondition: Right property %s not found"), *RightPropertyKey.ToString());
            return false;
        }

        // Convert right value to float
        switch (RightValue.GetType())
        {
            case EHTNPropertyType::Boolean:
                RightFloat = RightValue.GetBoolValue() ? 1.0f : 0.0f;
                break;
            case EHTNPropertyType::Integer:
                RightFloat = static_cast<float>(RightValue.GetIntValue());
                break;
            case EHTNPropertyType::Float:
                RightFloat = RightValue.GetFloatValue();
                break;
            default:
                UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: Right property %s is not numeric"), *RightPropertyKey.ToString());
                return false;
        }
    }

    // Perform the comparison
    switch (ComparisonType)
    {
        case EHTNComparisonType::LessThan:
            return LeftFloat < RightFloat;

        case EHTNComparisonType::LessThanOrEqual:
            return LeftFloat <= RightFloat;

        case EHTNComparisonType::GreaterThan:
            return LeftFloat > RightFloat;

        case EHTNComparisonType::GreaterThanOrEqual:
            return LeftFloat >= RightFloat;

        case EHTNComparisonType::ApproximatelyEqual:
            return FMath::Abs(LeftFloat - RightFloat) <= ApproximateTolerance;

        default:
            UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: Unknown comparison type %d"), 
                   static_cast<int32>(ComparisonType));
            return false;
    }
}

FString UHTNComparisonCondition::GetDescription_Implementation() const
{
    // Get the left operand description
    FString LeftDesc = LeftPropertyKey.ToString();
    
    // Get the comparison operator
    FString CompareDesc;
    switch (ComparisonType)
    {
        case EHTNComparisonType::LessThan:
            CompareDesc = TEXT("<");
            break;
        case EHTNComparisonType::LessThanOrEqual:
            CompareDesc = TEXT("<=");
            break;
        case EHTNComparisonType::GreaterThan:
            CompareDesc = TEXT(">");
            break;
        case EHTNComparisonType::GreaterThanOrEqual:
            CompareDesc = TEXT(">=");
            break;
        case EHTNComparisonType::ApproximatelyEqual:
            CompareDesc = TEXT("~=");
            break;
        default:
            CompareDesc = TEXT("??");
            break;
    }
    
    // Get the right operand description
    FString RightDesc;
    if (bUseFixedRightValue)
    {
        RightDesc = FString::SanitizeFloat(FixedRightValue);
    }
    else
    {
        RightDesc = RightPropertyKey.ToString();
    }
    
    return FString::Printf(TEXT("Comparison: %s %s %s"), *LeftDesc, *CompareDesc, *RightDesc);
}

bool UHTNComparisonCondition::ValidateCondition_Implementation() const
{
    // Check if base condition is valid
    if (!Super::ValidateCondition_Implementation())
    {
        return false;
    }
    
    // Check if the left property key is valid
    if (LeftPropertyKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: Left property key is none"));
        return false;
    }
    
    // Check if the right property key is valid when not using fixed value
    if (!bUseFixedRightValue && RightPropertyKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: Right property key is none"));
        return false;
    }
    
    // Check if tolerance is valid for approximate equality
    if (ComparisonType == EHTNComparisonType::ApproximatelyEqual && ApproximateTolerance <= 0.0f)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ComparisonCondition: Tolerance must be positive for approximate equality"));
        return false;
    }
    
    return true;
}