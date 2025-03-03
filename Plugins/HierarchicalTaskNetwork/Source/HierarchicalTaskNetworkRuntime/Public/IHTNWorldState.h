// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FHTNProperty.h"
#include "UObject/Interface.h"
#include "IHTNWorldState.generated.h"

/**
 * Interface for HTN world state representation.
 * The world state stores properties that represent the current state of the world for HTN planning.
 * It provides methods to get, set, and manipulate these properties.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UHTNWorldState : public UInterface
{
	GENERATED_BODY()
};

class HIERARCHICALTASKNETWORKRUNTIME_API IHTNWorldState
{
	GENERATED_BODY()

public:
	/**
	 * Get a property value by name.
	 * @param Key - The name of the property to get
	 * @param OutValue - The value of the property if found
	 * @return true if the property was found, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	bool GetProperty(FName Key, FHTNProperty& OutValue) const;

	/**
	 * Set a property value.
	 * @param Key - The name of the property to set
	 * @param Value - The value to set
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	void SetProperty(FName Key, const FHTNProperty& Value);

	/**
	 * Check if a property exists.
	 * @param Key - The name of the property to check
	 * @return true if the property exists, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	bool HasProperty(FName Key) const;

	/**
	 * Remove a property from the world state.
	 * @param Key - The name of the property to remove
	 * @return true if the property was removed, false if it didn't exist
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	bool RemoveProperty(FName Key);

	/**
	 * Create a clone of this world state.
	 * @return A new world state with the same properties as this one
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	TScriptInterface<IHTNWorldState> Clone() const;

	/**
	 * Check if this world state is equal to another.
	 * @param Other - The other world state to compare with
	 * @return true if the world states are equal, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	bool Equals(const TScriptInterface<IHTNWorldState>& Other) const;

	/**
	 * Create a difference between this world state and another.
	 * The result will contain only properties that differ between the two world states.
	 * @param Other - The other world state to compare with
	 * @return A world state containing only the differing properties
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	TScriptInterface<IHTNWorldState> CreateDifference(const TScriptInterface<IHTNWorldState>& Other) const;

	/**
	 * Get all property names in this world state.
	 * @return An array of property names
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	TArray<FName> GetPropertyNames() const;

	/**
	 * Get a string representation of the world state for debugging.
	 * @return A string representing the world state
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|WorldState")
	FString ToString() const;

	// Template methods for type-safe property access (C++ only)

	/**
	 * Get a typed property value.
	 * @param Key - The name of the property
	 * @param DefaultValue - Value to return if the property doesn't exist or is of wrong type
	 * @return The property value or the default value if not found
	 */
	template<typename T>
	T GetPropertyValue(FName Key, const T& DefaultValue) const;

	/**
	 * Set a typed property value.
	 * @param Key - The name of the property
	 * @param Value - The value to set
	 */
	template<typename T>
	void SetPropertyValue(FName Key, const T& Value);
};

// Template specializations for type-safe property access

template<>
FORCEINLINE bool IHTNWorldState::GetPropertyValue<bool>(FName Key, const bool& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Boolean)
	{
		return Value.GetBoolValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE int32 IHTNWorldState::GetPropertyValue<int32>(FName Key, const int32& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Integer)
	{
		return Value.GetIntValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE float IHTNWorldState::GetPropertyValue<float>(FName Key, const float& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Float)
	{
		return Value.GetFloatValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE FString IHTNWorldState::GetPropertyValue<FString>(FName Key, const FString& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::String)
	{
		return Value.GetStringValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE FName IHTNWorldState::GetPropertyValue<FName>(FName Key, const FName& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Name)
	{
		return Value.GetNameValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE UObject* IHTNWorldState::GetPropertyValue<UObject*>(FName Key, UObject* const& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Object)
	{
		return Value.GetObjectValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE FVector IHTNWorldState::GetPropertyValue<FVector>(FName Key, const FVector& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Vector)
	{
		return Value.GetVectorValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<bool>(FName Key, const bool& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<int32>(FName Key, const int32& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<float>(FName Key, const float& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<FString>(FName Key, const FString& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<FName>(FName Key, const FName& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<UObject*>(FName Key, UObject* const& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void IHTNWorldState::SetPropertyValue<FVector>(FName Key, const FVector& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}