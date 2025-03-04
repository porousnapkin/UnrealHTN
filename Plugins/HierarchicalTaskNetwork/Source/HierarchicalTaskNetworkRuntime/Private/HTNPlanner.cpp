// Copyright Epic Games, Inc. All Rights Reserved.

#include "HTNPlanner.h"

#define LOCTEXT_NAMESPACE "FHTNPlannerModule"

void FHTNPlannerModule::StartupModule()
{
	// This code will execute after your module is loaded into memory;
	// the exact timing is specified in the .uplugin file per-module

	UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNPlanner module has been loaded"));
}

void FHTNPlannerModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.
	// For modules that support dynamic reloading, we call this function before unloading the module.
	
	UE_LOG(LogHTNPlannerPlugin, Log, TEXT("HTNPlanner module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE
