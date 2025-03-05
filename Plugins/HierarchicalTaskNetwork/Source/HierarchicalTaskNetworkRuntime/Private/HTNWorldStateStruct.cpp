#include "HTNWorldStateStruct.h"

// FHTNWorldState Implementation

FHTNWorldStateStruct::FHTNWorldStateStruct(const TMap<FName, FHTNProperty>& InProperties)
	: Properties(InProperties)
	, OwnerActor(nullptr)
{
}

FHTNWorldStateStruct::FHTNWorldStateStruct(AActor* InOwnerActor)
	: OwnerActor(InOwnerActor)
{
}

FHTNWorldStateStruct::FHTNWorldStateStruct(AActor* InOwnerActor, const TMap<FName, FHTNProperty>& InProperties)
	: Properties(InProperties)
	, OwnerActor(InOwnerActor)
{
}

FHTNWorldStateStruct::FHTNWorldStateStruct(const FHTNWorldStateStruct& Other)
	: Properties(Other.Properties)
	, OwnerActor(Other.OwnerActor)
{
}

FHTNWorldStateStruct::FHTNWorldStateStruct(FHTNWorldStateStruct&& Other) noexcept
	: Properties(MoveTemp(Other.Properties))
	, OwnerActor(Other.OwnerActor)
{
	// Clear the moved-from object's owner to avoid double deletion issues
	Other.OwnerActor = nullptr;
}

FHTNWorldStateStruct& FHTNWorldStateStruct::operator=(const FHTNWorldStateStruct& Other)
{
	if (this != &Other)
	{
		Properties = Other.Properties;
		OwnerActor = Other.OwnerActor;
	}
	return *this;
}

FHTNWorldStateStruct& FHTNWorldStateStruct::operator=(FHTNWorldStateStruct&& Other) noexcept
{
	if (this != &Other)
	{
		Properties = MoveTemp(Other.Properties);
		OwnerActor = Other.OwnerActor;
		
		// Clear the moved-from object's owner to avoid double deletion issues
		Other.OwnerActor = nullptr;
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
	// Create a new world state with the same properties and owner
	return FHTNWorldStateStruct(OwnerActor, Properties);
}

bool FHTNWorldStateStruct::Equals(const FHTNWorldStateStruct& Other) const
{
	// Note: We don't compare owners because we're focusing on property equality
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
	// Create a world state for the differences with the same owner
	FHTNWorldStateStruct Difference(OwnerActor);

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
	
	// Add owner information
	Result += FString::Printf(TEXT("  Owner: %s\n"), OwnerActor ? *OwnerActor->GetName() : TEXT("None"));
	
	// Add properties
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