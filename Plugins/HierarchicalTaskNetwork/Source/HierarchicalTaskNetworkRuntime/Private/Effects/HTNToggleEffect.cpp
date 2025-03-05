// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/HTNToggleEffect.h"

#include "HTNWorldStateStruct.h"
#include "HTNLogging.h"

UHTNToggleEffect::UHTNToggleEffect()
    : Super()
    , PropertyKey(NAME_None)
    , bSetTrueIfMissing(true)
    , bForceValue(false)
    , ForcedValue(true)
{
    DebugColor = FLinearColor(0.6f, 0.8f, 0.1f); // Yellow-green
}

void UHTNToggleEffect::ApplyEffect_Implementation(UHTNWorldState* WorldState) const
{
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ToggleEffect: World state is null"));
        return;
    }

    // Get the current value if it exists
    FHTNProperty PropertyValue;
    bool bExists = WorldState->GetProperty(PropertyKey, PropertyValue);
    
    // If the property exists and is a boolean
    if (bExists && PropertyValue.GetType() == EHTNPropertyType::Boolean)
    {
        bool CurrentValue = PropertyValue.GetBoolValue();
        
        // If forcing a value, use that
        if (bForceValue)
        {
            WorldState->SetProperty(PropertyKey, FHTNProperty(ForcedValue));
        }
        else
        {
            // Otherwise toggle the current value
            WorldState->SetProperty(PropertyKey, FHTNProperty(!CurrentValue));
        }
    }
    else
    {
        // Property doesn't exist or isn't a boolean
        if (bForceValue)
        {
            // Use the forced value
            WorldState->SetProperty(PropertyKey, FHTNProperty(ForcedValue));
        }
        else if (bSetTrueIfMissing)
        {
            // Create with true value
            WorldState->SetProperty(PropertyKey, FHTNProperty(true));
        }
        else
        {
            // Create with false value
            WorldState->SetProperty(PropertyKey, FHTNProperty(false));
        }
    }
}

FString UHTNToggleEffect::GetDescription_Implementation() const
{
    if (bForceValue)
    {
        return FString::Printf(TEXT("Set %s = %s"), *PropertyKey.ToString(), ForcedValue ? TEXT("true") : TEXT("false"));
    }
    else
    {
        return FString::Printf(TEXT("Toggle %s"), *PropertyKey.ToString());
    }
}

bool UHTNToggleEffect::ValidateEffect_Implementation() const
{
    // Check if base effect is valid
    if (!Super::ValidateEffect_Implementation())
    {
        return false;
    }
    
    // Check if property key is valid
    if (PropertyKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("ToggleEffect: Property key is none"));
        return false;
    }
    
    return true;
}