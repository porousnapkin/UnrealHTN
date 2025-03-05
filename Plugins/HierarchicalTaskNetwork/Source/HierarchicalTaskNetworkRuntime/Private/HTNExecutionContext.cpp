// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNExecutionContext.h"
#include "HTNProperty.h"
#include "HTNLogging.h"

FHTNExecutionContext::FHTNExecutionContext()
    : OwnerActor(nullptr)
    , WorldState(nullptr)
{
}

FHTNExecutionContext::FHTNExecutionContext(AActor* InOwnerActor)
    : OwnerActor(InOwnerActor)
    , WorldState(nullptr)
{
}

FHTNExecutionContext::FHTNExecutionContext(const FHTNExecutionContext& Other)
    : OwnerActor(Other.OwnerActor)
    , Parameters(Other.Parameters)
{
    // Deep copy the world state if it exists
    if (Other.WorldState)
    {
        WorldState = Other.WorldState->Clone();
    }
    else
    {
        WorldState = nullptr;
    }
}

FHTNExecutionContext::FHTNExecutionContext(FHTNExecutionContext&& Other) noexcept
    : OwnerActor(Other.OwnerActor)
    , WorldState(Other.WorldState)
    , Parameters(MoveTemp(Other.Parameters))
{
    // Reset the moved-from object to a valid state
    Other.OwnerActor = nullptr;
    Other.WorldState = nullptr;
}

FHTNExecutionContext& FHTNExecutionContext::operator=(const FHTNExecutionContext& Other)
{
    if (this != &Other)
    {
        OwnerActor = Other.OwnerActor;
        Parameters = Other.Parameters;
        
        // Deep copy the world state
        if (WorldState)
        {
            // Clean up existing world state
            WorldState = nullptr;
        }
        
        if (Other.WorldState)
        {
            WorldState = Other.WorldState->Clone();
        }
    }
    return *this;
}

FHTNExecutionContext& FHTNExecutionContext::operator=(FHTNExecutionContext&& Other) noexcept
{
    if (this != &Other)
    {
        // Clean up existing world state if needed
        if (WorldState)
        {
            WorldState = nullptr;
        }
        
        // Move data from other context
        OwnerActor = Other.OwnerActor;
        WorldState = Other.WorldState;
        Parameters = MoveTemp(Other.Parameters);
        
        // Reset the moved-from object to a valid state
        Other.OwnerActor = nullptr;
        Other.WorldState = nullptr;
    }
    return *this;
}

void FHTNExecutionContext::SetOwner(AActor* InOwnerActor)
{
    OwnerActor = InOwnerActor;
}

void FHTNExecutionContext::SetWorldState(UHTNWorldState* InWorldState)
{
    WorldState = InWorldState;
}

bool FHTNExecutionContext::GetParameter(FName Key, FHTNProperty& OutValue) const
{
    if (const FHTNProperty* Property = Parameters.Find(Key))
    {
        OutValue = *Property;
        return true;
    }
    return false;
}

void FHTNExecutionContext::SetParameter(FName Key, const FHTNProperty& Value)
{
    Parameters.Add(Key, Value);
}

bool FHTNExecutionContext::HasParameter(FName Key) const
{
    return Parameters.Contains(Key);
}

bool FHTNExecutionContext::RemoveParameter(FName Key)
{
    return Parameters.Remove(Key) > 0;
}

TArray<FName> FHTNExecutionContext::GetParameterNames() const
{
    TArray<FName> Names;
    Parameters.GetKeys(Names);
    return Names;
}

void FHTNExecutionContext::ClearParameters()
{
    Parameters.Empty();
}

FString FHTNExecutionContext::ToString() const
{
    FString Result = TEXT("HTN Execution Context:\n");
    
    // Owner info
    Result += FString::Printf(TEXT("Owner: %s\n"), 
        OwnerActor ? *OwnerActor->GetName() : TEXT("None"));
    
    // World state info
    Result += TEXT("World State: ");
    if (WorldState)
    {
        Result += TEXT("\n");
        Result += WorldState->ToString();
    }
    else
    {
        Result += TEXT("None\n");
    }
    
    // Parameters info
    Result += TEXT("\nParameters:\n");
    if (Parameters.Num() > 0)
    {
        TArray<FName> SortedNames = GetParameterNames();
        SortedNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });
        
        for (const FName& Name : SortedNames)
        {
            FHTNProperty Value;
            GetParameter(Name, Value);
            Result += FString::Printf(TEXT("  %s: %s\n"), *Name.ToString(), *Value.ToString());
        }
    }
    else
    {
        Result += TEXT("  (None)\n");
    }
    
    return Result;
}

FHTNExecutionContext FHTNExecutionContext::Clone() const
{
    FHTNExecutionContext ClonedContext;
    
    // Copy the owner reference
    ClonedContext.OwnerActor = OwnerActor;
    
    // Deep copy the world state if it exists
    if (WorldState)
    {
        ClonedContext.WorldState = WorldState->Clone();
    }
    
    // Copy parameters
    ClonedContext.Parameters = Parameters;
    
    return ClonedContext;
}