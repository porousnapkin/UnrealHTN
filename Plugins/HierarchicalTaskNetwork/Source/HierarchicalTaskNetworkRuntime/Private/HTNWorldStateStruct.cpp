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

bool UHTNWorldState::GetProperty_Implementation(FName Key, FHTNProperty& OutValue) const
{
	return WorldState.GetProperty(Key, OutValue);
}

void UHTNWorldState::SetProperty_Implementation(FName Key, const FHTNProperty& Value)
{
	WorldState.SetProperty(Key, Value);
}

bool UHTNWorldState::HasProperty_Implementation(FName Key) const
{
	return WorldState.HasProperty(Key);
}

bool UHTNWorldState::RemoveProperty_Implementation(FName Key)
{
	return WorldState.RemoveProperty(Key);
}

TScriptInterface<IHTNWorldStateInterface> UHTNWorldState::Clone_Implementation() const
{
	UHTNWorldState* Clone = CreateFromStruct(WorldState.Clone());
	TScriptInterface<IHTNWorldStateInterface> Result;
	Result.SetObject(Clone);
	Result.SetInterface(Cast<IHTNWorldStateInterface>(Clone));
	return Result;
}

bool UHTNWorldState::Equals_Implementation(const TScriptInterface<IHTNWorldStateInterface>& Other) const
{
	if (!Other.GetObject())
	{
		return false;
	}

	const UHTNWorldState* OtherState = Cast<UHTNWorldState>(Other.GetObject());
	if (!OtherState)
	{
		// If it's a different implementation, compare property by property
		TArray<FName> AllNames = GetPropertyNames_Implementation();
		for (const FName& Name : Other->GetPropertyNames_Implementation())
		{
			if (!AllNames.Contains(Name))
			{
				AllNames.Add(Name);
			}
		}

		for (const FName& Name : AllNames)
		{
			FHTNProperty Value1, Value2;
			bool HasValue1 = GetProperty_Implementation(Name, Value1);
			bool HasValue2 = Other->GetProperty_Implementation(Name, Value2);

			if (HasValue1 != HasValue2 || (HasValue1 && HasValue2 && Value1 != Value2))
			{
				return false;
			}
		}

		return true;
	}

	return WorldState.Equals(OtherState->WorldState);
}

TScriptInterface<IHTNWorldStateInterface> UHTNWorldState::CreateDifference_Implementation(const TScriptInterface<IHTNWorldStateInterface>& Other) const
{
	if (!Other.GetObject())
	{
		return Clone_Implementation();
	}

	const UHTNWorldState* OtherState = Cast<UHTNWorldState>(Other.GetObject());
	if (!OtherState)
	{
		// If it's a different implementation, create a difference manually
		FHTNWorldStateStruct Difference;

		// Process properties in this world state
		TArray<FName> MyProperties = GetPropertyNames_Implementation();
		for (const FName& Name : MyProperties)
		{
			FHTNProperty MyValue, OtherValue;
			GetProperty_Implementation(Name, MyValue);
			
			if (!Other->GetProperty_Implementation(Name, OtherValue) || MyValue != OtherValue)
			{
				Difference.SetProperty(Name, MyValue);
			}
		}

		// Process properties in the other world state
		TArray<FName> OtherProperties = Other->GetPropertyNames_Implementation();
		for (const FName& Name : OtherProperties)
		{
			if (!HasProperty_Implementation(Name))
			{
				FHTNProperty OtherValue;
				Other->GetProperty_Implementation(Name, OtherValue);
				Difference.SetProperty(Name, OtherValue);
			}
		}

		UHTNWorldState* Result = CreateFromStruct(Difference);
		TScriptInterface<IHTNWorldStateInterface> ResultInterface;
		ResultInterface.SetObject(Result);
		ResultInterface.SetInterface(Cast<IHTNWorldStateInterface>(Result));
		return ResultInterface;
	}

	UHTNWorldState* Result = CreateFromStruct(WorldState.CreateDifference(OtherState->WorldState));
	TScriptInterface<IHTNWorldStateInterface> ResultInterface;
	ResultInterface.SetObject(Result);
	ResultInterface.SetInterface(Cast<IHTNWorldStateInterface>(Result));
	return ResultInterface;
}

TArray<FName> UHTNWorldState::GetPropertyNames_Implementation() const
{
	return WorldState.GetPropertyNames();
}

FString UHTNWorldState::ToString_Implementation() const
{
	return WorldState.ToString();
}

UHTNWorldState* UHTNWorldState::CreateFromStruct(const FHTNWorldStateStruct& InWorldState)
{
	UHTNWorldState* Result = NewObject<UHTNWorldState>();
	Result->SetWorldState(InWorldState);
	return Result;
}