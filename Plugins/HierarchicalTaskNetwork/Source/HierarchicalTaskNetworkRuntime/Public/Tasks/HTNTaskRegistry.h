// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNTaskFactory.h"
#include "HTNTaskRegistry.generated.h"

/**
 * Task registry subsystem for the HTN planner.
 * Manages the registration of built-in and custom HTN tasks.
 * Automatically registers tasks when the module starts up.
 */
UCLASS()
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNTaskRegistry : public UObject
{
	GENERATED_BODY()

public:
	UHTNTaskRegistry();
	virtual ~UHTNTaskRegistry();

	/**
	 * Initialize the task registry.
	 * This automatically registers all built-in tasks and any custom tasks found in loaded modules.
	 */
	void Initialize();

	/**
	 * Register the default built-in tasks that come with the HTN planner.
	 */
	void RegisterBuiltInTasks();

	/**
	 * Find and register any custom tasks defined in loaded modules.
	 */
	void RegisterCustomTasks();

	/**
	 * Gets the task registry singleton instance.
	 * 
	 * @return The singleton task registry instance
	 */
	UFUNCTION(BlueprintCallable, Category = "HTN|TaskRegistry", meta = (DisplayName = "Get Task Registry"))
	static UHTNTaskRegistry* Get();

private:
	// Register a primitive task with the task factory
	void RegisterPrimitiveTask(TSubclassOf<UHTNPrimitiveTask> TaskClass, FName Category);

	// Register a compound task with the task factory
	void RegisterCompoundTask(TSubclassOf<UHTNCompoundTask> TaskClass, FName Category);

	// Static singleton instance
	static UHTNTaskRegistry* Instance;

	// Flag to track if the registry has been initialized
	bool bInitialized;

	// Task factory reference
	UPROPERTY()
	UHTNTaskFactory* TaskFactory;
};