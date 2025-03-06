// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNEffect.h"
#include "HTNProperty.h"
#include "HTNSetPropertyEffect.generated.h"

/**
 * Effect that sets a property in the world state to a specific value.
 */
UCLASS(Blueprintable, EditInlineNew)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNSetPropertyEffect : public UHTNEffect
{
	GENERATED_BODY()

public:
	UHTNSetPropertyEffect();

	//~ Begin UHTNEffect Interface
	virtual void ApplyEffect_Implementation(UHTNWorldState* WorldState) const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool ValidateEffect_Implementation() const override;
	//~ End UHTNEffect Interface

	/** The key of the property to set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName PropertyKey;
    
	/** The value to set */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FHTNProperty PropertyValue;
    
	/** Whether to get the value from another property */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bUseSourceProperty;
    
	/** The key of the source property (if using one) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (EditCondition = "bUseSourceProperty"))
	FName SourcePropertyKey;
    
	/** Whether to remove the property instead of setting it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bRemoveProperty;
};