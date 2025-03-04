// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNTaskInterface.h"
#include "HTNWorldStateInterface.h"
#include "HTNTask.generated.h"

/**
 * Base class for all HTN tasks.
 * Provides common functionality for both primitive and compound tasks.
 * This abstract class implements the IHTNTaskInterface and serves as the
 * foundation for the HTN planning system's task hierarchy.
 */
UCLASS(Abstract, BlueprintType, Blueprintable, hidecategories = (Object))
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNTask : public UObject, public IHTNTaskInterface
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor
	 */
	UHTNTask();

	//~ Begin UObject Interface
	virtual void PostInitProperties() override;
	virtual void BeginDestroy() override;
	//~ End UObject Interface

	//~ Begin IHTNTaskInterface
	virtual bool Decompose_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<class UHTNPrimitiveTask*>& OutTasks) override;
	virtual bool IsApplicable_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState) const override;
	virtual void GetExpectedEffects_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TScriptInterface<IHTNWorldStateInterface>& OutEffects) const override;
	virtual FString GetDescription_Implementation() const override;
	//~ End IHTNTaskInterface

	/**
	 * Validates that the task is set up correctly.
	 * Called during initialization and when properties change.
	 * 
	 * @return True if the task is valid, false otherwise
	 */
	UFUNCTION(BlueprintNativeEvent, Category = "HTN|Task")
	bool ValidateTask() const;

	/**
	 * Gets a string representation of this task for logging.
	 * 
	 * @return A string describing this task
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|Task")
	virtual FString ToString() const;

	/**
	 * Gets the unique ID of this task.
	 * 
	 * @return The task's GUID
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|Task")
	FGuid GetTaskID() const { return TaskID; }

	/**
	 * Gets the cost of this task for planning purposes.
	 * Lower cost tasks are generally preferred when multiple options exist.
	 * 
	 * @return The task's cost
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|Task")
	float GetCost() const { return Cost; }

	/**
	 * Updates the task's display name from the task class name if not explicitly set.
	 */
	void UpdateTaskName();

	/** Display name of the task */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task")
	FName TaskName;

	/** Detailed description of what this task does */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task", meta = (MultiLine = true))
	FString Description;

	/** Cost of this task for planning (lower costs are preferred) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task", meta = (ClampMin = "0.0"))
	float Cost;

	/** Color used for debugging visualization */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	FLinearColor DebugColor;

protected:
	/** Unique identifier for this task instance */
	UPROPERTY(VisibleAnywhere, Category = "Task")
	FGuid TaskID;

	/** Whether this task is currently being debugged */
	UPROPERTY(Transient)
	uint8 bIsDebugging : 1;

private:
	/** Whether this task has been initialized */
	uint8 bInitialized : 1;
};

/**
 * Forward declarations for derived task classes.
 * These will be fully implemented in later steps.
 */
class UHTNPrimitiveTask;
class UHTNCompoundTask;