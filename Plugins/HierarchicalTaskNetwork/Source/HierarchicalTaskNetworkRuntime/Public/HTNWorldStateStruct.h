#pragma once

#include "CoreMinimal.h"
#include "HTNProperty.h"
#include "HTNPlanner.h"
#include "HTNWorldStateStruct.generated.h"

/**
 * Concrete implementation of the HTN world state.
 * Represents the current state of the world for HTN planning.
 */
USTRUCT(BlueprintType)
struct HIERARCHICALTASKNETWORKRUNTIME_API FHTNWorldStateStruct
{
	GENERATED_BODY()

public:
	/** Default constructor */
	FHTNWorldStateStruct() {}

	/** Constructor with initial properties */
	FHTNWorldStateStruct(const TMap<FName, FHTNProperty>& InProperties);

	/** Copy constructor */
	FHTNWorldStateStruct(const FHTNWorldStateStruct& Other);

	/** Move constructor */
	FHTNWorldStateStruct(FHTNWorldStateStruct&& Other) noexcept;

	/** Copy assignment operator */
	FHTNWorldStateStruct& operator=(const FHTNWorldStateStruct& Other);

	/** Move assignment operator */
	FHTNWorldStateStruct& operator=(FHTNWorldStateStruct&& Other) noexcept;

	/** Equality operator */
	bool operator==(const FHTNWorldStateStruct& Other) const;

	/** Inequality operator */
	bool operator!=(const FHTNWorldStateStruct& Other) const;

	/**
	 * Get a property value by name.
	 * @param Key - The name of the property to get
	 * @param OutValue - The value of the property if found
	 * @return true if the property was found, false otherwise
	 */
	bool GetProperty(FName Key, FHTNProperty& OutValue) const;

	/**
	 * Set a property value.
	 * @param Key - The name of the property to set
	 * @param Value - The value to set
	 */
	void SetProperty(FName Key, const FHTNProperty& Value);

	/**
	 * Check if a property exists.
	 * @param Key - The name of the property to check
	 * @return true if the property exists, false otherwise
	 */
	bool HasProperty(FName Key) const;

	/**
	 * Remove a property from the world state.
	 * @param Key - The name of the property to remove
	 * @return true if the property was removed, false if it didn't exist
	 */
	bool RemoveProperty(FName Key);

	/**
	 * Create a clone of this world state.
	 * @return A new world state with the same properties as this one
	 */
	FHTNWorldStateStruct Clone() const;

	/**
	 * Check if this world state is equal to another.
	 * @param Other - The other world state to compare with
	 * @return true if the world states are equal, false otherwise
	 */
	bool Equals(const FHTNWorldStateStruct& Other) const;

	/**
	 * Create a difference between this world state and another.
	 * The result will contain only properties that differ between the two world states.
	 * @param Other - The other world state to compare with
	 * @return A world state containing only the differing properties
	 */
	FHTNWorldStateStruct CreateDifference(const FHTNWorldStateStruct& Other) const;

	/**
	 * Get all property names in this world state.
	 * @return An array of property names
	 */
	TArray<FName> GetPropertyNames() const;

	/**
	 * Get a string representation of the world state for debugging.
	 * @return A string representing the world state
	 */
	FString ToString() const;

	// Template methods for type-safe property access

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

private:
	/** The properties stored in this world state */
	UPROPERTY()
	TMap<FName, FHTNProperty> Properties;
};

/**
 * UObject wrapper for FHTNWorldState.
 * This allows the world state to be used in Blueprints.
 */
UCLASS(BlueprintType)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNWorldState : public UObject
{
	GENERATED_BODY()

public:
	UHTNWorldState();

	/**
	 * Get a property value by name.
	 * @param Key - The name of the property to get
	 * @param OutValue - The value of the property if found
	 * @return true if the property was found, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	bool GetProperty(FName Key, FHTNProperty& OutValue) const;

	/**
	 * Set a property value.
	 * @param Key - The name of the property to set
	 * @param Value - The value to set
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	void SetProperty(FName Key, const FHTNProperty& Value);

	/**
	 * Check if a property exists.
	 * @param Key - The name of the property to check
	 * @return true if the property exists, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	bool HasProperty(FName Key) const;

	/**
	 * Remove a property from the world state.
	 * @param Key - The name of the property to remove
	 * @return true if the property was removed, false if it didn't exist
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	bool RemoveProperty(FName Key);

	/**
	 * Create a clone of this world state.
	 * @return A new world state with the same properties as this one
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	UHTNWorldState* Clone() const;

	/**
	 * Check if this world state is equal to another.
	 * @param Other - The other world state to compare with
	 * @return true if the world states are equal, false otherwise
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	bool Equals(const UHTNWorldState* Other) const;

	/**
	 * Create a difference between this world state and another.
	 * The result will contain only properties that differ between the two world states.
	 * @param Other - The other world state to compare with
	 * @return A world state containing only the differing properties
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	UHTNWorldState* CreateDifference(const UHTNWorldState* Other) const;

	/**
	 * Get all property names in this world state.
	 * @return An array of property names
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	TArray<FName> GetPropertyNames() const;

	/**
	 * Get a string representation of the world state for debugging.
	 * @return A string representing the world state
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	FString ToString() const;

	/**
	 * Create a new world state from an FHTNWorldState struct.
	 * @param InWorldState - The world state struct to copy
	 * @return A new UHTNWorldState object
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|WorldState")
	static UHTNWorldState* CreateFromStruct(const FHTNWorldStateStruct& InWorldState);

	/**
	 * Get the underlying FHTNWorldState struct.
	 * @return The world state struct
	 */
	const FHTNWorldStateStruct& GetWorldState() const { return WorldState; }

	/**
	 * Set the underlying FHTNWorldState struct.
	 * @param InWorldState - The world state struct to set
	 */
	void SetWorldState(const FHTNWorldStateStruct& InWorldState) { WorldState = InWorldState; }

	// Template methods for type-safe property access

	/**
	 * Get a typed property value.
	 * @param Key - The name of the property
	 * @param DefaultValue - Value to return if the property doesn't exist or is of wrong type
	 * @return The property value or the default value if not found
	 */
	template<typename T>
	T GetPropertyValue(FName Key, const T& DefaultValue) const
	{
		return WorldState.GetPropertyValue<T>(Key, DefaultValue);
	}

	/**
	 * Set a typed property value.
	 * @param Key - The name of the property
	 * @param Value - The value to set
	 */
	template<typename T>
	void SetPropertyValue(FName Key, const T& Value)
	{
		WorldState.SetPropertyValue<T>(Key, Value);
	}

private:
	/** The actual world state data */
	UPROPERTY()
	FHTNWorldStateStruct WorldState;
};

// Template specializations for FHTNWorldState

template<>
FORCEINLINE bool FHTNWorldStateStruct::GetPropertyValue<bool>(FName Key, const bool& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Boolean)
	{
		return Value.GetBoolValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE int32 FHTNWorldStateStruct::GetPropertyValue<int32>(FName Key, const int32& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Integer)
	{
		return Value.GetIntValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE float FHTNWorldStateStruct::GetPropertyValue<float>(FName Key, const float& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Float)
	{
		return Value.GetFloatValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE FString FHTNWorldStateStruct::GetPropertyValue<FString>(FName Key, const FString& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::String)
	{
		return Value.GetStringValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE FName FHTNWorldStateStruct::GetPropertyValue<FName>(FName Key, const FName& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Name)
	{
		return Value.GetNameValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE UObject* FHTNWorldStateStruct::GetPropertyValue<UObject*>(FName Key, UObject* const& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Object)
	{
		return Value.GetObjectValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE FVector FHTNWorldStateStruct::GetPropertyValue<FVector>(FName Key, const FVector& DefaultValue) const
{
	FHTNProperty Value;
	if (GetProperty(Key, Value) && Value.GetType() == EHTNPropertyType::Vector)
	{
		return Value.GetVectorValue();
	}
	return DefaultValue;
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<bool>(FName Key, const bool& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<int32>(FName Key, const int32& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<float>(FName Key, const float& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<FString>(FName Key, const FString& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<FName>(FName Key, const FName& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<UObject*>(FName Key, UObject* const& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}

template<>
FORCEINLINE void FHTNWorldStateStruct::SetPropertyValue<FVector>(FName Key, const FVector& Value)
{
	SetProperty(Key, FHTNProperty(Value));
}