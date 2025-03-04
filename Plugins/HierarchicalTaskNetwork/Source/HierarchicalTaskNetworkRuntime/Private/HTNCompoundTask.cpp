// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNCompoundTask.h"

UHTNCompoundTask::UHTNCompoundTask()
	: Super()
{
	// Will be expanded with full implementation later
}

bool UHTNCompoundTask::GetAvailableMethods_Implementation(const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNMethod*>& OutMethods) const
{
	// Will be implemented fully in a later step
	UE_LOG(LogHTNTask, Warning, TEXT("GetAvailableMethods not implemented for compound task: %s"), *ToString());
	return false;
}

bool UHTNCompoundTask::ApplyMethod_Implementation(UHTNMethod* Method, const TScriptInterface<IHTNWorldStateInterface>& WorldState, TArray<UHTNTask*>& OutTasks) const
{
	// Will be implemented fully in a later step
	UE_LOG(LogHTNTask, Warning, TEXT("ApplyMethod not implemented for compound task: %s"), *ToString());
	return false;
}