// Copyright Epic Games, Inc. All Rights Reserved.

#include "Effects/HTNSetPropertyEffect.h"

#include "HTNWorldStateStruct.h"
#include "HTNLogging.h"

UHTNSetPropertyEffect::UHTNSetPropertyEffect()
    : Super()
    , PropertyKey(NAME_None)
    , bUseSourceProperty(false)
    , SourcePropertyKey(NAME_None)
    , bRemoveProperty(false)
{
    DebugColor = FLinearColor(0.0f, 0.8f, 0.2f); // Green
}

void UHTNSetPropertyEffect::ApplyEffect_Implementation(UHTNWorldState* WorldState) const
{
    if (!WorldState)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("SetPropertyEffect: World state is null"));
        return;
    }

    // If we're removing the property, just do that and return
    if (bRemoveProperty)
    {
        WorldState->RemoveProperty(PropertyKey);
        return;
    }

    // If we're using a source property, get its value
    if (bUseSourceProperty)
    {
        FHTNProperty SourceValue;
        if (!WorldState->GetProperty(SourcePropertyKey, SourceValue))
        {
            UE_LOG(LogHTNPlannerPlugin, Verbose, TEXT("SetPropertyEffect: Source property %s not found"), *SourcePropertyKey.ToString());
            return;
        }

        // Set the property to the source value
        WorldState->SetProperty(PropertyKey, SourceValue);
    }
    else
    {
        // Set the property to the specified value
        WorldState->SetProperty(PropertyKey, PropertyValue);
    }
}

FString UHTNSetPropertyEffect::GetDescription_Implementation() const
{
    if (bRemoveProperty)
    {
        return FString::Printf(TEXT("Remove property: %s"), *PropertyKey.ToString());
    }
    else if (bUseSourceProperty)
    {
        return FString::Printf(TEXT("Set %s = %s (from property)"), *PropertyKey.ToString(), *SourcePropertyKey.ToString());
    }
    else
    {
        return FString::Printf(TEXT("Set %s = %s"), *PropertyKey.ToString(), *PropertyValue.ToString());
    }
}

bool UHTNSetPropertyEffect::ValidateEffect_Implementation() const
{
    // Check if base effect is valid
    if (!Super::ValidateEffect_Implementation())
    {
        return false;
    }
    
    // Check if property key is valid
    if (PropertyKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("SetPropertyEffect: Property key is none"));
        return false;
    }
    
    // If using source property, check if source key is valid
    if (bUseSourceProperty && SourcePropertyKey.IsNone())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("SetPropertyEffect: Source property key is none"));
        return false;
    }
    
    // If not using source property and not removing, check if property value is valid
    if (!bUseSourceProperty && !bRemoveProperty && !PropertyValue.IsValid())
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("SetPropertyEffect: Property value is invalid"));
        return false;
    }
    
    return true;
}