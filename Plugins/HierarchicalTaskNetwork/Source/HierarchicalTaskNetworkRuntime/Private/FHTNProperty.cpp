// Copyright Epic Games, Inc. All Rights Reserved.

#include "FHTNProperty.h"

FHTNProperty::FHTNProperty()
	: Type(EHTNPropertyType::Invalid)
	, BoolValue(false)
{
}

FHTNProperty::FHTNProperty(bool InBoolValue)
	: Type(EHTNPropertyType::Boolean)
	, BoolValue(InBoolValue)
{
}

FHTNProperty::FHTNProperty(int32 InIntValue)
	: Type(EHTNPropertyType::Integer)
	, IntValue(InIntValue)
{
}

FHTNProperty::FHTNProperty(float InFloatValue)
	: Type(EHTNPropertyType::Float)
	, FloatValue(InFloatValue)
{
}

FHTNProperty::FHTNProperty(const FString& InStringValue)
	: Type(EHTNPropertyType::String)
	, BoolValue(false) // Initialize union
	, StringValue(InStringValue)
{
}

FHTNProperty::FHTNProperty(FName InNameValue)
	: Type(EHTNPropertyType::Name)
	, BoolValue(false) // Initialize union
	, NameValue(InNameValue)
{
}

FHTNProperty::FHTNProperty(UObject* InObjectValue)
	: Type(EHTNPropertyType::Object)
	, ObjectValue(InObjectValue)
{
}

FHTNProperty::FHTNProperty(const FVector& InVectorValue)
	: Type(EHTNPropertyType::Vector)
	, VectorValue(InVectorValue)
{
}

FHTNProperty::FHTNProperty(const FHTNProperty& Other)
{
	CopyFrom(Other);
}

FHTNProperty::FHTNProperty(FHTNProperty&& Other) noexcept
	: Type(Other.Type)
{
	switch (Type)
	{
	case EHTNPropertyType::Boolean:
		BoolValue = Other.BoolValue;
		break;
	case EHTNPropertyType::Integer:
		IntValue = Other.IntValue;
		break;
	case EHTNPropertyType::Float:
		FloatValue = Other.FloatValue;
		break;
	case EHTNPropertyType::String:
		StringValue = MoveTemp(Other.StringValue);
		break;
	case EHTNPropertyType::Name:
		NameValue = Other.NameValue;
		break;
	case EHTNPropertyType::Object:
		ObjectValue = Other.ObjectValue;
		break;
	case EHTNPropertyType::Vector:
		VectorValue = Other.VectorValue;
		break;
	default:
		BoolValue = false;
		break;
	}

	// Reset the source to invalid state
	Other.Type = EHTNPropertyType::Invalid;
	Other.BoolValue = false;
	Other.StringValue.Empty();
	Other.NameValue = NAME_None;
}

FHTNProperty::~FHTNProperty()
{
	Clear();
}

FHTNProperty& FHTNProperty::operator=(const FHTNProperty& Other)
{
	if (this != &Other)
	{
		Clear();
		CopyFrom(Other);
	}
	return *this;
}

FHTNProperty& FHTNProperty::operator=(FHTNProperty&& Other) noexcept
{
	if (this != &Other)
	{
		Clear();
		
		Type = Other.Type;
		switch (Type)
		{
		case EHTNPropertyType::Boolean:
			BoolValue = Other.BoolValue;
			break;
		case EHTNPropertyType::Integer:
			IntValue = Other.IntValue;
			break;
		case EHTNPropertyType::Float:
			FloatValue = Other.FloatValue;
			break;
		case EHTNPropertyType::String:
			StringValue = MoveTemp(Other.StringValue);
			break;
		case EHTNPropertyType::Name:
			NameValue = Other.NameValue;
			break;
		case EHTNPropertyType::Object:
			ObjectValue = Other.ObjectValue;
			break;
		case EHTNPropertyType::Vector:
			VectorValue = Other.VectorValue;
			break;
		default:
			BoolValue = false;
			break;
		}

		// Reset the source to invalid state
		Other.Type = EHTNPropertyType::Invalid;
		Other.BoolValue = false;
		Other.StringValue.Empty();
		Other.NameValue = NAME_None;
	}
	return *this;
}

bool FHTNProperty::operator==(const FHTNProperty& Other) const
{
	if (Type != Other.Type)
	{
		return false;
	}

	switch (Type)
	{
	case EHTNPropertyType::Boolean:
		return BoolValue == Other.BoolValue;
	case EHTNPropertyType::Integer:
		return IntValue == Other.IntValue;
	case EHTNPropertyType::Float:
		return FMath::IsNearlyEqual(FloatValue, Other.FloatValue);
	case EHTNPropertyType::String:
		return StringValue.Equals(Other.StringValue);
	case EHTNPropertyType::Name:
		return NameValue == Other.NameValue;
	case EHTNPropertyType::Object:
		return ObjectValue == Other.ObjectValue;
	case EHTNPropertyType::Vector:
		return VectorValue.Equals(Other.VectorValue);
	default:
		return true; // Two invalid properties are considered equal
	}
}

bool FHTNProperty::operator!=(const FHTNProperty& Other) const
{
	return !(*this == Other);
}

FString FHTNProperty::ToString() const
{
	switch (Type)
	{
	case EHTNPropertyType::Boolean:
		return BoolValue ? TEXT("true") : TEXT("false");
	case EHTNPropertyType::Integer:
		return FString::FromInt(IntValue);
	case EHTNPropertyType::Float:
		return FString::SanitizeFloat(FloatValue);
	case EHTNPropertyType::String:
		return StringValue;
	case EHTNPropertyType::Name:
		return NameValue.ToString();
	case EHTNPropertyType::Object:
		return ObjectValue ? ObjectValue->GetName() : TEXT("None");
	case EHTNPropertyType::Vector:
		return VectorValue.ToString();
	default:
		return TEXT("Invalid");
	}
}

bool FHTNProperty::GetBoolValue(bool DefaultValue) const
{
	if (Type == EHTNPropertyType::Boolean)
	{
		return BoolValue;
	}
	
	// Type conversion for common cases
	if (Type == EHTNPropertyType::Integer)
	{
		return IntValue != 0;
	}
	else if (Type == EHTNPropertyType::Float)
	{
		return !FMath::IsNearlyZero(FloatValue);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get boolean from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

int32 FHTNProperty::GetIntValue(int32 DefaultValue) const
{
	if (Type == EHTNPropertyType::Integer)
	{
		return IntValue;
	}
	
	// Type conversion for common cases
	if (Type == EHTNPropertyType::Boolean)
	{
		return BoolValue ? 1 : 0;
	}
	else if (Type == EHTNPropertyType::Float)
	{
		return FMath::RoundToInt(FloatValue);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get integer from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

float FHTNProperty::GetFloatValue(float DefaultValue) const
{
	if (Type == EHTNPropertyType::Float)
	{
		return FloatValue;
	}
	
	// Type conversion for common cases
	if (Type == EHTNPropertyType::Boolean)
	{
		return BoolValue ? 1.0f : 0.0f;
	}
	else if (Type == EHTNPropertyType::Integer)
	{
		return static_cast<float>(IntValue);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get float from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

const FString& FHTNProperty::GetStringValue(const FString& DefaultValue) const
{
	if (Type == EHTNPropertyType::String)
	{
		return StringValue;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get string from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

FName FHTNProperty::GetNameValue(FName DefaultValue) const
{
	if (Type == EHTNPropertyType::Name)
	{
		return NameValue;
	}
	
	// Type conversion for string
	if (Type == EHTNPropertyType::String)
	{
		return FName(*StringValue);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get name from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

UObject* FHTNProperty::GetObjectValue(UObject* DefaultValue) const
{
	if (Type == EHTNPropertyType::Object)
	{
		return ObjectValue;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get object from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

FVector FHTNProperty::GetVectorValue(const FVector& DefaultValue) const
{
	if (Type == EHTNPropertyType::Vector)
	{
		return VectorValue;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Attempted to get vector from property of type %d"), static_cast<int32>(Type));
	return DefaultValue;
}

void FHTNProperty::Clear()
{
	// No need for special cleanup with the current types
	Type = EHTNPropertyType::Invalid;
	BoolValue = false;
	StringValue.Empty();
	NameValue = NAME_None;
}

void FHTNProperty::CopyFrom(const FHTNProperty& Other)
{
	Type = Other.Type;
	
	switch (Type)
	{
	case EHTNPropertyType::Boolean:
		BoolValue = Other.BoolValue;
		break;
	case EHTNPropertyType::Integer:
		IntValue = Other.IntValue;
		break;
	case EHTNPropertyType::Float:
		FloatValue = Other.FloatValue;
		break;
	case EHTNPropertyType::String:
		StringValue = Other.StringValue;
		break;
	case EHTNPropertyType::Name:
		NameValue = Other.NameValue;
		break;
	case EHTNPropertyType::Object:
		ObjectValue = Other.ObjectValue;
		break;
	case EHTNPropertyType::Vector:
		VectorValue = Other.VectorValue;
		break;
	default:
		BoolValue = false;
		break;
	}
}