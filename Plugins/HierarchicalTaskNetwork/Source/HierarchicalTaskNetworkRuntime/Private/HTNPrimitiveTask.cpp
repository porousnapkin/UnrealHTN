// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNPrimitiveTask.h"

UHTNPrimitiveTask::UHTNPrimitiveTask()
	: Super()
{
	// Will be expanded with full implementation later
}

bool UHTNPrimitiveTask::Execute_Implementation(TScriptInterface<IHTNWorldStateInterface>& WorldState)
{
	// Will be implemented fully in a later step
	UE_LOG(LogHTNTask, Warning, TEXT("Execute not implemented for primitive task: %s"), *ToString());
	return false;
}

bool UHTNPrimitiveTask::IsComplete_Implementation() const
{
	// Will be implemented fully in a later step
	return false;
}

EHTNTaskStatus UHTNPrimitiveTask::GetStatus_Implementation() const
{
	// Will be implemented fully in a later step
	return EHTNTaskStatus::Invalid;
}