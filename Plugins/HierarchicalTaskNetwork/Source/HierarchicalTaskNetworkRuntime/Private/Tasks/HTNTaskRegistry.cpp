// Copyright Epic Games, Inc. All Rights Reserved.

#include "Tasks/HTNTaskRegistry.h"
#include "Tasks/HTNTaskFactory.h"
#include "HTNPlanner.h"
#include "UObject/UObjectIterator.h"
#include "UObject/UObjectHash.h"
#include "UObject/Package.h"

// Initialize static instance to nullptr
UHTNTaskRegistry* UHTNTaskRegistry::Instance = nullptr;

UHTNTaskRegistry::UHTNTaskRegistry()
    : bInitialized(false)
{
    // If the instance is already set, log a warning
    if (Instance && Instance != this)
    {
        UE_LOG(LogHTNPlannerPlugin, Warning, TEXT("Multiple HTNTaskRegistry instances detected. This is not recommended."));
    }
    
    // Set this as the current instance
    Instance = this;

    // Get the task factory
    TaskFactory = UHTNTaskFactory::Get();
}

UHTNTaskRegistry::~UHTNTaskRegistry()
{
    // If this is the current instance, clear it
    if (Instance == this)
    {
        Instance = nullptr;
    }
}

void UHTNTaskRegistry::Initialize()
{
    if (bInitialized)
    {
        return;
    }

    // Register built-in tasks
    RegisterBuiltInTasks();

    // Register any custom tasks found in loaded modules
    RegisterCustomTasks();

    bInitialized = true;
    UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTN Task Registry initialized successfully"));
}

void UHTNTaskRegistry::RegisterBuiltInTasks()
{
    UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Registering built-in HTN tasks"));

    // Register primitive tasks
    // These will be added as implementations of primitive tasks are created
    // Example:
    // RegisterPrimitiveTask(UHTNMoveToTask::StaticClass(), TEXT("Navigation"));

    // Register compound tasks
    // These will be added as implementations of compound tasks are created
    // Example:
    // RegisterCompoundTask(UHTNPatrolTask::StaticClass(), TEXT("Movement"));
}

void UHTNTaskRegistry::RegisterCustomTasks()
{
    UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Scanning for custom HTN tasks"));

    // Find all classes derived from UHTNPrimitiveTask
    for (TObjectIterator<UClass> It; It; ++It)
    {
        UClass* Class = *It;

        // Skip abstract classes, native classes (built-in), and classes in transient packages
        if (Class->HasAnyClassFlags(CLASS_Abstract | CLASS_Native) || 
            Class->GetOutermost()->HasAnyPackageFlags(PKG_CompiledIn))
        {
            continue;
        }

        // Register primitive tasks
        if (Class->IsChildOf(UHTNPrimitiveTask::StaticClass()))
        {
            FName Category = NAME_None;

            // Try to determine the category from metadata or other sources
            // This could be extended to read category from a custom metadata tag
            const FString ClassName = Class->GetName();
            if (ClassName.Contains(TEXT("Movement")) || ClassName.Contains(TEXT("Navigate")))
            {
                Category = TEXT("Movement");
            }
            else if (ClassName.Contains(TEXT("Animation")) || ClassName.Contains(TEXT("Anim")))
            {
                Category = TEXT("Animation");
            }
            else if (ClassName.Contains(TEXT("Wait")) || ClassName.Contains(TEXT("Delay")))
            {
                Category = TEXT("Utility");
            }

            RegisterPrimitiveTask(Class, Category);
        }
        // Register compound tasks
        else if (Class->IsChildOf(UHTNCompoundTask::StaticClass()))
        {
            FName Category = NAME_None;

            // Try to determine the category from metadata or other sources
            const FString ClassName = Class->GetName();
            if (ClassName.Contains(TEXT("Patrol")) || ClassName.Contains(TEXT("Follow")))
            {
                Category = TEXT("Movement");
            }
            else if (ClassName.Contains(TEXT("Combat")) || ClassName.Contains(TEXT("Attack")))
            {
                Category = TEXT("Combat");
            }
            else if (ClassName.Contains(TEXT("Interact")) || ClassName.Contains(TEXT("Use")))
            {
                Category = TEXT("Interaction");
            }

            RegisterCompoundTask(Class, Category);
        }
    }
}

UHTNTaskRegistry* UHTNTaskRegistry::Get()
{
    // If we don't have an instance yet, create one
    if (!Instance)
    {
        Instance = NewObject<UHTNTaskRegistry>(GetTransientPackage());
        Instance->AddToRoot(); // Prevent garbage collection
        Instance->Initialize();
    }
    
    return Instance;
}

void UHTNTaskRegistry::RegisterPrimitiveTask(TSubclassOf<UHTNPrimitiveTask> TaskClass, FName Category)
{
    if (!TaskClass)
    {
        return;
    }

    TaskFactory->RegisterPrimitiveTaskClass(TaskClass, Category);
}

void UHTNTaskRegistry::RegisterCompoundTask(TSubclassOf<UHTNCompoundTask> TaskClass, FName Category)
{
    if (!TaskClass)
    {
        return;
    }

    TaskFactory->RegisterCompoundTaskClass(TaskClass, Category);
}