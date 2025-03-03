// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNPropertyType.h"
#include "HTNProperty.generated.h"

/**
 * Structure that can hold any property type supported by the HTN planner.
 * This provides type safety and flexibility for storing different values in the world state.
 */
USTRUCT(BlueprintType)
struct HIERARCHICALTASKNETWORKRUNTIME_API FHTNProperty
{
	GENERATED_BODY()

public:
	/** Default constructor */
	FHTNProperty();

	/** Constructor for boolean values */
	FHTNProperty(bool InBoolValue);

	/** Constructor for integer values */
	FHTNProperty(int32 InIntValue);

	/** Constructor for float values */
	FHTNProperty(float InFloatValue);

	/** Constructor for string values */
	FHTNProperty(const FString& InStringValue);

	/** Constructor for name values */
	FHTNProperty(FName InNameValue);

	/** Constructor for object reference values */
	FHTNProperty(UObject* InObjectValue);

	/** Constructor for vector values */
	FHTNProperty(const FVector& InVectorValue);

	/** Copy constructor */
	FHTNProperty(const FHTNProperty& Other);

	/** Move constructor */
	FHTNProperty(FHTNProperty&& Other) noexcept;

	/** Destructor */
	~FHTNProperty();

	/** Copy assignment operator */
	FHTNProperty& operator=(const FHTNProperty& Other);

	/** Move assignment operator */
	FHTNProperty& operator=(FHTNProperty&& Other) noexcept;

	/** Equality operator */
	bool operator==(const FHTNProperty& Other) const;

	/** Inequality operator */
	bool operator!=(const FHTNProperty& Other) const;

	/** Returns the type of this property */
	EHTNPropertyType GetType() const { return Type; }

	/** Returns true if the property is valid (has a non-Invalid type) */
	bool IsValid() const { return Type != EHTNPropertyType::Invalid; }

	/** Convert the property to a string for debugging */
	FString ToString() const;

	/** Get the property as a boolean value */
	bool GetBoolValue(bool DefaultValue = false) const;

	/** Get the property as an integer value */
	int32 GetIntValue(int32 DefaultValue = 0) const;

	/** Get the property as a float value */
	float GetFloatValue(float DefaultValue = 0.0f) const;

	/** Get the property as a string value */
	const FString& GetStringValue(const FString& DefaultValue = FString()) const;

	/** Get the property as a name value */
	FName GetNameValue(FName DefaultValue = NAME_None) const;

	/** Get the property as an object value */
	UObject* GetObjectValue(UObject* DefaultValue = nullptr) const;

	/** Get the property as a vector value */
	FVector GetVectorValue(const FVector& DefaultValue = FVector::ZeroVector) const;

	/** Static helper to create an invalid property */
	static FHTNProperty Invalid() { return FHTNProperty(); }

private:
	/** The type of this property */
	UPROPERTY()
	EHTNPropertyType Type;

	/** Union to efficiently store the property value based on its type */
	union
	{
		bool BoolValue;
		int32 IntValue;
		float FloatValue;
		UObject* ObjectValue;
		FVector VectorValue;
	};

	/** Storage for string values (can't be in the union) */
	UPROPERTY()
	FString StringValue;

	/** Storage for name values (can't be in the union) */
	UPROPERTY()
	FName NameValue;

	/** Clear any allocated memory */
	void Clear();

	/** Copy from another property */
	void CopyFrom(const FHTNProperty& Other);
};

/**
 * Template functions for type-safe property creation
 */

/** Primary template for creating properties - not implemented */
template<typename T>
FORCEINLINE FHTNProperty MakeHTNProperty(const T& Value);

/** Create a boolean property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<bool>(const bool& Value)
{
	return FHTNProperty(Value);
}

/** Create an integer property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<int32>(const int32& Value)
{
	return FHTNProperty(Value);
}

/** Create a float property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<float>(const float& Value)
{
	return FHTNProperty(Value);
}

/** Create a string property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<FString>(const FString& Value)
{
	return FHTNProperty(Value);
}

/** Create a name property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<FName>(const FName& Value)
{
	return FHTNProperty(Value);
}

/** Create an object property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<UObject*>(UObject* const& Value)
{
	return FHTNProperty(Value);
}

/** Create a vector property */
template<>
FORCEINLINE FHTNProperty MakeHTNProperty<FVector>(const FVector& Value)
{
	return FHTNProperty(Value);
}