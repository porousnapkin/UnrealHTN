// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNTask.h"
#include "HTNWorldStateStruct.h"
#include "HTNPrimitiveTask.h"

UHTNTask::UHTNTask()
	: TaskID(FGuid::NewGuid())
	, Cost(1.0f)
	, DebugColor(FLinearColor::White)
	, bIsDebugging(false)
	, bInitialized(false)
{
}

void UHTNTask::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (!HasAnyFlags(RF_ClassDefaultObject | RF_ArchetypeObject) && !bInitialized)
	{
		bInitialized = true;
		
		// Initialize the task name if it's not set
		UpdateTaskName();
		
		// Validate the task configuration
		ValidateTask();
	}
}

void UHTNTask::BeginDestroy()
{
	// Cleanup any resources if needed
	
	Super::BeginDestroy();
}

bool UHTNTask::Decompose(const UHTNWorldState* WorldState, TArray<UHTNPrimitiveTask*>& OutTasks)
{
	// Base implementation - to be overridden by derived classes
	UE_LOG(LogHTNTask, Warning, TEXT("Decompose not implemented for task: %s"), *ToString());
	return false;
}

bool UHTNTask::IsApplicable(const UHTNWorldState* WorldState) const
{
	// Base implementation - default to applicable
	// Derived classes should override this to check preconditions
	return true;
}

UHTNWorldState* UHTNTask::GetExpectedEffects(const UHTNWorldState* WorldState) const
{
	// Create empty effects world state if none provided
	UHTNWorldState* OutEffects = NewObject<UHTNWorldState>(GetTransientPackage());

	return OutEffects;
}

FString UHTNTask::GetDescription() const
{
	// Use the custom description if provided, otherwise use the task name
	return !Description.IsEmpty() ? Description : TaskName.ToString();
}

bool UHTNTask::ValidateTask_Implementation() const
{
	// Base validation - check if task name is set
	if (TaskName.IsNone())
	{
		UE_LOG(LogHTNTask, Warning, TEXT("Task has no name: %s"), *GetClass()->GetName());
		return false;
	}
	
	return true;
}

FString UHTNTask::ToString() const
{
	return FString::Printf(TEXT("[%s] %s (Cost: %.2f)"), 
		*GetClass()->GetName(), 
		*TaskName.ToString(), 
		Cost);
}

void UHTNTask::UpdateTaskName()
{
	if (TaskName.IsNone())
	{
		// Generate a name from the class name
		FString ClassName = GetClass()->GetName();
		
		// Remove common prefixes if present
		if (ClassName.StartsWith(TEXT("UHTN")))
		{
			ClassName.RemoveFromStart(TEXT("UHTN"));
		}
		else if (ClassName.StartsWith(TEXT("HTN")))
		{
			ClassName.RemoveFromStart(TEXT("HTN"));
		}
		
		// Remove "Task" suffix if present
		if (ClassName.EndsWith(TEXT("Task")))
		{
			ClassName.RemoveFromEnd(TEXT("Task"));
		}
		
		// Convert from camel case to readable format
		FString ReadableName;
		for (int32 CharIndex = 0; CharIndex < ClassName.Len(); CharIndex++)
		{
			const TCHAR CurrentChar = ClassName[CharIndex];
			
			// Add a space before uppercase letters that follow lowercase letters
			if (CharIndex > 0 && FChar::IsUpper(CurrentChar) && FChar::IsLower(ClassName[CharIndex - 1]))
			{
				ReadableName.AppendChar(' ');
			}
			
			ReadableName.AppendChar(CurrentChar);
		}
		
		TaskName = FName(*ReadableName);
	}
}