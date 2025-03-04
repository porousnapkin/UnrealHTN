// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNWorldStateStruct.h"

// FHTNWorldState Implementation

FHTNWorldStateStruct::FHTNWorldStateStruct(const TMap<FName, FHTNProperty>& InProperties)
	: Properties(InProperties)
{
}

FHTNWorldStateStruct::FHTNWorldStateStruct(const FHTNWorldStateStruct& Other)
	: Properties(Other.Properties)
{
}

FHTNWorldStateStruct::FHTNWorldStateStruct(FHTNWorldStateStruct&& Other) noexcept
	: Properties(MoveTemp(Other.Properties))
{
}

FHTNWorldStateStruct& FHTNWorldStateStruct::operator=(const FHTNWorldStateStruct& Other)
{
	if (this != &Other)
	{
		Properties = Other.Properties;
	}
	return *this;
}

FHTNWorldStateStruct& FHTNWorldStateStruct::operator=(FHTNWorldStateStruct&& Other) noexcept
{
	if (this != &Other)
	{
		Properties = MoveTemp(Other.Properties);
	}
	return *this;
}

bool FHTNWorldStateStruct::operator==(const FHTNWorldStateStruct& Other) const
{
	return Equals(Other);
}

bool FHTNWorldStateStruct::operator!=(const FHTNWorldStateStruct& Other) const
{
	return !Equals(Other);
}

bool FHTNWorldStateStruct::GetProperty(FName Key, FHTNProperty& OutValue) const
{
	if (const FHTNProperty* Property = Properties.Find(Key))
	{
		OutValue = *Property;
		return true;
	}
	return false;
}

void FHTNWorldStateStruct::SetProperty(FName Key, const FHTNProperty& Value)
{
	Properties.Add(Key, Value);
}

bool FHTNWorldStateStruct::HasProperty(FName Key) const
{
	return Properties.Contains(Key);
}

bool FHTNWorldStateStruct::RemoveProperty(FName Key)
{
	return Properties.Remove(Key) > 0;
}

FHTNWorldStateStruct FHTNWorldStateStruct::Clone() const
{
	return FHTNWorldStateStruct(*this);
}

bool FHTNWorldStateStruct::Equals(const FHTNWorldStateStruct& Other) const
{
	if (Properties.Num() != Other.Properties.Num())
	{
		return false;
	}

	for (const auto& Pair : Properties)
	{
		const FHTNProperty* OtherProperty = Other.Properties.Find(Pair.Key);
		if (!OtherProperty || *OtherProperty != Pair.Value)
		{
			return false;
		}
	}

	return true;
}

FHTNWorldStateStruct FHTNWorldStateStruct::CreateDifference(const FHTNWorldStateStruct& Other) const
{
	FHTNWorldStateStruct Difference;

	// Add properties that are different or don't exist in Other
	for (const auto& Pair : Properties)
	{
		const FHTNProperty* OtherProperty = Other.Properties.Find(Pair.Key);
		if (!OtherProperty || *OtherProperty != Pair.Value)
		{
			Difference.SetProperty(Pair.Key, Pair.Value);
		}
	}

	// Add properties that exist in Other but not in this
	for (const auto& Pair : Other.Properties)
	{
		if (!Properties.Contains(Pair.Key))
		{
			Difference.SetProperty(Pair.Key, Pair.Value);
		}
	}

	return Difference;
}

TArray<FName> FHTNWorldStateStruct::GetPropertyNames() const
{
	TArray<FName> Names;
	Properties.GetKeys(Names);
	return Names;
}

FString FHTNWorldStateStruct::ToString() const
{
	FString Result = TEXT("WorldState {\n");
	
	TArray<FName> SortedNames = GetPropertyNames();
	SortedNames.Sort([](const FName& A, const FName& B) { return A.LexicalLess(B); });
	
	for (const FName& Name : SortedNames)
	{
		FHTNProperty Value;
		GetProperty(Name, Value);
		Result += FString::Printf(TEXT("  %s: %s\n"), *Name.ToString(), *Value.ToString());
	}
	
	Result += TEXT("}");
	return Result;
}

// UHTNWorldState Implementation

UHTNWorldState::UHTNWorldState()
{
}

bool UHTNWorldState::GetProperty(FName Key, FHTNProperty& OutValue) const
{
	return WorldState.GetProperty(Key, OutValue);
}

void UHTNWorldState::SetProperty(FName Key, const FHTNProperty& Value)
{
	WorldState.SetProperty(Key, Value);
}

bool UHTNWorldState::HasProperty(FName Key) const
{
	return WorldState.HasProperty(Key);
}

bool UHTNWorldState::RemoveProperty(FName Key)
{
	return WorldState.RemoveProperty(Key);
}

UHTNWorldState* UHTNWorldState::Clone() const
{
	UHTNWorldState* Clone = CreateFromStruct(WorldState.Clone());
	return Clone;
}

bool UHTNWorldState::Equals(const UHTNWorldState* Other) const
{
	if (!Other)
	{
		return false;
	}

	return WorldState.Equals(Other->WorldState);
}

UHTNWorldState* UHTNWorldState::CreateDifference(const UHTNWorldState* Other) const
{
	if (!Other)
	{
		return Clone();
	}

	return CreateFromStruct(WorldState.CreateDifference(Other->WorldState));
}

TArray<FName> UHTNWorldState::GetPropertyNames() const
{
	return WorldState.GetPropertyNames();
}

FString UHTNWorldState::ToString() const
{
	return WorldState.ToString();
}

UHTNWorldState* UHTNWorldState::CreateFromStruct(const FHTNWorldStateStruct& InWorldState)
{
	UHTNWorldState* Result = NewObject<UHTNWorldState>();
	Result->SetWorldState(InWorldState);
	return Result;
}