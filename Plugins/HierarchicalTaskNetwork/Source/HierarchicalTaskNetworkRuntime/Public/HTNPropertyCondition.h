// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNCondition.h"
#include "HTNProperty.h"
#include "HTNPropertyCondition.generated.h"

/**
 * Enum for property check types.
 */
UENUM(BlueprintType)
enum class EHTNPropertyCheckType : uint8
{
	/** Check if the property is true (bool properties only) */
	IsTrue UMETA(DisplayName = "Is True"),
    
	/** Check if the property is false (bool properties only) */
	IsFalse UMETA(DisplayName = "Is False"),
    
	/** Check if the property exists */
	Exists UMETA(DisplayName = "Exists"),
    
	/** Check if the property does not exist */
	NotExists UMETA(DisplayName = "Does Not Exist"),
    
	/** Check if the property equals a specific value */
	Equals UMETA(DisplayName = "Equals"),
    
	/** Check if the property does not equal a specific value */
	NotEquals UMETA(DisplayName = "Not Equals")
};

/**
 * Condition that checks a property in the world state.
 */
UCLASS(Blueprintable, EditInlineNew)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPropertyCondition : public UHTNCondition
{
	GENERATED_BODY()

public:
	UHTNPropertyCondition();

	//~ Begin UHTNCondition Interface
	virtual bool CheckCondition_Implementation(const UHTNWorldState* WorldState) const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool ValidateCondition_Implementation() const override;
	//~ End UHTNCondition Interface

protected:
	/** The key of the property to check */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FName PropertyKey;
    
	/** The type of check to perform */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	EHTNPropertyCheckType CheckType;
    
	/** The value to compare against (for Equals and NotEquals) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
	FHTNProperty CompareValue;
};