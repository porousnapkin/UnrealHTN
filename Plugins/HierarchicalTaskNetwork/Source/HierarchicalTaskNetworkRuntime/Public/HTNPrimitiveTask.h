// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNTask.h"
#include "HTNTaskInterface.h"
#include "HTNPrimitiveTask.generated.h"

/**
 * Base class for primitive (directly executable) HTN tasks.
 * Primitive tasks represent atomic actions that can be executed directly,
 * without further decomposition.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNPrimitiveTask : public UHTNTask, public IHTNPrimitiveTaskInterface
{
	GENERATED_BODY()

public:
	UHTNPrimitiveTask();

	// Forward declarations for needed classes
	class UHTNCondition;
	class UHTNEffect;

	//~ Begin IHTNPrimitiveTaskInterface
	virtual bool Execute_Implementation(TScriptInterface<IHTNWorldStateInterface>& WorldState) override;
	virtual bool IsComplete_Implementation() const override;
	virtual EHTNTaskStatus GetStatus_Implementation() const override;
	//~ End IHTNPrimitiveTaskInterface

	/**
	 * This will be implemented later with full functionality
	 * when we build the Primitive Task implementation.
	 * For now, this is just a placeholder to satisfy the forward declaration.
	 */
};