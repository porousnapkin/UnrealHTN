// Copyright Epic Games, Inc. All Rights Reserved.

#include "HierarchicalTaskNetworkRuntime.h"
#include "Tasks/HTNTaskRegistry.h"
#include "HTNLogging.h"

#define LOCTEXT_NAMESPACE "FHierarchicalTaskNetworkModule"

void FHierarchicalTaskNetworkRuntimeModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
    
	// Initialize the HTN task registry
	UHTNTaskRegistry::Get()->Initialize();
    
	UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Hierarchical Task Network Runtime module has been loaded"));
}

void FHierarchicalTaskNetworkRuntimeModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
    
	UE_LOG(LogHTNPlannerPlugin, Log, TEXT("Hierarchical Task Network Runtime module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FHierarchicalTaskNetworkRuntimeModule, HierarchicalTaskNetworkRuntime)