#pragma once

#include "CoreMinimal.h"
#include "HTNWorldStateStruct.h"
#include "HTNProperty.h"
#include "HTNExecutionContext.generated.h"

/**
 * Execution context for HTN tasks.
 * This class manages the state and resources during plan execution,
 * providing tasks with access to the world state and shared parameters.
 */
UCLASS(BlueprintType)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNExecutionContext : public UObject
{
    GENERATED_BODY()

public:
    /** Default constructor */
    UHTNExecutionContext();

    /** Copy constructor */
    UHTNExecutionContext(const UHTNExecutionContext* Other);

    /**
     * Gets the world state being used for execution.
     * @return The mutable world state
     */
    FORCEINLINE UHTNWorldState* GetWorldState() const { return WorldState; }

    /**
     * Sets the world state used for execution.
     * @param InWorldState - The world state to use
     */
    void SetWorldState(UHTNWorldState* InWorldState);

    /**
     * Gets the owner actor (from the world state).
     * @return The owner actor, or nullptr if not set
     */
    FORCEINLINE AActor* GetOwner() const { return WorldState ? WorldState->GetOwner() : nullptr; }

    /**
     * Gets a parameter value by name.
     * @param Key - The name of the parameter
     * @param OutValue - The value of the parameter if found
     * @return true if the parameter was found, false otherwise
     */
    bool GetParameter(FName Key, FHTNProperty& OutValue) const;

    /**
     * Sets a parameter value.
     * @param Key - The name of the parameter
     * @param Value - The value to set
     */
    void SetParameter(FName Key, const FHTNProperty& Value);

    /**
     * Checks if a parameter exists.
     * @param Key - The name of the parameter to check
     * @return true if the parameter exists, false otherwise
     */
    bool HasParameter(FName Key) const;

    /**
     * Removes a parameter.
     * @param Key - The name of the parameter to remove
     * @return true if the parameter was removed, false if it didn't exist
     */
    bool RemoveParameter(FName Key);

    /**
     * Gets all parameter names.
     * @return An array of parameter names
     */
    TArray<FName> GetParameterNames() const;

    /**
     * Clears all parameters.
     */
    void ClearParameters();

    /**
     * Creates a string representation of this context for debugging.
     * @return A string representation
     */
    FString ToString() const;

    /**
     * Creates a clone of this context.
     * @return A new context with the same state
     */
    UHTNExecutionContext Clone() const;

    // Template methods for type-safe parameter access

    /**
     * Gets a typed parameter value.
     * @param Key - The name of the parameter
     * @param DefaultValue - Value to return if the parameter doesn't exist or is of wrong type
     * @return The parameter value or the default value if not found
     */
    template<typename T>
    T GetParameterValue(FName Key, const T& DefaultValue) const;

    /**
     * Sets a typed parameter value.
     * @param Key - The name of the parameter
     * @param Value - The value to set
     */
    template<typename T>
    void SetParameterValue(FName Key, const T& Value);

private:
    /** The current world state */
    UPROPERTY()
    UHTNWorldState* WorldState;

    /** Parameters shared between tasks */
    UPROPERTY()
    TMap<FName, FHTNProperty> Parameters;
};

// Template specializations for type-safe parameter access
template<>
FORCEINLINE bool UHTNExecutionContext::GetParameterValue<bool>(FName Key, const bool& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::Boolean)
    {
        return Value.GetBoolValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE int32 UHTNExecutionContext::GetParameterValue<int32>(FName Key, const int32& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::Integer)
    {
        return Value.GetIntValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE float UHTNExecutionContext::GetParameterValue<float>(FName Key, const float& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::Float)
    {
        return Value.GetFloatValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE FString UHTNExecutionContext::GetParameterValue<FString>(FName Key, const FString& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::String)
    {
        return Value.GetStringValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE FName UHTNExecutionContext::GetParameterValue<FName>(FName Key, const FName& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::Name)
    {
        return Value.GetNameValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE UObject* UHTNExecutionContext::GetParameterValue<UObject*>(FName Key, UObject* const& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::Object)
    {
        return Value.GetObjectValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE FVector UHTNExecutionContext::GetParameterValue<FVector>(FName Key, const FVector& DefaultValue) const
{
    FHTNProperty Value;
    if (GetParameter(Key, Value) && Value.GetType() == EHTNPropertyType::Vector)
    {
        return Value.GetVectorValue();
    }
    return DefaultValue;
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<bool>(FName Key, const bool& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<int32>(FName Key, const int32& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<float>(FName Key, const float& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<FString>(FName Key, const FString& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<FName>(FName Key, const FName& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<UObject*>(FName Key, UObject* const& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void UHTNExecutionContext::SetParameterValue<FVector>(FName Key, const FVector& Value)
{
    SetParameter(Key, FHTNProperty(Value));
}