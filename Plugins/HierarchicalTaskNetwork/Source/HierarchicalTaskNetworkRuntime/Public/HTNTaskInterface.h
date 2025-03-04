// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HTNWorldStateInterface.h"
#include "UObject/Interface.h"
#include "HTNTaskInterface.generated.h"

// Declare log category for HTN task operations
DECLARE_LOG_CATEGORY_EXTERN(LogHTNTask, Log, All);

/**
 * Enumeration for task execution status.
 * Represents the current state of a task during execution.
 */
UENUM(BlueprintType)
enum class EHTNTaskStatus : uint8
{
	/** Task is currently being executed */
	InProgress UMETA(DisplayName = "In Progress"),
	
	/** Task has completed successfully */
	Succeeded UMETA(DisplayName = "Succeeded"),
	
	/** Task has failed during execution */
	Failed UMETA(DisplayName = "Failed"),
	
	/** Task is in an invalid state and cannot be executed */
	Invalid UMETA(DisplayName = "Invalid")
};

/**
 * Forward declarations for types we'll reference
 */
class UHTNTask;
class UHTNPrimitiveTask;
class UHTNCompoundTask;
class UHTNMethod;

/**
 * Base interface for all HTN tasks.
 * Defines the core functionality that all tasks (primitive and compound) must implement.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UHTNTaskInterface : public UInterface
{
	GENERATED_BODY()
};

class HIERARCHICALTASKNETWORKRUNTIME_API IHTNTaskInterface
{
	GENERATED_BODY()

public:
	/**
	 * Decompose this task into subtasks given a world state.
	 * For primitive tasks, this typically returns just the task itself.
	 * For compound tasks, this involves selecting a method and returning its subtasks.
	 * 
	 * @param WorldState - The current world state to use for decomposition
	 * @param OutTasks - The resulting primitive tasks after decomposition
	 * @return True if decomposition was successful, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	bool Decompose(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNPrimitiveTask*>& OutTasks);

	/**
	 * Check if this task is applicable in the given world state.
	 * This verifies that the task's preconditions are satisfied.
	 * 
	 * @param WorldState - The world state to check against
	 * @return True if the task can be applied in the given world state, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	bool IsApplicable(const TScriptInterface<IHTNWorldStateInterface>& WorldState) const;

	/**
	 * Get the expected effects of executing this task on the world state.
	 * This is used during planning to predict the resulting world state after execution.
	 * 
	 * @param WorldState - The current world state
	 * @param OutEffects - The world state modifications that this task is expected to make
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	void GetExpectedEffects(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TScriptInterface<IHTNWorldStateInterface>& OutEffects) const;

	/**
	 * Get a human-readable description of this task.
	 * Useful for debugging and visualization.
	 * 
	 * @return String description of the task
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	FString GetDescription() const;
};

/**
 * Interface for primitive tasks that can be directly executed.
 * Primitive tasks represent atomic actions that agents can perform.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UHTNPrimitiveTaskInterface : public UHTNTaskInterface
{
	GENERATED_BODY()
};

class HIERARCHICALTASKNETWORKRUNTIME_API IHTNPrimitiveTaskInterface : public IHTNTaskInterface
{
	GENERATED_BODY()

public:
	/**
	 * Begin execution of this primitive task.
	 * 
	 * @param WorldState - The current world state to use during execution
	 * @return True if execution started successfully, false if it failed immediately
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	bool Execute(TScriptInterface<IHTNWorldStateInterface>& WorldState);

	/**
	 * Check if this task has completed execution.
	 * 
	 * @return True if the task has finished (either succeeded or failed), false if still in progress
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	bool IsComplete() const;

	/**
	 * Get the current execution status of this task.
	 * 
	 * @return Current status (InProgress, Succeeded, Failed, or Invalid)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	EHTNTaskStatus GetStatus() const;
};

/**
 * Interface for compound tasks that decompose into other tasks.
 * Compound tasks represent higher-level actions that can be broken down into simpler tasks.
 */
UINTERFACE(MinimalAPI, Blueprintable)
class UHTNCompoundTaskInterface : public UHTNTaskInterface
{
	GENERATED_BODY()
};

class HIERARCHICALTASKNETWORKRUNTIME_API IHTNCompoundTaskInterface : public IHTNTaskInterface
{
	GENERATED_BODY()

public:
	/**
	 * Get all methods (alternative decompositions) that are available for this task.
	 * 
	 * @param WorldState - The current world state
	 * @param OutMethods - The available methods for this compound task
	 * @return True if at least one method is available, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	bool GetAvailableMethods(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<class UHTNMethod*>& OutMethods) const;

	/**
	 * Apply a selected method to decompose this task into subtasks.
	 * 
	 * @param Method - The method to apply
	 * @param WorldState - The current world state
	 * @param OutTasks - The resulting tasks after decomposition
	 * @return True if the method was successfully applied, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HTN|Task")
	bool ApplyMethod(class UHTNMethod* Method, const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNTask*>& OutTasks) const;
};