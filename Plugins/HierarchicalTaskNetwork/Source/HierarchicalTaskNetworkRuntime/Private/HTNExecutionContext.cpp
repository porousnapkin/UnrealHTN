#include "HTNExecutionContext.h"
#include "HTNProperty.h"
#include "HTNLogging.h"

UHTNExecutionContext::UHTNExecutionContext()
    : WorldState(nullptr)
{
}

UHTNExecutionContext::UHTNExecutionContext(const UHTNExecutionContext* Other)
    : Parameters(Other->Parameters)
{
    // Deep copy the world state if it exists
    if (Other->WorldState)
    {
        WorldState = Other->WorldState->Clone();
    }
    else
    {
        WorldState = nullptr;
    }
}

void UHTNExecutionContext::SetWorldState(UHTNWorldState* InWorldState)
{
    WorldState = InWorldState;
}

bool UHTNExecutionContext::GetParameter(FName Key, FHTNProperty& OutValue) const
{
    if (const FHTNProperty* Property = Parameters.Find(Key))
    {
        OutValue = *Property;
        return true;
    }
    return false;
}

void UHTNExecutionContext::SetParameter(FName Key, const FHTNProperty& Value)
{
    Parameters.Add(Key, Value);
}

bool UHTNExecutionContext::HasParameter(FName Key) const
{
    return Parameters.Contains(Key);
}

bool UHTNExecutionContext::RemoveParameter(FName Key)
{
    return Parameters.Remove(Key) > 0;
}

TArray<FName> UHTNExecutionContext::GetParameterNames() const
{
    TArray<FName> Names;
    Parameters.GetKeys(Names);
    return Names;
}

void UHTNExecutionContext::ClearParameters()
{
    Parameters.Empty();
}

FString UHTNExecutionContext::ToString() const
{
    FString Result = TEXT("HTN Execution Context:\n");
    
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

UHTNExecutionContext UHTNExecutionContext::Clone() const
{
    UHTNExecutionContext ClonedContext;
    
    // Deep copy the world state if it exists
    if (WorldState)
    {
        ClonedContext.WorldState = WorldState->Clone();
    }
    
    // Copy parameters
    ClonedContext.Parameters = Parameters;
    
    return ClonedContext;
}