// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tasks/HTNTaskFactory.h"
#include "Tasks/HTNPrimitiveTask.h"
#include "Tasks/HTNCompoundTask.h"
#include "HTNPlanner.h"
#include "UObject/UObjectGlobals.h"

// Initialize static instance to nullptr
UHTNTaskFactory* UHTNTaskFactory::Instance = nullptr;

UHTNTaskFactory::UHTNTaskFactory()
{
    // If the instance is already set, log a warning
    if (Instance && Instance != this)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Multiple HTNTaskFactory instances detected. This is not recommended."));
    }
    
    // Set this as the current instance
    Instance = this;
}

UHTNTaskFactory::~UHTNTaskFactory()
{
    // If this is the current instance, clear it
    if (Instance == this)
    {
        Instance = nullptr;
    }
}

UHTNTaskFactory* UHTNTaskFactory::Get()
{
    // If we don't have an instance yet, create one
    if (!Instance)
    {
        Instance = NewObject<UHTNTaskFactory>(GetTransientPackage());
        Instance->AddToRoot(); // Prevent garbage collection
    }
    
    return Instance;
}

bool UHTNTaskFactory::RegisterPrimitiveTaskClass(TSubclassOf<UHTNPrimitiveTask> TaskClass, FName Category)
{
    if (!TaskClass)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Attempted to register null primitive task class"));
        return false;
    }
    
    // Check if the class is already registered
    if (RegisteredPrimitiveTaskClasses.Contains(TaskClass))
    {
        // Update the category if different
        if (RegisteredPrimitiveTaskClasses[TaskClass] != Category)
        {
            RegisteredPrimitiveTaskClasses[TaskClass] = Category;
            UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Updated category for primitive task class %s to %s"), *TaskClass->GetName(), *Category.ToString());
        }
        
        return true;
    }
    
    // Add the class to the registry
    RegisteredPrimitiveTaskClasses.Add(TaskClass, Category);
    
    // Add the category to the set (if not already present and not None)
    if (!Category.IsNone())
    {
        TaskCategories.Add(Category);
    }
    
    UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Registered primitive task class: %s in category: %s"), *TaskClass->GetName(), *Category.ToString());
    return true;
}

bool UHTNTaskFactory::RegisterCompoundTaskClass(TSubclassOf<UHTNCompoundTask> TaskClass, FName Category)
{
    if (!TaskClass)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Attempted to register null compound task class"));
        return false;
    }
    
    // Check if the class is already registered
    if (RegisteredCompoundTaskClasses.Contains(TaskClass))
    {
        // Update the category if different
        if (RegisteredCompoundTaskClasses[TaskClass] != Category)
        {
            RegisteredCompoundTaskClasses[TaskClass] = Category;
            UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Updated category for compound task class %s to %s"), *TaskClass->GetName(), *Category.ToString());
        }
        
        return true;
    }
    
    // Add the class to the registry
    RegisteredCompoundTaskClasses.Add(TaskClass, Category);
    
    // Add the category to the set (if not already present and not None)
    if (!Category.IsNone())
    {
        TaskCategories.Add(Category);
    }
    
    UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Registered compound task class: %s in category: %s"), *TaskClass->GetName(), *Category.ToString());
    return true;
}

UHTNPrimitiveTask* UHTNTaskFactory::CreatePrimitiveTask(TSubclassOf<UHTNPrimitiveTask> TaskClass, UObject* Outer, FName TaskName)
{
    return CreateTask<UHTNPrimitiveTask>(TaskClass, Outer, TaskName);
}

UHTNCompoundTask* UHTNTaskFactory::CreateCompoundTask(TSubclassOf<UHTNCompoundTask> TaskClass, UObject* Outer, FName TaskName)
{
    return CreateTask<UHTNCompoundTask>(TaskClass, Outer, TaskName);
}

TArray<TSubclassOf<UHTNPrimitiveTask>> UHTNTaskFactory::GetRegisteredPrimitiveTaskClasses() const
{
    TArray<TSubclassOf<UHTNPrimitiveTask>> Result;
    RegisteredPrimitiveTaskClasses.GetKeys(Result);
    return Result;
}

TArray<TSubclassOf<UHTNCompoundTask>> UHTNTaskFactory::GetRegisteredCompoundTaskClasses() const
{
    TArray<TSubclassOf<UHTNCompoundTask>> Result;
    RegisteredCompoundTaskClasses.GetKeys(Result);
    return Result;
}

TArray<TSubclassOf<UHTNPrimitiveTask>> UHTNTaskFactory::GetPrimitiveTaskClassesByCategory(FName Category) const
{
    TArray<TSubclassOf<UHTNPrimitiveTask>> Result;
    
    for (const auto& Pair : RegisteredPrimitiveTaskClasses)
    {
        if (Pair.Value == Category)
        {
            Result.Add(Pair.Key);
        }
    }
    
    return Result;
}

TArray<TSubclassOf<UHTNCompoundTask>> UHTNTaskFactory::GetCompoundTaskClassesByCategory(FName Category) const
{
    TArray<TSubclassOf<UHTNCompoundTask>> Result;
    
    for (const auto& Pair : RegisteredCompoundTaskClasses)
    {
        if (Pair.Value == Category)
        {
            Result.Add(Pair.Key);
        }
    }
    
    return Result;
}

TArray<FName> UHTNTaskFactory::GetTaskCategories() const
{
    TArray<FName> Result = TaskCategories.Array();
    return Result;
}

bool UHTNTaskFactory::ValidateTask(UHTNTask* Task) const
{
    if (!Task)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Attempted to validate null task"));
        return false;
    }
    
    // Call the task's validation method
    return Task->ValidateTask();
}

// Blueprint Function Library Implementation

UHTNPrimitiveTask* UHTNTaskFactoryLibrary::CreatePrimitiveTask(TSubclassOf<UHTNPrimitiveTask> TaskClass, FName TaskName)
{
    return UHTNTaskFactory::Get()->CreatePrimitiveTask(TaskClass, nullptr, TaskName);
}

UHTNCompoundTask* UHTNTaskFactoryLibrary::CreateCompoundTask(TSubclassOf<UHTNCompoundTask> TaskClass, FName TaskName)
{
    return UHTNTaskFactory::Get()->CreateCompoundTask(TaskClass, nullptr, TaskName);
}

TArray<TSubclassOf<UHTNPrimitiveTask>> UHTNTaskFactoryLibrary::GetRegisteredPrimitiveTaskClasses()
{
    return UHTNTaskFactory::Get()->GetRegisteredPrimitiveTaskClasses();
}

TArray<TSubclassOf<UHTNCompoundTask>> UHTNTaskFactoryLibrary::GetRegisteredCompoundTaskClasses()
{
    return UHTNTaskFactory::Get()->GetRegisteredCompoundTaskClasses();
}

TArray<FName> UHTNTaskFactoryLibrary::GetTaskCategories()
{
    return UHTNTaskFactory::Get()->GetTaskCategories();
}