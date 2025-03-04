// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "HTNTaskInterface.h"
#include "HTNMethod.h"
#include "HTNCompoundTask.generated.h"

/**
 * Base class for compound HTN tasks.
 * Compound tasks represent higher-level tasks that must be decomposed into 
 * simpler tasks before they can be executed.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNCompoundTask : public UHTNTask, public IHTNCompoundTaskInterface
{
	GENERATED_BODY()

public:
	UHTNCompoundTask();

	//~ Begin IHTNCompoundTaskInterface
	virtual bool GetAvailableMethods_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNMethod*>& OutMethods) const override;
	virtual bool ApplyMethod_Implementation(UHTNMethod* Method, const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNTask*>& OutTasks) const override;
	//~ End IHTNCompoundTaskInterface

	/**
	 * This will be implemented later with full functionality
	 * when we build the Compound Task implementation.
	 * For now, this is just a placeholder to satisfy the forward declaration.
	 */
};