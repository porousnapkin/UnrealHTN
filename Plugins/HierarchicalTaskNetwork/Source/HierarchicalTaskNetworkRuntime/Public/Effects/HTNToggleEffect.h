// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNEffect.h"
#include "HTNToggleEffect.generated.h"

/**
 * Effect that toggles a boolean property in the world state.
 */
UCLASS(Blueprintable, EditInlineNew)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNToggleEffect : public UHTNEffect
{
	GENERATED_BODY()

public:
	UHTNToggleEffect();

	//~ Begin UHTNEffect Interface
	virtual void ApplyEffect_Implementation(UHTNWorldState* WorldState) const override;
	virtual FString GetDescription_Implementation() const override;
	virtual bool ValidateEffect_Implementation() const override;
	//~ End UHTNEffect Interface

protected:
	/** The key of the boolean property to toggle */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	FName PropertyKey;
    
	/** Whether to set to true if the property doesn't exist */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bSetTrueIfMissing;
    
	/** Whether to force to a specific value instead of toggling */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	bool bForceValue;
    
	/** The value to force (if not toggling) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect", meta = (EditCondition = "bForceValue"))
	bool ForcedValue;
};