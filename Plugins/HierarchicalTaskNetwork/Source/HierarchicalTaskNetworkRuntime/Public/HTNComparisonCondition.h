// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNCondition.h"
#include "HTNProperty.h"
#include "HTNComparisonCondition.generated.h"

/**
 * Enum for comparison types.
 */
UENUM(BlueprintType)
enum class EHTNComparisonType : uint8
{
    /** Less than */
    LessThan UMETA(DisplayName = "Less Than (<)"),
    
    /** Less than or equal to */
    LessThanOrEqual UMETA(DisplayName = "Less Than Or Equal (<=)"),
    
    /** Greater than */
    GreaterThan UMETA(DisplayName = "Greater Than (>)"),
    
    /** Greater than or equal to */
    GreaterThanOrEqual UMETA(DisplayName = "Greater Than Or Equal (>=)"),
    
    /** Approximately equal (for floating point) */
    ApproximatelyEqual UMETA(DisplayName = "Approximately Equal (~=)")
};

/**
 * Condition that compares numeric properties in the world state.
 */
UCLASS(Blueprintable, EditInlineNew)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNComparisonCondition : public UHTNCondition
{
    GENERATED_BODY()

public:
    UHTNComparisonCondition();

    //~ Begin UHTNCondition Interface
    virtual bool CheckCondition_Implementation(const UHTNWorldState* WorldState) const override;
    virtual FString GetDescription_Implementation() const override;
    virtual bool ValidateCondition_Implementation() const override;
    //~ End UHTNCondition Interface

protected:
    /** The key of the left-hand property to compare */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    FName LeftPropertyKey;
    
    /** The comparison operation to perform */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    EHTNComparisonType ComparisonType;
    
    /** Whether to use a fixed value for the right side of the comparison */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition")
    bool bUseFixedRightValue;
    
    /** The key of the right-hand property to compare (if not using fixed value) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (EditCondition = "!bUseFixedRightValue"))
    FName RightPropertyKey;
    
    /** The fixed value to compare against (if using fixed value) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (EditCondition = "bUseFixedRightValue"))
    float FixedRightValue;
    
    /** Tolerance for approximate equality comparison (for floating point) */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Condition", meta = (ClampMin = "0.0001"))
    float ApproximateTolerance;
};