// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "HTNTask.h"
#include "HTNPrimitiveTask.h"
#include "HTNCompoundTask.h"
#include "HTNTaskFactory.generated.h"

/**
 * Factory class for creating and registering HTN tasks.
 * This class manages a registry of available task types and provides methods for creating task instances.
 */
UCLASS(Blueprintable)
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNTaskFactory : public UObject
{
    GENERATED_BODY()

public:
    UHTNTaskFactory();
    virtual ~UHTNTaskFactory();

    /**
     * Gets the task factory singleton instance.
     * 
     * @return The singleton task factory instance
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory", meta = (DisplayName = "Get Task Factory"))
    static UHTNTaskFactory* Get();

    /**
     * Registers a primitive task class with the factory.
     * 
     * @param TaskClass - The class to register
     * @param Category - Optional category for organizing tasks in the editor
     * @return True if the registration was successful
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    bool RegisterPrimitiveTaskClass(TSubclassOf<UHTNPrimitiveTask> TaskClass, FName Category = NAME_None);

    /**
     * Registers a compound task class with the factory.
     * 
     * @param TaskClass - The class to register
     * @param Category - Optional category for organizing tasks in the editor
     * @return True if the registration was successful
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    bool RegisterCompoundTaskClass(TSubclassOf<UHTNCompoundTask> TaskClass, FName Category = NAME_None);
 
     // Helper template function for creating tasks
     template<typename TaskType>
     TaskType* CreateTask(TSubclassOf<TaskType> TaskClass, UObject* Outer, FName TaskName);
 
    /**
     * Creates a new instance of a primitive task.
     * 
     * @param TaskClass - The class to instantiate
     * @param Outer - The outer object for the new task (defaults to transient package)
     * @param TaskName - Optional name for the new task
     * @return The created task instance
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    UHTNPrimitiveTask* CreatePrimitiveTask(TSubclassOf<UHTNPrimitiveTask> TaskClass, UObject* Outer = nullptr, FName TaskName = NAME_None);

    /**
     * Creates a new instance of a compound task.
     * 
     * @param TaskClass - The class to instantiate
     * @param Outer - The outer object for the new task (defaults to transient package)
     * @param TaskName - Optional name for the new task
     * @return The created task instance
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    UHTNCompoundTask* CreateCompoundTask(TSubclassOf<UHTNCompoundTask> TaskClass, UObject* Outer = nullptr, FName TaskName = NAME_None);

    /**
     * Gets all registered primitive task classes.
     * 
     * @return Array of registered primitive task classes
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    TArray<TSubclassOf<UHTNPrimitiveTask>> GetRegisteredPrimitiveTaskClasses() const;

    /**
     * Gets all registered compound task classes.
     * 
     * @return Array of registered compound task classes
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    TArray<TSubclassOf<UHTNCompoundTask>> GetRegisteredCompoundTaskClasses() const;

    /**
     * Gets primitive task classes by category.
     * 
     * @param Category - The category to filter by
     * @return Array of primitive task classes in the specified category
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    TArray<TSubclassOf<UHTNPrimitiveTask>> GetPrimitiveTaskClassesByCategory(FName Category) const;

    /**
     * Gets compound task classes by category.
     * 
     * @param Category - The category to filter by
     * @return Array of compound task classes in the specified category
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    TArray<TSubclassOf<UHTNCompoundTask>> GetCompoundTaskClassesByCategory(FName Category) const;

    /**
     * Gets all registered task categories.
     * 
     * @return Array of registered task categories
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    TArray<FName> GetTaskCategories() const;

    /**
     * Validates a task instance to ensure it's properly configured.
     * 
     * @param Task - The task to validate
     * @return True if the task is valid, false otherwise
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory")
    bool ValidateTask(UHTNTask* Task) const;

private:
    // Static singleton instance
    static UHTNTaskFactory* Instance;

    // Registry of primitive task classes
    UPROPERTY()
    TMap<TSubclassOf<UHTNPrimitiveTask>, FName> RegisteredPrimitiveTaskClasses;

    // Registry of compound task classes
    UPROPERTY()
    TMap<TSubclassOf<UHTNCompoundTask>, FName> RegisteredCompoundTaskClasses;

    // Registry of all task categories
    UPROPERTY()
    TSet<FName> TaskCategories;
};

/**
 * Blueprint function library for HTN Task Factory operations.
 * Provides static functions for working with the task factory from Blueprint.
 */
UCLASS()
class HIERARCHICALTASKNETWORKRUNTIME_API UHTNTaskFactoryLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Creates a new primitive task of the specified class.
     * 
     * @param TaskClass - The class to instantiate
     * @param TaskName - Optional name for the new task
     * @return The created task instance
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory", meta = (DisplayName = "Create Primitive Task"))
    static UHTNPrimitiveTask* CreatePrimitiveTask(TSubclassOf<UHTNPrimitiveTask> TaskClass, FName TaskName = NAME_None);

    /**
     * Creates a new compound task of the specified class.
     * 
     * @param TaskClass - The class to instantiate
     * @param TaskName - Optional name for the new task
     * @return The created task instance
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory", meta = (DisplayName = "Create Compound Task"))
    static UHTNCompoundTask* CreateCompoundTask(TSubclassOf<UHTNCompoundTask> TaskClass, FName TaskName = NAME_None);

    /**
     * Gets all registered primitive task classes.
     * 
     * @return Array of registered primitive task classes
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory", meta = (DisplayName = "Get Registered Primitive Task Classes"))
    static TArray<TSubclassOf<UHTNPrimitiveTask>> GetRegisteredPrimitiveTaskClasses();

    /**
     * Gets all registered compound task classes.
     * 
     * @return Array of registered compound task classes
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory", meta = (DisplayName = "Get Registered Compound Task Classes"))
    static TArray<TSubclassOf<UHTNCompoundTask>> GetRegisteredCompoundTaskClasses();

    /**
     * Gets all task categories.
     * 
     * @return Array of task categories
     */
    UFUNCTION(BlueprintCallable, Category = "HTN|TaskFactory", meta = (DisplayName = "Get Task Categories"))
    static TArray<FName> GetTaskCategories();
};


template <typename TaskType>
FORCEINLINE TaskType* UHTNTaskFactory::CreateTask(TSubclassOf<TaskType> TaskClass, UObject* Outer, FName TaskName)
{
 if (!TaskClass)
 {
  UE_LOG(LogTemp, Warning, TEXT("Attempted to create task with null class"));
  return nullptr;
 }

 // Use transient package if no outer is specified
 if (!Outer)
 {
  Outer = GetTransientPackage();
 }

 // Create a new instance of the task
 TaskType* NewTask = NewObject<TaskType>(Outer, TaskClass);

 // Set the name if provided
 if (!TaskName.IsNone())
 {
  NewTask->TaskName = TaskName;
 }

 // Validate the task
 if (!ValidateTask(NewTask))
 {
  UE_LOG(LogTemp, Warning, TEXT("Created task %s is invalid"), *NewTask->ToString());
 }

 return NewTask;
}
